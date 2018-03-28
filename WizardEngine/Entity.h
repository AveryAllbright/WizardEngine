#pragma once
class Component;
class Collider;
#include "Object.h"
#include "Mesh.h"
#include "Material.h"


#include <DirectXMath.h>

using vec3 = DirectX::XMFLOAT3;
using mat4 = DirectX::XMFLOAT4X4;


class Entity : public Object
{
private:
	
	DirectX::XMFLOAT3 m_vRotation;
	DirectX::XMFLOAT3 m_vScale;
	DirectX::XMFLOAT3 m_vPos;
	DirectX::XMFLOAT4X4 m_mWorld;
	DirectX::XMFLOAT3 m_velocity;
	Mesh* m_pMesh;
	Material* m_pMaterial;

	bool m_bDirty = false;


public:
	Entity(Mesh* a_pMesh, Material* a_pMaterial, DirectX::XMFLOAT4X4 a_mWorld, DirectX::XMFLOAT3 a_vPos, DirectX::XMFLOAT3 a_vRotation, DirectX::XMFLOAT3 a_vScale);
	
	/*
	 *		Accessors
	 */
	void SetRotation(DirectX::XMFLOAT3 a_vRotation);
	void SetScale(DirectX::XMFLOAT3 a_vScale);
	void SetPos(DirectX::XMFLOAT3 a_vPos);
	void SetWorld(DirectX::XMFLOAT4X4 a_mWorld);
	void PrepareMaterial(DirectX::XMFLOAT4X4 a_view, DirectX::XMFLOAT4X4 a_proj);


	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetPos();
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT3 GetVelocity();
	void SetVelocity(DirectX::XMFLOAT3 toCopy);
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
};

