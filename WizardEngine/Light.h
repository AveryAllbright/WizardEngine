#pragma once

#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColour;
	DirectX::XMFLOAT4 DiffuseColour;
	DirectX::XMFLOAT3 Direction;
};