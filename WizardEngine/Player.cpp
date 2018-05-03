#include "Player.h"
#include "Prefabs.h"
#include "Game.h"
#include <iostream>

using namespace DirectX;

Player::Player(Camera* a_Camera, ID3D11Device* device, ID3D11DeviceContext* context, Game* ref)
{
	m_Camera = a_Camera; 

	m_vPos = m_Camera->GetPosition();
	m_Jumping = false;
	m_bGrounded = true;
	m_bJump = false;

	m_fWalkSpeed = 18.f;
	m_fRunSpeed = 6.0f;

	m_fJumpSpeed = .012f;

	m_fStickToGroundForce = 9.81f;
	m_fGravityMult = 1.f;

	m_Casting = false;
	cooldown = 0;
	m_nActiveSpell = 0;


	playerHeight = 2.5f;
	this->device = device;
	game = ref;
}


Player::~Player(){}

void Player::Update(float delt)
{
	m_vPos = m_Camera->GetPosition();

	if (!m_Casting && cooldown <= 0)
	{
		m_Casting = (GetKeyState(VK_LBUTTON) & 0x100);
	}
	
	if (m_Casting) {
		switch(m_nActiveSpell)
		{
		case 0: CastSpellOne();
			break;
		case 1: CastSpellTwo();
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

	//if (m_vPos.y <= 0.0f) { m_vPos.y = 0.125f; m_bGrounded = true;}

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
	/*
	for (int j = 0; j < Entities.size(); j++) 
	{
		Entities[j]->UpdateEmitters(delt, entityOneSpeed, wallRiseSpeed);
	}
	*/
	
	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_fGravityMult = 0;
	}


	m_bPreviouslyGrounded = m_bGrounded;
}

void Player::CastSpellOne()
{
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixIdentity()));
	SpellOne* castedSpell = new SpellOne(game->basicGeometry.sphere, game->matSpellOne, device, game->spellOneTexture);
	castedSpell->velocity = m_Camera->GetForward();

	//TODO need to override this to set particle pos
	castedSpell->SetPosition(m_vPos)->SetScale(XMFLOAT3(.01f, .01f, .01f));
	castedSpell->ParticlePS = game->particlePS;
	castedSpell->ParticleVS = game->particleVS;

	Game::Entities.push_back(castedSpell);
}

void Player::CastSpellTwo()
{
	XMVECTOR pos = XMLoadFloat3(&m_vPos);
	XMVECTOR offset = XMVectorSet(0, -3.5, 0, 0);
	XMVECTOR displace = XMLoadFloat3(&m_Camera->GetForward());
	displace = XMVectorScale(displace, 3);
	displace = XMVectorSetIntY(displace, 0);
	pos = XMVectorAdd(pos, offset);
	pos = XMVectorAdd(pos, displace);
	XMFLOAT3 offsetby;
	XMStoreFloat3(&offsetby, pos);

	XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixIdentity()));
	//Game::Entities.push_back(new SpellTwo());
	//Entities.push_back((new Emitter(meshSpellTwo, matSpellTwo, 1, XMFLOAT3(0, wallRiseSpeed, 0), offsetby, XMFLOAT3(2, 5, .55f), particleVS, particlePS, device, spellTwoParticle)));
}

void Player::SetActiveSpell(float input)
{
	if (input < 0)
	{
		m_nActiveSpell--;
		if (m_nActiveSpell < 0) { m_nActiveSpell = m_nMaxSpell; }
	}
	else
	{
		m_nActiveSpell++;
		if (m_nActiveSpell > m_nMaxSpell) { m_nActiveSpell = 0; }
	}
}
