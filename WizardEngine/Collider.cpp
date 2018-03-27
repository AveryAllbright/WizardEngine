#pragma once
#include "Collider.h"
#include <iostream>

Collider::Collider() {}

Collider::~Collider() {}

std::vector<Collider*> Collider::ColliderCollection;

void Collider::onCollisionEnter(Collider* other) {
	if (onCollisionEnterFunction == nullptr) return;
	onCollisionEnterFunction(this, other);
}

void Collider::onCollisionExit(Collider* other) {
	if (onCollisionExitFunction == nullptr) return;
	onCollisionExitFunction(this, other);
}

void Collider::onCollisionStay(Collider* other) {
	if (onCollisionStayFunction == nullptr) return;
	onCollisionStayFunction(this, other);
}

void Collider::onTriggerEnter(Collider* other) {
	if (onTriggerEnterFunction == nullptr) return;
	onTriggerEnterFunction(this, other);
}

void Collider::onTriggerExit(Collider* other) {
	if (onTriggerExitFunction == nullptr) return;
	onTriggerExitFunction(this, other);
}

void Collider::onTriggerStay(Collider* other) {
	if (onTriggerStayFunction == nullptr) return;
	onTriggerStayFunction(this, other);
}

void Collider::subUpdate() {}
void Collider::callCallbacks(bool contact, Collider* other) {
	if (contact) {
		if (!isTrigger) {
			if (previouslyCollided) {
				onCollisionStay(other);
			}
			else {
				onCollisionEnter(other);
				previouslyCollided = true;
			}
		}
		else {
			if (previouslyTriggered) {
				onTriggerStay(other);
			}
			else {
				onTriggerEnter(other);
				previouslyTriggered;
			}
		}
	}
	else {
		if (isTrigger) {
			if (previouslyTriggered) {
				onTriggerExit(other);
				previouslyTriggered = false;
			}
		}
		else {
			if (previouslyCollided) {
				onCollisionExit(other);
				previouslyCollided = false;
			}
		}
	}
}
void Collider::testCollision(Collider* other) {}
void Collider::Update(float deltaTime) {
	subUpdate();
	for (auto singleCollid : ColliderCollection) {
		if (singleCollid == this)
			continue;
		//Don't collide with non-active colliders
		//if (!singleCollid->getGameObject()->active) continue;
		testCollision(singleCollid);
	}

}

void Collider::Render() {}
