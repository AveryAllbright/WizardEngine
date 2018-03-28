#include "ColliderBox.h"

ColliderBox::ColliderBox(DirectX::XMFLOAT3 center)
{
	this->center = center;
	this->scale = DirectX::XMFLOAT3(1, 1, 1);
	calcMinsMaxs();
	ColliderCollection.push_back(this);
}

ColliderBox::ColliderBox(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 scale)
{
	this->center = center;
	this->scale = scale;
	calcMinsMaxs();
	ColliderCollection.push_back(this);
}

ColliderBox::~ColliderBox() {}

ColliderBox * ColliderBox::SetCenter(DirectX::XMFLOAT3 center)
{
	this->center = center;
	calcMinsMaxs();
	return this;
}

ColliderBox * ColliderBox::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	calcMinsMaxs();
	return this;
}



void ColliderBox::calcMinsMaxs() {
	DirectX::XMVECTOR centerVector = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR scaleVector = DirectX::XMLoadFloat3(&scale);
	DirectX::XMVectorScale(scaleVector, 0.5f);
	DirectX::XMStoreFloat3(&mins, DirectX::XMVectorSubtract(centerVector, scaleVector));
	DirectX::XMStoreFloat3(&maxs, DirectX::XMVectorAdd(centerVector, scaleVector));
}

void ColliderBox::subUpdate() {}

void ColliderBox::testCollision(Collider * other)
{
	ColliderBox* castedOther = dynamic_cast<ColliderBox*>(other);
	callCallbacks(IsColliding(castedOther), other);
}

bool ColliderBox::IsColliding(ColliderBox * other)
{
	// test x
	if (maxs.x < other->mins.x) return false;
	if (mins.x > other->maxs.x) return false;

	// test y
	if (maxs.y < other->mins.y) return false;
	if (mins.y > other->maxs.y) return false;

	//test z
	if (maxs.z < other->mins.z) return false;
	if (mins.z > other->maxs.z) return false;

	return true;
}
