#pragma once
#include "DXCore.h"
#include <DirectXMath.h>

class Camera {
public:
	// Contruct/Destruct
	Camera(int width, int height);
	Camera(int width, int height, DirectX::XMFLOAT3 position);
	~Camera();
	void init(int width, int height, DirectX::XMFLOAT3 position);

	// Getters/Setters
	DirectX::XMFLOAT3   GetPosition();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	Camera* SetPosition(DirectX::XMFLOAT3 position);
	Camera* SetMoveSpeed(float moveSpeed);

	Camera* Update(float deltaTime, float totalTime);
	Camera* OnMouseMove(int prex, int prey, int x, int y);

	float cameraMovementSpeed;
	float cameraRotationSpeed;
	float rotationXRadians;
	float rotationYRadians;

	void CreateProjection(int width, int height);
private:
	void CreateView(DirectX::XMVECTOR pos, DirectX::XMVECTOR dir);

	// Internal Data
	DirectX::XMFLOAT3 position;
	bool outdatedMatrix;
	const DirectX::XMFLOAT3 FORWARD = DirectX::XMFLOAT3(0, 0, 1);

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
};