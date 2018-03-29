#pragma once

#include "DXCore.h"
#include "WICTextureLoader.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "Player.h"

struct BasicGeometry {
	Mesh* cone;
	Mesh* cube;
	Mesh* cylinder;
	Mesh* helix;
	Mesh* sphere;
	Mesh* torus;
};


class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;

	SimpleSRV* srv;

	BasicGeometry basicGeometry;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
	
	std::vector<Entity> Entities;
	
	Mesh* Melon;
	Mesh* skyCube;
	Mesh* floor;
	
	ID3D11ShaderResourceView* melonTexture;
	ID3D11ShaderResourceView* skySRV;

	ID3D11RasterizerState* skyRast;
	ID3D11DepthStencilState* skyDepth;

	ID3D11SamplerState* sampler;
	
	Material* melonMat;
	Material* floorMat;	


	Camera* Cam;
	Player* player;

	DirectionalLight DirLight;
	DirectionalLight TopLight;

};

