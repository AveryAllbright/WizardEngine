#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(Mesh* mesh, Material* material, int type, XMFLOAT3 velocity, XMFLOAT3 pos, XMFLOAT3 scale, SimpleVertexShader* particleVS, SimplePixelShader* particlePS, ID3D11Device* device, ID3D11ShaderResourceView* texture) : Entity(mesh, material)
{
	this->type = type;
	this->texture = texture;
	m_vPos = pos;
	particlePos = pos;
	m_vScale = scale;
	m_vRotation = XMFLOAT3(0, 0, 0);
	this->velocity = velocity;

	//set up particles
	if (type == 0) {
		startColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
		endColor = XMFLOAT4(10.0f, 10.2f, 10.0f, 0);
		startVelocity = XMFLOAT3(-.01f, .01f, 0);
		emitterAcceleration = XMFLOAT3(0, 0, 0);
		maxParticles = 1000;
		particlesPerSecond = 100;
		secondsPerParticle = .001f;
		lifetime = .7f;
		startSize = .04f;
		endSize = .08f;
	}
	else if (type == 1) {
		startColor = XMFLOAT4(120, 42, 42, 0.2f);
		endColor = XMFLOAT4(120, 42, 42, 0);
		startVelocity = XMFLOAT3(-1, 1, -1);
		emitterAcceleration = XMFLOAT3(0, -1, 0);
		maxParticles = 1000;
		particlesPerSecond = 100;
		secondsPerParticle = .01f;
		lifetime = .8f;
		startSize = .8f;
		endSize = 2;
		particlePos.y = m_vPos.y + 2.5f;
		wallFinal = m_vPos;
		wallFinal.y = m_vPos.y + 2.5f;
	}

	vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	this->ParticleVS = particleVS;
	this->ParticlePS = particlePS;

	timeSinceEmitt = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;
	livingParticleCount = 0;

	particles = new Particle[maxParticles];

	//local particle info
	localVertices = new ParticleVertex[4 * maxParticles];
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		localVertices[i + 0].UV = XMFLOAT2(0, 0);
		localVertices[i + 1].UV = XMFLOAT2(1, 0);
		localVertices[i + 2].UV = XMFLOAT2(1, 1);
		localVertices[i + 3].UV = XMFLOAT2(0, 1);
	}

	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;
}


Emitter::~Emitter()
{
	delete[] particles;
	delete[] localVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

bool Emitter::UpdateEmitters(float delt, float speed, float speed2)
{
	
	if (type == 0) {
		UpdateFireball(delt, speed);
	}
	else if (type == 1) {
		UpdateWall(delt, speed2);
	}

	if (firstAliveIndex < firstDeadIndex) {
		for (int i = firstAliveIndex; i < firstDeadIndex; i++) {
			updateSingleParticle(delt, i);
		}
	}
	else {
		for (int i = firstAliveIndex; i < maxParticles; i++) {
			updateSingleParticle(delt, i);
		}
		for (int i = 0; i < firstDeadIndex; i++) {
			updateSingleParticle(delt, i);
		}
	}

	timeSinceEmitt += delt;

	while (timeSinceEmitt > secondsPerParticle) {
		SpawnParticle();
		timeSinceEmitt -= secondsPerParticle;
	}


	return false;
}

void Emitter::SpawnParticle()
{
	if (livingParticleCount == maxParticles) {
		return;
	}

	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].Position = particlePos;
	particles[firstDeadIndex].StartVel = startVelocity;
	particles[firstDeadIndex].StartVel.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	if (type == 1) {
		startVelocity.x = 0 - startVelocity.x;
		startVelocity.z = 0 - startVelocity.z;
	}

	livingParticleCount++;
}

void Emitter::CopyParticlesToGPU(ID3D11DeviceContext * context)
{

	if (firstAliveIndex < firstDeadIndex) {
		for (int i = firstAliveIndex; i < firstDeadIndex; i++) {
			CopyOneParticle(i);
		}
	}
	else {
		for (int i = firstAliveIndex; i < maxParticles; i++) {
			CopyOneParticle(i);
		}
		for (int i = 0; i < firstDeadIndex; i++) {
			CopyOneParticle(i);
		}
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, localVertices, sizeof(ParticleVertex) * 4 * maxParticles);

	context->Unmap(vertexBuffer, 0);


}

