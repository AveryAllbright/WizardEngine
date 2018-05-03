#pragma once

#include "DXCore.h"
#include "WICTextureLoader.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
class Player;
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
	void RenderShadowMap();
	void DrawBox(XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT4 color = XMFLOAT4(0, 1, 0, 1));

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);
	static std::vector<Entity*> Entities;
	static std::vector<Entity*> EntitiesTransparent;


	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderDebug;
	SimplePixelShader* pixelShaderDebug;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;
	SimpleVertexShader* normalVS;
	SimplePixelShader* normalPS;
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;

	BasicGeometry basicGeometry;

	Material* melonMaterial;
	Material* marbleMaterial;
	Material* marbleHitMaterial;
	Material* sandMaterial;
	Material* stoneMaterial;
	Material* dirtMaterial;
	Material* matSpellOne;
	Material* matSpellTwo;


	ID3D11ShaderResourceView* spellOneTexture;
	ID3D11ShaderResourceView* spellTwoTexture;
	ID3D11ShaderResourceView* spellTwoParticle;
private:
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void CreateBasicGeometry();
	void CreateMaterials();
	void CreateModels();

	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	Mesh* melonMesh;
	Mesh* floorMesh;
	Mesh* columnMesh;
	Mesh* wallMesh;


	ID3D11ShaderResourceView* melonTexture;
	ID3D11ShaderResourceView* marbleTexture;
	ID3D11ShaderResourceView* marbleHitTexture;
	ID3D11ShaderResourceView* sandDiffuse;
	ID3D11ShaderResourceView* sandNormal;
	ID3D11ShaderResourceView* stoneWall;
	ID3D11ShaderResourceView* stoneWallNormal;
	ID3D11ShaderResourceView* dirtTexture;
	ID3D11ShaderResourceView* dirtNormal;
	ID3D11ShaderResourceView* skySRV;

	ID3D11RasterizerState* skyRast;
	ID3D11DepthStencilState* skyDepth;

	ID3D11RasterizerState* debugRast;

	ID3D11SamplerState* sampler;

	// For shadow map
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSamplerState;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS;
	DirectX::XMFLOAT4X4 dirLightProjection;
	DirectX::XMFLOAT4X4 dirLightView;

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