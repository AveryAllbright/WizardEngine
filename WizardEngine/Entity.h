#pragma once
class Component;
class Collider;
#include "Object.h"
#include "Mesh.h"
#include "Material.h"
#include "DXCore.h"
#include <DirectXMath.h>

class Entity : public Object {
public:
	Entity(Mesh* a_pMesh, Material* a_pMaterial);
	~Entity();

	// Accessors 
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	Entity* SetPosition(DirectX::XMFLOAT3 a_vPos);
	Entity* SetRotation(DirectX::XMFLOAT3 a_vRotation);
	Entity* SetScale(DirectX::XMFLOAT3 a_vScale);

	void PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj);

	void Move(DirectX::XMFLOAT3 a_vDisplaceBy);
	void MoveForward(float a_fDisplaceBy);

	void RotateBy(DirectX::XMFLOAT3 a_vRotation);
	void ScaleBy(DirectX::XMFLOAT3 a_vScale);

	std::vector<Component*> components;

	//provides accessor for getting components by type
	//NOTE: only gets first of type
	template<class T>
	T* getComponent() {
		for (unsigned int i = 0; i < components.size(); i++) {
			T* type = dynamic_cast<T*>(components[i]);
			if (type)
				return type;
		}
		return nullptr;
	}

	Entity* AddComponent(Component* component);
	virtual Entity* Start();
	virtual bool Update(float deltaTime);

	Mesh* mesh = nullptr;
	Material* material = nullptr;
	DirectX::XMFLOAT3 velocity;
private:
	DirectX::XMFLOAT3 m_vRotation;
	DirectX::XMFLOAT3 m_vScale;
	DirectX::XMFLOAT3 m_vPos;
	DirectX::XMFLOAT4X4 m_mWorld;
	void UpdateWorldView();
	bool outdatedMatrix = false;
};

