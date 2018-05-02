#pragma once
#include "Entity.h"
#include "Game.h"

class Column : public Entity {
public:
	Column(Mesh*, Material* mainMaterial, Material* damageMaterial, Game* ref);
	Material* hitMaterial;
	static void testHandleCollision(Collider* me, Collider* other);

	// need to override to update collider's pos as well
	virtual Entity* SetPosition(DirectX::XMFLOAT3 a_vPos);
};
