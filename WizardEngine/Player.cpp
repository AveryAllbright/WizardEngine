#include "Player.h"
#include <iostream>

using namespace DirectX;

Player::Player(Camera* a_Camera, ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader)
{
	m_Camera = a_Camera; 

	m_vPos = m_Camera->GetPosition();
	m_Jumping = false;
	m_bGrounded = true;
	m_bJump = false;

	m_fWalkSpeed = 3.f;
	m_fRunSpeed = 6.0f;

	m_fJumpSpeed = .002f;

	m_fStickToGroundForce = 9.81f;
	m_fGravityMult = 1.f;

	m_Casting = false;
	cooldown = 0;
	spellReady = spellOne;
	
	meshSpellOne = new Mesh("Models//melon.obj", device);

}


Player::~Player()
{
	
}

void Player::Update(float delt)
{
	m_vPos = m_Camera->GetPosition();

	if (!m_Casting && cooldown <= 0)
	{
		m_Casting = (GetKeyState(VK_LBUTTON) & 0x100);
	}
	
	if (m_Casting) {
		switch(spellReady)
		{
		case spellOne: SpellOne();
			break;
		case spellTwo: SpellTwo();
			break;
		case spellThree: SpellThree();
			break;
			
		}
		m_Casting = false;
		cooldown = .5;
	}

	if (cooldown > 0) 
	{
		cooldown -= delt;
	}

	if (!m_bJump)
	{
		m_bJump = (GetAsyncKeyState(VK_SPACE) & 0x8000);
	}

	m_bIsWalking = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

	m_fMoveSpeed = m_bIsWalking ? m_fWalkSpeed : m_fRunSpeed;
	m_Camera->SetMoveSpeed(m_fMoveSpeed);

	if (m_vPos.y <= 0.0f) { m_vPos.y = 0.125f; m_bGrounded = true;}

  	if (!m_bPreviouslyGrounded && m_bGrounded)
	{
		m_vMoveDir.y = 0;
		m_Jumping = false;
		m_bJump = false;
	}

	if (!m_bGrounded && !m_Jumping && m_bPreviouslyGrounded)
	{
		m_vMoveDir.y = 0;
	}

	

	if (m_bGrounded)
	{
		//m_vMoveDir.y = -m_fStickToGroundForce;

		if (m_bJump)
		{
			m_vMoveDir.y = m_fJumpSpeed;
			m_bJump = false;
			m_Jumping = true;
			m_bGrounded = false;
		}
	}
	else
	{
		m_vMoveDir.y += -.00981f * m_fGravityMult * delt;
	}

	

	if (m_vMoveDir.y != 0)
	{
		XMVECTOR pos;
		XMVECTOR move;

		pos = XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 0);
		move = XMVectorSet(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z, 0);

		XMFLOAT3 temp; 
		pos = XMVectorAdd(pos, move);
		XMStoreFloat3(&temp, pos);

		m_Camera->SetPosition(temp);
		
	}

	

	m_bPreviouslyGrounded = m_bGrounded;



}

void Player::SpellOne()
{
	
}

void Player::SpellTwo()
{
}

void Player::SpellThree()
{
}
