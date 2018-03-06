#include "Entity.h"

Entity::Entity(Mesh * a_pMesh, Material* a_pMaterial, DirectX::XMFLOAT4X4 a_mWorld, DirectX::XMFLOAT3 a_vPos, DirectX::XMFLOAT3 a_vRotation, DirectX::XMFLOAT3 a_vScale) : m_pMesh(a_pMesh), m_mWorld(a_mWorld),
m_vPos(a_vPos), m_vRotation(a_vRotation), m_vScale(a_vScale), m_pMaterial(a_pMaterial)
{
	//Nothing TODO here
}

void Entity::SetRotation(DirectX::XMFLOAT3 a_vRotation) 
{
	m_vRotation = a_vRotation;
}

void Entity::SetScale(DirectX::XMFLOAT3 a_vScale)
{
	m_vScale = a_vScale;
}

void Entity::SetPos(DirectX::XMFLOAT3 a_vPos)
{
	m_vPos = a_vPos;
}

void Entity::SetWorld(DirectX::XMFLOAT4X4 a_mWorld)
{
	m_mWorld = a_mWorld;
}

void Entity::PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj)
{
	m_pMaterial->GetVertShader()->SetMatrix4x4("world", m_mWorld);
	m_pMaterial->GetVertShader()->SetMatrix4x4("view", a_view);
	m_pMaterial->GetVertShader()->SetMatrix4x4("projection", a_proj);

	m_pMaterial->GetVertShader()->CopyAllBufferData();
	m_pMaterial->GetVertShader()->SetShader();
	m_pMaterial->GetPixelShader()->SetShader();
}

DirectX::XMFLOAT3 Entity::GetRotation()
{
	return m_vRotation;
}

DirectX::XMFLOAT3 Entity::GetScale()
{
	return m_vScale;
}

DirectX::XMFLOAT3 Entity::GetPos()
{
	return m_vPos;
}

DirectX::XMFLOAT4X4 Entity::GetWorld()
{
	return m_mWorld;
}

Mesh * Entity::GetMesh()
{
	return m_pMesh;
}

Material * Entity::GetMaterial()
{
	return m_pMaterial;
}

void Entity::Move(DirectX::XMFLOAT3 a_vDisplaceBy)
{
	m_vPos.x += a_vDisplaceBy.x;
	m_vPos.y += a_vDisplaceBy.y;
	m_vPos.z += a_vDisplaceBy.z;
}

void Entity::MoveForward(float a_fDisplaceBy)
{
	//TODO : implement Look Move
}

void Entity::UpdateWorldView()
{
	DirectX::XMVECTOR trans = DirectX::XMLoadFloat3(&m_vPos);
	DirectX::XMVECTOR rotate = DirectX::XMLoadFloat3(&m_vRotation);
	DirectX::XMVECTOR scale = DirectX::XMLoadFloat3(&m_vScale);

	DirectX::XMMATRIX transl = DirectX::XMMatrixTranslationFromVector(trans);
	DirectX::XMMATRIX rotat = DirectX::XMMatrixRotationRollPitchYawFromVector(rotate);
	DirectX::XMMATRIX scal = DirectX::XMMatrixScalingFromVector(scale);

	DirectX::XMMATRIX world = scal * rotat * transl;

	DirectX::XMStoreFloat4x4(&m_mWorld, DirectX::XMMatrixTranspose(world));
}

void Entity::RotateBy(DirectX::XMFLOAT3 a_vRotation)
{
	m_vRotation.x += a_vRotation.x;
	m_vRotation.y += a_vRotation.y;
	m_vRotation.z += a_vRotation.z;
}

void Entity::ScaleBy(DirectX::XMFLOAT3 a_vScale)
{
	m_vScale.x += a_vScale.x;
	m_vScale.y += a_vScale.y;
	m_vScale.z += a_vScale.z;
}

Entity::~Entity()
{
}


