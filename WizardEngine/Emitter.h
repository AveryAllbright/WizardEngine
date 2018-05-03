#pragma once
class Component;
class Collider;
#include "Object.h"
#include "Mesh.h"
#include "Material.h"
#include "DXCore.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Entity.h"
#include "Camera.h"
#include "Game.h"
#include "SimpleShader.h"

struct Particle{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartVel;
	float Size;
	float Age;
};

struct ParticleVertex {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
	float Size;
};

class Emitter :
	public Entity
{
public:
	//TODO: temp
	Emitter(Mesh* a_pMesh, Material* a_pMaterial, ID3D11Device* device, ID3D11ShaderResourceView* texture);
	~Emitter();

	virtual bool Update(float delta);

	virtual void SpawnParticle();

	void CopyParticlesToGPU(ID3D11DeviceContext* context);
	void CopyOneParticle(int index);

	// need to override to update particle's pos as well
	virtual Entity* SetPosition(DirectX::XMFLOAT3 a_vPos);

	void Draw(ID3D11DeviceContext* context, Camera* camera);

	SimpleVertexShader* ParticleVS;
	SimplePixelShader* ParticlePS;
protected:
	DirectX::XMFLOAT3 particlePos;
	DirectX::XMFLOAT3 wallFinal;

	// emitters
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	DirectX::XMFLOAT3 startVelocity;
	DirectX::XMFLOAT3 emitterAcceleration;


	
	int maxParticles;
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmitt;
	float lifetime;
	float startSize;
	float endSize;
	float speed;
	D3D11_BUFFER_DESC vbDesc;

	int firstDeadIndex;
	int firstAliveIndex;
	int livingParticleCount;

	Particle* particles;
	ParticleVertex* localVertices;



	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* texture;

	void updateSingleParticle(float delt, int i);

};

