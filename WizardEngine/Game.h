#pragma once

#include "DXCore.h"
#include "WICTextureLoader.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "Player.h"
#include "Terrain.h"


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
	void DrawBox(XMFLOAT3 position, XMFLOAT3 scale);

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

	static void testHandleCollision(Collider* me, Collider* that);
private:
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void CreateBasicGeometry();
	void CreateMaterials();
	void CreateModels();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;
	SimpleVertexShader* ParticleVS;
	SimplePixelShader* ParticlePS;
	SimpleVertexShader* normalVS;
	SimplePixelShader* normalPS;

	BasicGeometry basicGeometry;

	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	std::vector<Entity*> Entities;

	Mesh* melonMesh;
	Mesh* floorMesh;
	Mesh* columnMesh;
	Mesh* wallMesh;

	ID3D11ShaderResourceView* melonTexture;
	ID3D11ShaderResourceView* marbleTexture;
	ID3D11ShaderResourceView* sandDiffuse;
	ID3D11ShaderResourceView* sandNormal;
	ID3D11ShaderResourceView* stoneWall;
	ID3D11ShaderResourceView* stoneWallNormal;
	ID3D11ShaderResourceView* skySRV;

	ID3D11RasterizerState* skyRast;
	ID3D11DepthStencilState* skyDepth;

	ID3D11RasterizerState* debugRast;

	ID3D11SamplerState* sampler;

	Material* melonMaterial;
	Material* marbleMaterial;
	Material* sandMaterial;
	Material* stoneMaterial;

	Camera* Cam;
	Player* player;

	DirectionalLight DirLight;
	DirectionalLight TopLight;

	Terrain* terrain;

	//for particles
	// Particle stuff
	ID3D11ShaderResourceView* particleTexture;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;



};