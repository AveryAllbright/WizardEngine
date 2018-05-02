#pragma once
#include "Entity.h"
#include "Game.h"

class Column : public Entity {
public:
	Column(Mesh*, Material*, Material*, Game*);
	Material* hitMaterial;
	static void testHandleCollision(Collider* me, Collider* other);
	virtual Entity* SetPosition(DirectX::XMFLOAT3 a_vPos);
};
