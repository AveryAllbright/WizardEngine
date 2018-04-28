#pragma once

#include "Camera.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Emitter.h"
#include "DXCore.h"
#include "WICTextureLoader.h"

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
	

	float m_fMoveSpeed;

	int m_nActiveSpell;
	int m_nMaxSpell = 1;

	float cooldown;

	bool m_Casting;

	Mesh* meshSpellOne;
	Mesh* meshSpellTwo;
	Mesh* meshSpellThree;

	Material* matSpellOne;
	Material* matSpellTwo;
	Material* matSpellThree;

	ID3D11ShaderResourceView* spellOneTexture;
	ID3D11ShaderResourceView* spellTwoTexture;

	ID3D11SamplerState* sampler;

	DirectX::XMFLOAT4X4 world;

	float entityOneSpeed;
	float wallRiseSpeed;

	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	ID3D11Device* device;


public:
	Player(Camera* a_Camera, ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, SimpleVertexShader* particleVS, SimplePixelShader* particlePS);
	~Player();
	void Update(float delt);

	void SpellOne();
	std::vector<Emitter*> Entities;
	

	void SpellTwo();
	

	void SpellThree();
	

	void SetActiveSpell(float input);
	float playerHeight;
	bool m_bGrounded;
};

