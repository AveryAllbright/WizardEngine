#pragma once
class Component;
class Collider;
#include "Object.h"
#include "Mesh.h"
#include "Material.h"
#include <DirectXMath.h>

class Entity : public Object {
public:
	Entity(Mesh* a_pMesh, Material* a_pMaterial, DirectX::XMFLOAT4X4 a_mWorld, DirectX::XMFLOAT3 a_vPos, DirectX::XMFLOAT3 a_vRotation, DirectX::XMFLOAT3 a_vScale);
	
	// Accessors 
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT3 GetVelocity();

	void SetPosition(DirectX::XMFLOAT3 a_vPos);
	void SetRotation(DirectX::XMFLOAT3 a_vRotation);
	void SetScale(DirectX::XMFLOAT3 a_vScale);
	void SetWorld(DirectX::XMFLOAT4X4 a_mWorld);
	void SetVelocity(DirectX::XMFLOAT3 toCopy);

	void PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj);

	Mesh* GetMesh();
	Material* GetMaterial();

	void Move(DirectX::XMFLOAT3 a_vDisplaceBy);
	void MoveForward(float a_fDisplaceBy);

	void UpdateWorldView();
	void RotateBy(DirectX::XMFLOAT3 a_vRotation);
	void ScaleBy(DirectX::XMFLOAT3 a_vScale);

	virtual ~Entity();

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

	static void HandleCollision(Collider*, Collider*);

private:
	DirectX::XMFLOAT3 m_vRotation;
	DirectX::XMFLOAT3 m_vScale;
	DirectX::XMFLOAT3 m_vPos;
	DirectX::XMFLOAT4X4 m_mWorld;
	DirectX::XMFLOAT3 m_velocity;
	Mesh* m_pMesh;
	Material* m_pMaterial;

	bool m_bDirty = false;
};

