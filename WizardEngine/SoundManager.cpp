#include "SoundManager.h"
#pragma once

SoundManager * SoundManager::GetInstance()
{
	return nullptr;
}

void SoundManager::ReleaseInstance(void)
{
}

SpatialSound * SoundManager::LoadSound(const wchar_t * filename, std::string soundName)
{
	return nullptr;
}

SoundManager * SoundManager::StartSound(std::string soundName)
{
	return nullptr;
}

SoundManager * SoundManager::StartSound(SpatialSound * sound)
{
	return nullptr;
}

SoundManager * SoundManager::StopSound(std::string soundName)
{
	return nullptr;
}

SoundManager * SoundManager::StopSound(SpatialSound * sound)
{
	return nullptr;
}

SoundManager * SoundManager::UpdateSound(std::string soundName, float dx, float dy, float dz)
{
	return nullptr;
}

SoundManager * SoundManager::UpdateSound(SpatialSound * sound, float dx, float dy, float dz)
{
	return nullptr;
}

SoundManager::~SoundManager(void)
{
}

void SoundManager::Init(void)
{
}
