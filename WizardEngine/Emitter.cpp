#include "Emitter.h"

using namespace DirectX;


DirectX::XMFLOAT3 Emitter::GetPosition()
{
	return m_vPos;
}

DirectX::XMFLOAT3 Emitter::GetRotation()
{
	return m_vRotation;
}

DirectX::XMFLOAT3 Emitter::GetScale()
{
	return m_vScale;
}

DirectX::XMFLOAT4X4 Emitter::GetWorldMatrix()
{
	if (outdatedMatrix) {
		UpdateWorldView();
		outdatedMatrix = false;
	}
	return m_mWorld;
}

void Emitter::PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj)
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	SimpleVertexShader* vertexShader = material->GetVertShader();
	SimplePixelShader*  pixelShader = material->GetPixelShader();
	vertexShader->SetShader();
	pixelShader->SetShader();

	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	vertexShader->SetMatrix4x4("world", GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", a_view);
	vertexShader->SetMatrix4x4("projection", a_proj);

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vertexShader->CopyAllBufferData();

	//pixelShader->SetSamplerState("basicSampler", material->samplerState);
	//pixelShader->SetShaderResourceView("diffuseTexture", material->shaderResourceView);
	//pixelShader->CopyAllBufferData();
}

Emitter::Emitter(Mesh* mesh, Material* material, int type, XMFLOAT3 velocity, XMFLOAT3 pos, XMFLOAT3 scale, SimpleVertexShader* particleVS, SimplePixelShader* particlePS, ID3D11Device* device, ID3D11ShaderResourceView* texture)
{
	this->mesh = mesh;
	this->material = material;
	this->type = type;
	this->texture = texture;
	components = std::vector<Component*>();
	m_vPos = pos;
	m_vScale = scale;
	m_vRotation = XMFLOAT3(0, 0, 0);
	this->velocity = velocity;
	UpdateWorldView();

	//set up particles
	if (type == 0) {
		startColor = XMFLOAT4(150.0f, 0.0f, 0.0f, 0.2f);
		endColor = XMFLOAT4(20.0f, 0.2f, 0.0f, 0);
		startVelocity = XMFLOAT3(-.01, .01, 0);
		emitterAcceleration = XMFLOAT3(0, -.5, 0);
		maxParticles = 1000;
		particlesPerSecond = 100;
		secondsPerParticle = .001f;
		lifetime = .7;
		startSize = .04;
		endSize = .08;
	}
	else if (type == 1) {
		startColor = XMFLOAT4(1, 0.1f, 0.1f, 0.2f);
		endColor = XMFLOAT4(1, 0.6f, 0.1f, 0);
		startVelocity = XMFLOAT3(-2, 2, 0);
		emitterAcceleration = XMFLOAT3(0, -1, 0);
		maxParticles = 1000;
		particlesPerSecond = 100;
		secondsPerParticle = 1.0f;
		lifetime = 5;
		startSize = .1;
		endSize = 5;
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
	for (int i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
	delete[] particles;
	delete[] localVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

Emitter* Emitter::SetPosition(XMFLOAT3 a_vPos)
{
	m_vPos = a_vPos;
	outdatedMatrix = true;
	return this;
}

Emitter* Emitter::SetRotation(XMFLOAT3 a_vRotation)
{
	m_vRotation = a_vRotation;
	outdatedMatrix = true;
	return this;
}

Emitter* Emitter::SetScale(XMFLOAT3 a_vScale)
{
	m_vScale = a_vScale;
	outdatedMatrix = true;
	return this;
}

void Emitter::UpdateWorldView()
{
	XMMATRIX tr = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
	XMMATRIX ro = XMMatrixRotationRollPitchYaw(m_vRotation.x, m_vRotation.y, m_vRotation.z);
	XMMATRIX sc = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);

	XMStoreFloat4x4(&m_mWorld, XMMatrixTranspose(sc * ro * tr));
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
	particles[firstDeadIndex].Position = m_vPos;
	particles[firstDeadIndex].StartVel = startVelocity;
	particles[firstDeadIndex].StartVel.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

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
		outdatedMatrix = true;

		if (GetPosition().y > -1.3)
		{
			velocity = XMFLOAT3(0, 0, 0);
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

	XMVECTOR startPos = XMLoadFloat3(&m_vPos);
	XMVECTOR startVel = XMLoadFloat3(&particles[i].StartVel);
	XMVECTOR accel = XMLoadFloat3(&emitterAcceleration);
	float t = particles[i].Age;

	XMStoreFloat3(&particles[i].Position, accel * t * t / 2.0f + startVel * t + startPos);
}


