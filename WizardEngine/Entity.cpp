#include "Entity.h"
#include "Component.h"

Entity::Entity(Mesh * a_pMesh, Material* a_pMaterial, DirectX::XMFLOAT4X4 a_mWorld, DirectX::XMFLOAT3 a_vPos, DirectX::XMFLOAT3 a_vRotation, DirectX::XMFLOAT3 a_vScale) : m_pMesh(a_pMesh), m_mWorld(a_mWorld),
m_vPos(a_vPos), m_vRotation(a_vRotation), m_vScale(a_vScale), m_pMaterial(a_pMaterial)
{
	components = std::vector<Component*>();
}

Entity* Entity::AddComponent(Component* component) {
	components.push_back(component);
	component->setEntity(this);
	return this;
}

Entity* Entity::Start() {
	for (int i = 0; i < (int)components.size(); i++)
		components[i]->Start();

	return this;
}

bool Entity::Update(float deltaTime) {
	//early return if not active
	//returning bools lets child classes also early return
	//if (!active) return false;

	//update all components as well
	for (int i = 0; i < (int)components.size(); i++)
		components[i]->Update(deltaTime);
	return true;
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

DirectX::XMFLOAT3 Entity::GetVelocity()
{
	return m_velocity;
}
void Entity::SetVelocity(DirectX::XMFLOAT3 toCopy)
{
	m_velocity = toCopy;
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

/*
void Entity::HandleCollision(Collider* mainobj, Collider* other)
{
	
	DestructObj* me = static_cast<DestructObj*>(mainobj->getGameObject());

	Projectile* castedOther = dynamic_cast<Projectile*>(other->getGameObject());
	if (!castedOther) return;

	me->health -= 10;

	if (me->health <= 0) {
		std::cout << "dead" << std::endl;
		me->SetActive(false);
	}
	
}
*/

