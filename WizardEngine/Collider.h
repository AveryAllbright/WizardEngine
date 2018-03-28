#pragma once

#include "Component.h"
class Entity;
#include <vector>

class Collider : public Component {
public:
	Collider();
	~Collider();

	float contactOffset;
	bool enabled;
	bool isTrigger;

	void(*onCollisionEnterFunction)(Collider*, Collider*);
	void(*onCollisionExitFunction)(Collider*, Collider*);
	void(*onCollisionStayFunction)(Collider*, Collider*);
	void(*onTriggerEnterFunction)(Collider*, Collider*);
	void(*onTriggerExitFunction)(Collider*, Collider*);
	void(*onTriggerStayFunction)(Collider*, Collider*);

	virtual void Update(float deltaTime);
	virtual void Render();

protected:
	bool previouslyCollided;
	bool previouslyTriggered;

	void onCollisionEnter(Collider* other);
	void onCollisionExit(Collider* other);
	void onCollisionStay(Collider* other);
	void onTriggerEnter(Collider* other);
	void onTriggerExit(Collider* other);
	void onTriggerStay(Collider* other);

	static std::vector<Collider*> ColliderCollection;
	virtual void subUpdate();
	virtual void callCallbacks(bool contact, Collider* other);
	virtual void testCollision(Collider* other);
};