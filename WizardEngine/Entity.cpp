#include "Entity.h"
#include "Component.h"

using namespace DirectX;

Entity::Entity(Mesh * mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;
	components = std::vector<Component*>();
	m_vPos = XMFLOAT3(0, 0, 0);
	m_vScale = XMFLOAT3(1, 1, 1);
	m_vRotation = XMFLOAT3(0, 0, 0);
	UpdateWorldView();
}

Entity::~Entity() {
	for (int i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
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
#pragma region Accessors
XMFLOAT3 Entity::GetPosition()
{
	return m_vPos;
}

XMFLOAT3 Entity::GetRotation()
{
	return m_vRotation;
}

XMFLOAT3 Entity::GetScale()
{
	return m_vScale;
}

XMFLOAT4X4 Entity::GetWorldMatrix()
{
	if (outdatedMatrix) {
		UpdateWorldView();
		outdatedMatrix = false;
	}
	return m_mWorld;
}

Entity* Entity::SetPosition(XMFLOAT3 a_vPos)
{
	m_vPos = a_vPos;
	outdatedMatrix = true;
	return this;
}

Entity* Entity::SetRotation(XMFLOAT3 a_vRotation)
{
	m_vRotation = a_vRotation;
	outdatedMatrix = true;
	return this;
}

Entity* Entity::SetScale(XMFLOAT3 a_vScale)
{
	m_vScale = a_vScale;
	outdatedMatrix = true;
	return this;
}
#pragma endregion

void Entity::PrepareMaterial(XMFLOAT4X4 a_view, XMFLOAT4X4 a_proj)
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

void Entity::Move(XMFLOAT3 a_vDisplaceBy)
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
	XMMATRIX tr = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
	XMMATRIX ro = XMMatrixRotationRollPitchYaw(m_vRotation.x, m_vRotation.y, m_vRotation.z);
	XMMATRIX sc = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);

	XMStoreFloat4x4(&m_mWorld, XMMatrixTranspose(sc * ro * tr));
}

void Entity::RotateBy(XMFLOAT3 a_vRotation)
{
	m_vRotation.x += a_vRotation.x;
	m_vRotation.y += a_vRotation.y;
	m_vRotation.z += a_vRotation.z;
}

void Entity::ScaleBy(XMFLOAT3 a_vScale)
{
	m_vScale.x += a_vScale.x;
	m_vScale.y += a_vScale.y;
	m_vScale.z += a_vScale.z;
}