#pragma once

#include "Camera.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "DXCore.h"

class Player
{
private:
	bool m_bIsWalking;
	float m_fWalkSpeed;
	float m_fRunSpeed;
	float m_fRunStepLength;

	float m_fJumpSpeed;
	float m_fStickToGroundForce;
	float m_fGravityMult;

	Camera* m_Camera;
	bool m_bJump;
	float m_YRot;

	DirectX::XMFLOAT3 m_vMoveDir = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 m_vPos;
	bool m_bPreviouslyGrounded;

	bool m_Jumping;
	bool m_bGrounded;

	float m_fMoveSpeed;

	enum spellToCast
	{
		spellOne,
		spellTwo,
		spellThree
	};

	spellToCast spellReady;

	float cooldown;

	bool m_Casting;

	Mesh* meshSpellOne;
	Mesh* meshSpellTwo;
	Mesh* meshSpellThree;

	Material* matSpellOne;
	Material* matSpellTwo;
	Material* matSpellThree;

	ID3D11ShaderResourceView* spellOneTexture;

	ID3D11SamplerState* Sampler;


public:
	Player(Camera* a_Camera, ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader);
	~Player();
	void Update(float delt);

	void SpellOne();
	Entity* EntitiesOne;

	void SpellTwo();
	Entity* EntitiesTwo;

	void SpellThree();
	Entity* EntitiesThree;
};

