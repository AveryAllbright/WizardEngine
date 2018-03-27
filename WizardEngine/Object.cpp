#pragma once
#include "Object.h"

// instantiate static counter
int Object::nextInstanceId = 0;

Object::Object() {
	// make sure to update nextInstanceId after use
	//NOTE: will fail if we reach MAX_INT objects over lifetime of game
	instanceId = nextInstanceId++;
}

Object::~Object()
{
}

int Object::GetInstanceId() const
{
	return instanceId;
}

bool Object::operator!=(Object & other)
{
	return instanceId != other.GetInstanceId();
}

bool Object::operator==(Object & other)
{
	return instanceId == other.GetInstanceId();
}
