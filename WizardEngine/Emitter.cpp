#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(Mesh* mesh, Material* material, ID3D11Device* device, ID3D11ShaderResourceView* texture) : Entity(mesh, material)
{
	this->texture = texture;
	maxParticles = 1000;

	vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

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

Entity * Emitter::SetPosition(DirectX::XMFLOAT3 a_vPos)
{
	//update particle pos
	particlePos = a_vPos;

	//base class call
	Entity::SetPosition(a_vPos);
	return this;
}

bool Emitter::Update(float delt)
{
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

	Entity::Update(delt);
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

	particleVS->SetMatrix4x4("view", camera->GetViewMatrix());
	particleVS->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	particleVS->SetShader();
	particleVS->CopyAllBufferData();

	particlePS->SetShaderResourceView("particle", texture);
	particlePS->SetShader();
	particlePS->CopyAllBufferData();

	if (firstAliveIndex < firstDeadIndex) {
		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
	}
	else {
		context->DrawIndexed(firstDeadIndex *6, 0, 0);

		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
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


