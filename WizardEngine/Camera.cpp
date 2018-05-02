#include "Camera.h"
using namespace DirectX;

#pragma region Construct/Destruct
Camera::Camera(int width, int height)
{
	//Default values for camera positioning
	XMFLOAT3 position = XMFLOAT3(140, 0, 83);

	init(width, height, position);
}

Camera::Camera(int width, int height, XMFLOAT3 position)
{
	init(width, height, position);
}
void Camera::init(int width, int height, XMFLOAT3 position)
{
	rotationXRadians = 0;
	rotationYRadians = 0;
	this->position = position;

	CreateView(XMLoadFloat3(&position), XMLoadFloat3(&FORWARD));
	CreateProjection(width, height);
	cameraMovementSpeed = 3;
	cameraRotationSpeed = 0.002f;
}
Camera::~Camera() {}
#pragma endregion

#pragma region MatrixCreations
DirectX::XMFLOAT3 Camera::GetForward()
{
	return forward;
}
// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Camera::CreateProjection(int width, int height)
{
	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		10000.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::CreateView(XMVECTOR pos, XMVECTOR dir)
{
	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space

	XMMATRIX V = XMMatrixLookToLH(
		pos,											// The position of the "camera"
		dir,											// Direction the camera is looking
		XMVectorSet(0, 1, 0, 0));				// "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!
}
#pragma endregion

#pragma region Getters/Setters
XMFLOAT3 Camera::GetPosition()
{
	return position;
}
XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
Camera * Camera::SetPosition(XMFLOAT3 position)
{
	outdatedMatrix = true;
	this->position = position;
	return this;
}
Camera * Camera::SetMoveSpeed(float moveSpeed)
{
	this->cameraMovementSpeed = moveSpeed;
	return this;
}
#pragma endregion

Camera * Camera::Update(float deltaTime, float totalTime)
{
	XMVECTOR rot = XMQuaternionRotationRollPitchYaw(rotationYRadians, rotationXRadians, 0);

	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);

	XMVECTOR cameraDirectionVector = XMVector3Rotate(dir, rot);
	DirectX::XMStoreFloat3(&forward, cameraDirectionVector);
	cameraDirectionVector = XMVector4Normalize(cameraDirectionVector);
	XMVECTOR cameraDirectionLeftVector = XMVector3Cross(cameraDirectionVector, XMVectorSet(0, 1, 0, 0));

	float scale = deltaTime * cameraMovementSpeed;
	cameraDirectionVector = XMVectorScale(cameraDirectionVector, scale);
	cameraDirectionLeftVector = XMVectorScale(cameraDirectionLeftVector, scale);

	XMVECTOR cameraPositionVector = XMLoadFloat3(&position);

	//Movement forward and back along direction vector
	if (GetAsyncKeyState('W') & 0x8000) {
		XMVECTOR temp = cameraDirectionVector;
		temp = XMVectorSetY(temp, 0);
		XMStoreFloat3(&position, XMVectorAdd(cameraPositionVector, temp));
		outdatedMatrix = true;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		XMVECTOR temp = cameraDirectionVector;
		temp = XMVectorSetY(temp, 0);
		XMStoreFloat3(&position, XMVectorSubtract(cameraPositionVector, temp));
		outdatedMatrix = true;
	}

	//Movement left and right based on direction vector
	if (GetAsyncKeyState('A') & 0x8000) {
		XMStoreFloat3(&position, XMVectorAdd(cameraPositionVector, cameraDirectionLeftVector));
		outdatedMatrix = true;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		XMStoreFloat3(&position, XMVectorSubtract(cameraPositionVector, cameraDirectionLeftVector));
		outdatedMatrix = true;
	}
	//Movement up and down in world coords
	if (GetAsyncKeyState('Q') & 0x8000) {
		position.y -= deltaTime * cameraMovementSpeed;
		outdatedMatrix = true;
	}
	if (GetAsyncKeyState('E') & 0x8000) {
		position.y += deltaTime * cameraMovementSpeed;
		outdatedMatrix = true;
	}

	if (outdatedMatrix) {
		CreateView(XMLoadFloat3(&this->position), cameraDirectionVector);
	}
	return this;
}

Camera* Camera::OnMouseMove(int previousX, int previousY, int x, int y) {
	// The pixel position change in the mouse since the last call
	int deltaX = x - previousX;
	int deltaY = y - previousY;

	rotationXRadians += deltaX * cameraRotationSpeed;
	rotationYRadians += deltaY * cameraRotationSpeed;

	// Clamp the pitch to prevent flipping upside-down
	const float BOUNDS = XMConvertToRadians(170);
	rotationYRadians = min(max(rotationYRadians, -BOUNDS), BOUNDS);

	return this;
}