void Emitter::CopyOneParticle(int index)
{
	int i = index * 4;

	localVertices[i + 0].Position = particles[index].Position;
	localVertices[i + 1].Position = particles[index].Position;
	localVertices[i + 2].Position = particles[index].Position;
	localVertices[i + 3].Position = particles[index].Position;

	localVertices[i + 0].Size = particles[index].Size;
	localVertices[i + 1].Size = particles[index].Size;
	localVertices[i + 2].Size = particles[index].Size;
	localVertices[i + 3].Size = particles[index].Size;

	localVertices[i + 0].Color = particles[index].Color;
	localVertices[i + 1].Color = particles[index].Color;
	localVertices[i + 2].Color = particles[index].Color;
	localVertices[i + 3].Color = particles[index].Color;
}

void Emitter::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	CopyParticlesToGPU(context);

	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	ParticleVS->SetMatrix4x4("view", camera->GetViewMatrix());
	ParticleVS->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	ParticleVS->SetShader();
	ParticleVS->CopyAllBufferData();

	ParticlePS->SetShaderResourceView("particle", texture);
	ParticlePS->SetShader();
	ParticlePS->CopyAllBufferData();

	if (firstAliveIndex < firstDeadIndex) {
		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
	}
	else {
		context->DrawIndexed(firstDeadIndex *6, 0, 0);

		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
	}
}

void Emitter::UpdateFireball(float delt, float speed)
{
	XMVECTOR vecOne = XMLoadFloat3(&m_vPos);
	XMVECTOR vecTwo = XMLoadFloat3(&velocity);
	vecTwo = DirectX::XMVectorScale(vecTwo, delt * speed);
	XMVECTOR vecFinal = XMVectorAdd(vecOne, vecTwo);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, vecFinal);
	m_vPos = temp;
	particlePos = temp;
	outdatedMatrix = true;

}

void Emitter::UpdateWall(float delt, float speed2)
{
	XMVECTOR vecOne = XMLoadFloat3(&m_vPos);
		XMVECTOR vecTwo = XMLoadFloat3(&velocity);
		vecTwo = DirectX::XMVectorScale(vecTwo, delt * speed2);

		XMVECTOR vecFinal = XMVectorAdd(vecOne, vecTwo);
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, vecFinal);
		m_vPos = temp;
		vecOne = XMLoadFloat3(&particlePos);
		vecTwo = XMLoadFloat3(&velocity);
		vecTwo = DirectX::XMVectorScale(vecTwo, delt * speed2);

		vecFinal = XMVectorAdd(vecOne, -vecTwo);
		temp;
		XMStoreFloat3(&temp, vecFinal);
		particlePos = temp;
		
		outdatedMatrix = true;

		if (GetPosition().y > wallFinal.y)
		{
			velocity = XMFLOAT3(0, 0, 0);
			startSize = 0;
			endSize = 0;
		}
}

void Emitter::updateSingleParticle(float delt, int i)
{
	if (particles[i].Age >= lifetime) {
		return;
	}

	particles[i].Age += delt;
	if (particles[i].Age >= lifetime) {
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}

	float agePercent = particles[i].Age / lifetime;

	XMStoreFloat4(&particles[i].Color, XMVectorLerp(XMLoadFloat4(&startColor), XMLoadFloat4(&endColor), agePercent));

	particles[i].Size = startSize + agePercent * (endSize - startSize);

	XMVECTOR startPos = XMLoadFloat3(&particlePos);
	XMVECTOR startVel = XMLoadFloat3(&particles[i].StartVel);
	XMVECTOR accel = XMLoadFloat3(&emitterAcceleration);
	float t = particles[i].Age;

	XMStoreFloat3(&particles[i].Position, accel * t * t / 2.0f + startVel * t + startPos);
}


