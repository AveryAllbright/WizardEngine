#pragma once
class Entity;
#include "Object.h"

class Component : public Object
{
public:
	Component();
	~Component();

	Entity* getEntity();
	void setEntity(Entity* gameObject);
	virtual void Start();
	virtual void Update(float deltaTime);

private:
	Entity* entity;
};