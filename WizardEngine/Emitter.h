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
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	Mesh * mesh = nullptr;
	Material* material = nullptr;

	void PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj);

	Emitter(Mesh* a_pMesh, Material* a_pMaterial, int type, DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, SimpleVertexShader* particleVS, SimplePixelShader* particlePS, ID3D11Device* device, ID3D11ShaderResourceView* texture);
	~Emitter();

	Emitter* SetPosition(DirectX::XMFLOAT3 a_vPos);
	Emitter* SetRotation(DirectX::XMFLOAT3 a_vRotation);
	Emitter* SetScale(DirectX::XMFLOAT3 a_vScale);

	std::vector<Component*> components;

	void UpdateWorldView();

	bool UpdateEmitters(float delt, float speed, float speed2);

	void SpawnParticle();

	void CopyParticlesToGPU(ID3D11DeviceContext* context);
	void CopyOneParticle(int index);

	void Draw(ID3D11DeviceContext* context, Camera* camera);


private:
	DirectX::XMFLOAT3 m_vRotation;
	DirectX::XMFLOAT3 m_vScale;
	DirectX::XMFLOAT3 m_vPos;
	DirectX::XMFLOAT4X4 m_mWorld;


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
	D3D11_BUFFER_DESC vbDesc;

	int firstDeadIndex;
	int firstAliveIndex;
	int livingParticleCount;

	int type;

	Particle* particles;
	ParticleVertex* localVertices;

	SimpleVertexShader* ParticleVS;
	SimplePixelShader* ParticlePS;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* texture;

	void UpdateFireball(float delt, float speed);

	void UpdateWall(float delt, float speed);

	void updateSingleParticle(float delt, int i);

	bool outdatedMatrix = false;


};

