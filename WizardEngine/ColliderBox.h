#pragma once
#include "Collider.h"

class ColliderBox : public Collider {
private:
	DirectX::XMFLOAT3 center;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 mins;
	DirectX::XMFLOAT3 maxs;
	void calcMinsMaxs();
public:
	virtual void subUpdate();
	virtual void testCollision(Collider* other);

	bool colliding = false;

	ColliderBox(DirectX::XMFLOAT3 center);
	ColliderBox(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 scale);
	~ColliderBox();

	ColliderBox* SetCenter(DirectX::XMFLOAT3 center);
	ColliderBox* SetScale(DirectX::XMFLOAT3 scale);

	//TODO getters

	bool IsColliding(ColliderBox* a_other);
};