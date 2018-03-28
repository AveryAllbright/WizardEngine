#pragma once
#include "SpatialSound.h"

class CenteredSound : SpatialSound
{
public:
	~CenteredSound();
	HRESULT OnUpdate(float directionx, float directiony, float directionz);
};