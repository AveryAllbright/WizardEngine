#pragma once
#include "Component.h"
#include "Entity.h"

Component::Component()
{
	entity = nullptr;
}

Component::~Component() {}

Entity* Component::getEntity()
{
	return entity;
}

void Component::setEntity(Entity* entity)
{
	this->entity = entity;
}
void Component::Start() {}
void Component::Update(float deltaTime) {}
void Component::Render() {}