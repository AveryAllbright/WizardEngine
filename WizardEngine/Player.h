#pragma once

#include "Camera.h"
#include <DirectXMath.h>

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


public:
	Player(Camera* a_Camera);
	~Player();
	void Update(float delt);
};

