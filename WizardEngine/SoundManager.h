#pragma once
#include <string>
#include "SpatialSound.h"

// A singleton to contain all of the sound buffers, music and sound variables.
class SoundManager
{
	static SoundManager* m_pInstance;
public:
	static SoundManager* GetInstance();

	static void ReleaseInstance(void);

	SpatialSound* LoadSound(const wchar_t* filename, std::string soundName);

	SoundManager* StartSound(std::string soundName);
	SoundManager* StartSound(SpatialSound* sound);

	SoundManager* StopSound(std::string soundName);
	SoundManager* StopSound(SpatialSound* sound);


	// updates the sounds position relative to the player for spatial falloff
	SoundManager* UpdateSound(std::string soundName, float dx, float dy, float dz);
	SoundManager* UpdateSound(SpatialSound* sound, float dx, float dy, float dz);

private:
	SoundManager(void);

	~SoundManager(void);

	void Init(void);
};