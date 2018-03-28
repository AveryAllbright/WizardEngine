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
	
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//melon.tif", 0, &spellOneTexture);

	D3D11_SAMPLER_DESC sd = {};
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 16;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sd, &Sampler);

	matSpellOne = new Material(vertexShader, pixelShader, spellOneTexture, Sampler);

	meshSpellOne = new Mesh("..//..//Assets//Models//melon.obj", device);

	entityOneSpeed = 1;

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
	for (int j = 0; j < EntitiesOne.size(); j++) 
	{
		XMVECTOR vecOne = XMLoadFloat3(&EntitiesOne[j].GetPos());
		XMVECTOR vecTwo = XMLoadFloat3(&EntitiesOne[j].GetVelocity());
		vecTwo = DirectX::XMVectorScale(vecTwo, delt * entityOneSpeed);
		XMVECTOR vecFinal = XMVectorAdd(vecOne, vecTwo);
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, vecFinal);

		EntitiesOne[j].SetPos(temp);
		EntitiesOne[j].UpdateWorldView();
	}
	

	m_bPreviouslyGrounded = m_bGrounded;



}

void Player::SpellOne()
{
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixIdentity()));
	EntitiesOne.push_back(Entity(meshSpellOne, matSpellOne, world, m_vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)));
	EntitiesOne[EntitiesOne.size() - 1].SetVelocity(m_Camera->GetForward());
	EntitiesOne[EntitiesOne.size() - 1].UpdateWorldView();
}

void Player::SpellTwo()
{
}

void Player::SpellThree()
{
}
