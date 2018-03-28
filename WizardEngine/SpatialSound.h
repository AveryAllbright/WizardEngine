#pragma once

#include <xaudio2.h>
#include <xapo.h>
#include <hrtfapoapi.h>

class SpatialSound
{
public:
	~SpatialSound();
	HRESULT Initialize(LPCWSTR filename);

	HRESULT Start();
	HRESULT Stop();
	HRESULT OnUpdate(float directionx, float directiony, float directionz);
};