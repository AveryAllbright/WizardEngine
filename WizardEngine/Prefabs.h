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
//Entities.push_back((new Emitter(meshSpellOne, matSpellOne, 0, m_Camera->GetForward(), m_vPos, XMFLOAT3(.01f,.01f,.01f), particleVS, particlePS , device, spellOneTexture)));
public:
	SpellOne(Mesh*, Material*, ID3D11Device* device, ID3D11ShaderResourceView* texture);
	virtual bool UpdateEmitters(float delta);
};

class SpellTwo : public Emitter {
//Entities.push_back((new Emitter(meshSpellTwo, matSpellTwo, 1, XMFLOAT3(0, wallRiseSpeed, 0), offsetby, XMFLOAT3(2, 5, .55f), particleVS, particlePS, device, spellTwoParticle)));
public:
	SpellTwo(Mesh*, Material*, ID3D11Device* device, ID3D11ShaderResourceView* texture);
	virtual bool UpdateEmitters(float delta);
	virtual void SpawnParticle();
};