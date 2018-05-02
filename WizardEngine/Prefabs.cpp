#include "Prefabs.h"
#include "ColliderBox.h"

Column::Column(Mesh * mesh, Material * mainMaterial, Material* damageMaterial, Game* game)
	: Entity(mesh, mainMaterial){

	this->m_vScale = XMFLOAT3(0.01f, 0.01f, 0.01f);
	this->hitMaterial = damageMaterial;

	//create a box collider component
	ColliderBox* collider = new ColliderBox(this->m_vPos);

	//TODO: temporary function that is called on collision
	collider->onCollisionEnterFunction = &Column::testHandleCollision;
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
