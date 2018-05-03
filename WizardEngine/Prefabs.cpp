#include "Prefabs.h"
#include "ColliderBox.h"

#pragma region Column
Column::Column(Mesh * mesh, Material * mainMaterial, Material* damageMaterial, Game* game)
	: Entity(mesh, mainMaterial){

	this->m_vScale = XMFLOAT3(0.01f, 0.01f, 0.01f);
	this->hitMaterial = damageMaterial;

	//create a box collider component
	ColliderBox* collider = new ColliderBox(this->m_vPos);

	//TODO: temporary function that is called on collision
	collider->onTriggerEnterFunction = &Column::testHandleCollision;
	collider->setGameRef(game);

	//set collider pos and scale
	collider->SetCenter(this->m_vPos)->SetScale(XMFLOAT3(4, 4, 4));

	this->AddComponent(collider);
}

void Column::testHandleCollision(Collider * me, Collider * other)
{
	printf("collider colliding with collider causing collision");

	//fiddle with casted to get the entity of this collider as a column
	Column* castedMe = (Column*)(static_cast<ColliderBox*>(me))->getEntity();
	
	//set the material as the damaged one
	castedMe->material = castedMe->hitMaterial;
}

Entity * Column::SetPosition(DirectX::XMFLOAT3 a_vPos)
{
	//update collider position
	this->getComponent<ColliderBox>()->SetCenter(a_vPos);

	//base class call
	Entity::SetPosition(a_vPos);
	return this;
}
#pragma endregion


SpellOne::SpellOne(Mesh* mesh, Material* material, ID3D11Device* device, ID3D11ShaderResourceView* texture, Game* game) 
	: Emitter(mesh, material, device, texture)
{
	//setup particles
	startColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
	endColor = XMFLOAT4(10.0f, 10.2f, 10.0f, 0);
	startVelocity = XMFLOAT3(-.01f, .01f, 0);
	emitterAcceleration = XMFLOAT3(0, 0, 0);
	particlesPerSecond = 100;
	secondsPerParticle = .001f;
	lifetime = .7f;
	startSize = .04f;
	endSize = .08f;
	speed = 3.5f;


	//create a box collider component
	ColliderBox* collider = new ColliderBox(this->m_vPos);

	collider->setGameRef(game);
	//collider->visible = false;


	//set collider pos and scale
	collider->SetCenter(this->m_vPos)->SetScale(XMFLOAT3(4, 4, 4));

	this->AddComponent(collider);
}

bool SpellOne::Update(float delta)
{
	XMVECTOR vecOne = XMLoadFloat3(&m_vPos);
	XMVECTOR vecTwo = XMLoadFloat3(&velocity);
	vecTwo = DirectX::XMVectorScale(vecTwo, delta * speed);
	XMVECTOR vecFinal = XMVectorAdd(vecOne, vecTwo);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, vecFinal);
	m_vPos = temp;
	particlePos = temp;
	outdatedMatrix = true;

	//base call
	Emitter::Update(delta);
	
	return false;
}
void SpellTwo::SpawnParticle() {
	Emitter::SpawnParticle();
	startVelocity.x = 0 - startVelocity.x;
	startVelocity.z = 0 - startVelocity.z;
}


SpellTwo::SpellTwo(Mesh* mesh, Material* material, ID3D11Device* device, ID3D11ShaderResourceView* texture) 
	: Emitter(mesh, material, device, texture)
{
	//setup particles
	startColor = XMFLOAT4(120, 42, 42, 0.2f);
	endColor = XMFLOAT4(120, 42, 42, 0);
	startVelocity = XMFLOAT3(-1, 1, -1);
	emitterAcceleration = XMFLOAT3(0, -1, 0);
	particlesPerSecond = 100;
	secondsPerParticle = .01f;
	lifetime = .8f;
	startSize = .8f;
	endSize = 2;
	speed = 1.f;
}

bool SpellTwo::Update(float delta)
{
	XMVECTOR vecOne = XMLoadFloat3(&m_vPos);
	XMVECTOR vecTwo = XMLoadFloat3(&velocity);
	vecTwo = DirectX::XMVectorScale(vecTwo, delta * speed);

	XMVECTOR vecFinal = XMVectorAdd(vecOne, vecTwo);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, vecFinal);
	m_vPos = temp;
	vecOne = XMLoadFloat3(&particlePos);
	vecTwo = XMLoadFloat3(&velocity);
	vecTwo = DirectX::XMVectorScale(vecTwo, delta * speed);

	vecFinal = XMVectorAdd(vecOne, -vecTwo);
	XMStoreFloat3(&temp, vecFinal);
	particlePos = temp;

	outdatedMatrix = true;

	if (GetPosition().y > wallFinal.y)
	{
		velocity = XMFLOAT3(0, 0, 0);
		startSize = 0;
		endSize = 0;
	}

	//base call
	Emitter::Update(delta);

	return false;
}