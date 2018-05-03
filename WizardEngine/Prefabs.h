#pragma once
#include "Entity.h"
#include "Emitter.h"
#include "Game.h"

class Column : public Entity {
public:
	Column(Mesh*, Material* mainMaterial, Material* damageMaterial, Game* ref);
	Material* hitMaterial;
	static void testHandleCollision(Collider* me, Collider* other);

	// need to override to update collider's pos as well
	virtual Entity* SetPosition(DirectX::XMFLOAT3 a_vPos);
};

class SpellOne : public Emitter {
public:
	SpellOne(Mesh*, Material*, ID3D11Device* device, ID3D11ShaderResourceView* texture, Game* game);
	virtual bool Update(float delta);

};

class SpellTwo : public Emitter {
public:
	SpellTwo(Mesh*, Material*, ID3D11Device* device, ID3D11ShaderResourceView* texture);
	virtual bool Update(float delta);
	virtual void SpawnParticle();
};