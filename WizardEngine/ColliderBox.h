#pragma once
#include "Collider.h"
#include "Game.h"

class ColliderBox : public Collider {
private:
	DirectX::XMFLOAT3 center;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 mins;
	DirectX::XMFLOAT3 maxs;
	void calcMinsMaxs();
	Game* game;
public:
	//TODO this is straight hacks (needed to call drawBox in render)
	void setGameRef(Game* ref);

	virtual void subUpdate();
	virtual void testCollision(Collider* other);
	virtual void Render();

	bool colliding = false;

	ColliderBox(DirectX::XMFLOAT3 center);
	ColliderBox(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 scale);
	~ColliderBox();

	ColliderBox* SetCenter(DirectX::XMFLOAT3 center);
	ColliderBox* SetScale(DirectX::XMFLOAT3 scale);

	//TODO autoset params based on vertex list
	//TODO getters

	bool IsColliding(ColliderBox* a_other);
};