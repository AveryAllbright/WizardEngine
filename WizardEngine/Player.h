#pragma once

#include "Camera.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Emitter.h"
#include "DXCore.h"
#include "WICTextureLoader.h"
class Game;

class Player
{
private:
	Game* game;
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

	ID3D11Device* device;
public:
	Player(Camera* a_Camera, ID3D11Device* device, ID3D11DeviceContext* context, Game* ref);
	~Player();
	void Update(float delt);

	void CastSpellOne();
	void CastSpellTwo();

	void SetActiveSpell(float input);
	float playerHeight;
	bool m_bGrounded;
};

