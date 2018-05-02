#include "Game.h"
#include "Prefabs.h"
#include "Vertex.h"
#include "ColliderBox.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include <SimpleMath.h>
#define _USE_MATH_DEFINES
#include <math.h>

// For the DirectX Math library
using namespace DirectX;

const float TERRAIN_MOVE[] = { 60, 0, 23 };
const float TERRAIN_SCALE[] = { .05f, .05f, .05f };


// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1920,			   // Width of the window's client area
		1080,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;
	normalVS = 0;
	normalPS = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete vertexShaderDebug;
	delete pixelShaderDebug;
	delete skyVS;
	delete skyPS;
	delete ParticleVS;
	delete ParticlePS;
	delete normalVS;
	delete normalPS;

	// Meshes
	delete melonMesh;
	delete columnMesh;
	delete floorMesh;

	// Materials
	delete melonMaterial;
	delete marbleMaterial;
	delete marbleHitMaterial;
	delete sandMaterial;

	delete Cam;
	delete player;

	skySRV->Release();
	skyDepth->Release();
	skyRast->Release();

	if (sampler) { sampler->Release(); sampler = 0; }
	if (melonTexture) { melonTexture->Release(); melonTexture = 0; }
	if (marbleTexture) { marbleTexture->Release(); marbleTexture = 0; }
	if (marbleHitTexture) { marbleHitTexture->Release(); marbleHitTexture = 0; }
	if (sandDiffuse) { sandDiffuse->Release(); sandDiffuse = 0; }
	if (sandNormal) { sandNormal->Release(); sandNormal = 0; }
	if (terrain) { terrain->ShutDown(); delete terrain; terrain = 0; }


	delete basicGeometry.cone;
	delete basicGeometry.cube;
	delete basicGeometry.cylinder;
	delete basicGeometry.helix;
	delete basicGeometry.sphere;
	delete basicGeometry.torus;

	for (UINT i = 0; i < Entities.size(); i++)
	{
		delete Entities[i];
		Entities[i] = 0;
	}
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	Cam = new Camera(width, height);
	player = new Player(Cam, device, context, vertexShader, pixelShader, ParticleVS, ParticlePS);
	terrain = new Terrain();
	bool result = terrain->InitialiseTerrain(device, "..//..//Assets//Setup.txt");
	if (!result)
	{
		printf("Could not initialise terrain");
		return;
	}

	CreateBasicGeometry();
	CreateMaterials();
	CreateModels();

	DirLight.AmbientColour = XMFLOAT4(.5f, .5f, .5f, 1.f);
	DirLight.DiffuseColour = XMFLOAT4(.5f, 0.5f, 0.5f, 1.f);
	DirLight.Direction = XMFLOAT3(1.f, 0.f, 1.f);

	TopLight.AmbientColour = XMFLOAT4(.1f, .1f, .1f, .1f);
	TopLight.DiffuseColour = XMFLOAT4(0.5f, 0.5f, .5f, 1.f);
	TopLight.Direction = XMFLOAT3(0.f, 2.f, 0.f);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateDDSTextureFromFile(device, L"..//..//Assets//Textures//SunnyCubeMap.dds", 0, &skySRV);

	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rs, &skyRast);

	//rasterizer description for debug box
	D3D11_RASTERIZER_DESC debugrs = {};
	debugrs.FillMode = D3D11_FILL_WIREFRAME;
	debugrs.CullMode = D3D11_CULL_NONE;
	device->CreateRasterizerState(&debugrs, &debugRast);

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &skyDepth);

	//device states for particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
	
	vertexShaderDebug = new SimpleVertexShader(device, context);
	vertexShaderDebug->LoadShaderFile(L"VertexShaderDebug.cso");

	pixelShaderDebug = new SimplePixelShader(device, context);
	pixelShaderDebug->LoadShaderFile(L"PixelShaderDebug.cso");

	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");

	ParticleVS = new SimpleVertexShader(device, context);
	ParticleVS->LoadShaderFile(L"ParticleVS.cso");

	ParticlePS = new SimplePixelShader(device, context);
	ParticlePS->LoadShaderFile(L"ParticlePS.cso");

	normalVS = new SimpleVertexShader(device, context);
	normalVS->LoadShaderFile(L"VertexShaderNormal.cso");

	normalPS = new SimplePixelShader(device, context);
	normalPS->LoadShaderFile(L"PixelShaderNormal.cso");
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	basicGeometry.cone = new Mesh("../../Assets/Models/cone.obj", device);
	basicGeometry.cube = new Mesh("../../Assets/Models/cube.obj", device);
	basicGeometry.cylinder = new Mesh("../../Assets/Models/cylinder.obj", device);
	basicGeometry.helix = new Mesh("../../Assets/Models/helix.obj", device);
	basicGeometry.sphere = new Mesh("../../Assets/Models/sphere.obj", device);
	basicGeometry.torus = new Mesh("../../Assets/Models/torus.obj", device);
}

void Game::CreateMaterials() {

	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//melon.tif", 0, &melonTexture);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//marble.jpg", 0, &marbleTexture);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//marbleHit.png", 0, &marbleHitTexture);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//sand.jpg", 0, &sandDiffuse);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//sandNormal.jpg", 0, &sandNormal);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//stoneWall.jpg", 0, &stoneWall);
	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//stoneWallNormal.jpg", 0, &stoneWallNormal);


	D3D11_SAMPLER_DESC sd = {};
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 16;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sd, &sampler);

	melonMaterial = new Material(vertexShader, pixelShader, melonTexture, sampler, XMFLOAT2(1.0f, 1.0f));
	marbleMaterial = new Material(vertexShader, pixelShader, marbleTexture, sampler, XMFLOAT2(1.0f, 1.0f));
	marbleHitMaterial = new Material(vertexShader, pixelShader, marbleHitTexture, sampler, XMFLOAT2(1.0f, 1.0f));
	sandMaterial = new Material(normalVS, normalPS, sandDiffuse, sampler, sandNormal, XMFLOAT2(1.0f, 1.0f));
	stoneMaterial = new Material(normalVS, normalPS, stoneWall, sampler, stoneWallNormal, XMFLOAT2(1.0f, 1.0f));
}

void Game::CreateModels() {

	wallMesh   = new Mesh("..//..//Assets//Models//wall.obj", device);
	melonMesh  = new Mesh("..//..//Assets//Models//melon.obj", device);
	floorMesh  = new Mesh("..//..//Assets//Models//floor.obj", device);
	columnMesh = new Mesh("..//..//Assets//Models//column.obj", device);

	// ------------------------
	// Create a ring of columns
	// ------------------------
	const int RING_RADIUS = 30;
	const int COLUMN_COUNT = 10;

	// How many degrees between the columns
	const float SPACING_RADIANS = 2 * (float)M_PI / COLUMN_COUNT;

	// The model transform is off by ~ this amount
	const float MODEL_VERTICAL_OFFSET = 2.5f;

	for (int columnNumber = 0; columnNumber < COLUMN_COUNT; columnNumber++) {

		//calculate the x and z position in the ring
		float xPosition = (cosf(SPACING_RADIANS * columnNumber) * RING_RADIUS) + 120;
		float zPosition = (sinf(SPACING_RADIANS * columnNumber) * RING_RADIUS) + 75;

		//adjust vertical height
		XMFLOAT3 columnPosition(xPosition, -player->playerHeight + MODEL_VERTICAL_OFFSET, zPosition);

		//create the entity setting pos and scale
		Column* column = new Column(columnMesh, marbleMaterial, marbleHitMaterial, this);
		
		column->SetPosition(columnPosition);
		Entities.push_back(column);
	}

	for (int i = 0; i < 4; i++)
	{
		Entity* wall = new Entity(wallMesh, stoneMaterial);
		switch (i)
		{
		case 0: wall->SetPosition(XMFLOAT3(70, -20, 75))->SetScale(XMFLOAT3(4, 4, 4)); break;
		case 1: wall->SetPosition(XMFLOAT3(170, -20, 75))->SetScale(XMFLOAT3(4, 4, 4)); break;
		case 2: wall->SetPosition(XMFLOAT3(120, -20, 127))->SetRotation(XMFLOAT3(0, 1.57f, 0))->SetScale(XMFLOAT3(4, 4, 4)); break; //Set
		case 3: wall->SetPosition(XMFLOAT3(120, -20, 25))->SetRotation(XMFLOAT3(0, 1.57f, 0))->SetScale(XMFLOAT3(4, 4, 4)); break; //Set


		}
		Entities.push_back(wall);
	}
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	Cam->CreateProjection(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	for (UINT i = 0; i < Entities.size(); i++)
		Entities[i]->Update(deltaTime);

	Cam->Update(deltaTime, totalTime);
	player->Update(deltaTime);

	//Set Player Heights based on Terrain Heights

	//Descale the Terrain
	XMFLOAT3 TruPos = Cam->GetPosition();
	XMFLOAT3 playerLoc = TruPos;
	//playerLoc.x *= .5
	//playerLoc.y *= .5;
	//playerLoc.z *= .5;



	std::cout << playerLoc.x << ' ' << playerLoc.y << ' ' << playerLoc.z << endl;
	//SimpleMath::Quaternion ro(0, -1.57, 0, 0);
	//XMVECTOR playVec = XMLoadFloat3(&playerLoc);
	//playVec = XMVector3Rotate(playVec, ro);
	//XMStoreFloat3(&playerLoc, playVec);

	//Detranslate the Terrain
	//playerLoc.x -= TERRAIN_MOVE[0];
	//playerLoc.y -= TERRAIN_MOVE[1];
	//playerLoc.z -= TERRAIN_MOVE[2];

	float height = terrain->GetHeight(playerLoc.x, playerLoc.z);

	if (playerLoc.x <= 0)
	{
		TruPos.x = 255;
	}
	if (playerLoc.x >= 256)
	{
		TruPos.x = 1;
	}

	if (playerLoc.z <= 0)
	{
		TruPos.z = 255;
	}
	if (playerLoc.z >= 256)
	{
		TruPos.z = 1;
	}

	Cam->SetPosition(XMFLOAT3(TruPos.x, TruPos.y, TruPos.z));

	std::cout << height << endl;

	if (playerLoc.y < height + 1)
	{
		playerLoc.y = 1 + height * TERRAIN_SCALE[1];
		Cam->SetPosition(XMFLOAT3(TruPos.x, playerLoc.y, TruPos.z));
		player->m_bGrounded = true;
	}

	else if (playerLoc.y > height + 1 && player->m_bGrounded)
	{
		playerLoc.y = height + 1;
		Cam->SetPosition(XMFLOAT3(TruPos.x, playerLoc.y, TruPos.z));

	}


}

// Draws a wireframe box at a certain position with scale
void Game::DrawBox(XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT4 color) {
	//switch to wireframe
	context->RSSetState(debugRast);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vert = basicGeometry.cube->GetVertexBuffer();

	context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
	context->IASetIndexBuffer(basicGeometry.cube->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	vertexShaderDebug->SetShader();
	pixelShaderDebug->SetShader();

	XMFLOAT4X4 m_mWorld;
	XMMATRIX tr = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX sc = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMStoreFloat4x4(&m_mWorld, XMMatrixTranspose(sc * tr));

	vertexShaderDebug->SetMatrix4x4("world", m_mWorld);
	vertexShaderDebug->SetMatrix4x4("view", Cam->GetViewMatrix());
	vertexShaderDebug->SetMatrix4x4("projection", Cam->GetProjectionMatrix());
	vertexShaderDebug->SetFloat4("color", color);
	vertexShaderDebug->CopyAllBufferData();

	context->DrawIndexed(basicGeometry.cube->GetIndexCount(), 0, 0);

	//switch back
	context->RSSetState(0);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.f, 0.f, 0.f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vert;

	TopLight.DiffuseColour.x -= sin(deltaTime / 6);

	pixelShader->SetShaderResourceView("SkyTexture", skySRV);
	pixelShader->SetSamplerState("BasicSampler", sampler);

	for (UINT i = 0; i < Entities.size(); i++)
	{

		Entities[i]->PrepareMaterial(Cam->GetViewMatrix(), Cam->GetProjectionMatrix());

		Entities[i]->material->GetPixelShader()->SetSamplerState("basicSampler", sampler);
		Entities[i]->material->GetPixelShader()->SetShaderResourceView("diffuseTexture", Entities[i]->material->GetSRV());
		Entities[i]->material->GetPixelShader()->SetShaderResourceView("normalTexture", Entities[i]->material->GetSRVNormal());


		Entities[i]->material->GetPixelShader()->SetData("topLight", &TopLight, sizeof(DirectionalLight));

		Entities[i]->material->GetPixelShader()->SetData("light", &DirLight, sizeof(DirectionalLight));

		Entities[i]->material->GetPixelShader()->CopyAllBufferData();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		vert = Entities[i]->mesh->GetVertexBuffer();

		context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
		context->IASetIndexBuffer(Entities[i]->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			Entities[i]->mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	vertexShader->SetShader();
	pixelShader->SetShader();

	XMFLOAT4X4 m_mWorld;
	XMMATRIX tr = XMMatrixTranslation(TERRAIN_MOVE[0], TERRAIN_MOVE[1], TERRAIN_MOVE[2]);
	XMMATRIX ro = XMMatrixRotationRollPitchYaw(0, -1.57f, 0);
	XMMATRIX sc = XMMatrixScaling(TERRAIN_SCALE[0], TERRAIN_SCALE[1], TERRAIN_SCALE[2]);

	XMStoreFloat4x4(&m_mWorld, XMMatrixTranspose(sc * ro * tr));

	vertexShader->SetMatrix4x4("world", m_mWorld);
	vertexShader->SetMatrix4x4("view", Cam->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", Cam->GetProjectionMatrix());

	vertexShader->CopyAllBufferData();

	pixelShader->SetSamplerState("basicSampler", sampler);
	pixelShader->SetShaderResourceView("diffuseTexture", sandDiffuse);

	pixelShader->SetData("topLight", &TopLight, sizeof(DirectionalLight));

	pixelShader->SetData("light", &DirLight, sizeof(DirectionalLight));

	pixelShader->CopyAllBufferData();

	terrain->Render(context);
	context->DrawIndexed(terrain->GetIndexCount(), 0, 0);



	for (UINT i = 0; i < player->Entities.size(); i++)

	{
		player->Entities[i]->PrepareMaterial(Cam->GetViewMatrix(), Cam->GetProjectionMatrix());

		pixelShader->SetSamplerState("basicSampler", sampler);
		pixelShader->SetShaderResourceView("diffuseTexture", player->Entities[i]->material->GetSRV());
		if (player->Entities[i]->material->m_hasNormal) {
			pixelShader->SetShaderResourceView("normalTexture", player->Entities[i]->material->GetSRVNormal());
		}

		pixelShader->SetData("topLight", &TopLight, sizeof(DirectionalLight));

		pixelShader->SetData("light", &DirLight, sizeof(DirectionalLight));

		pixelShader->CopyAllBufferData();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		vert = player->Entities[i]->mesh->GetVertexBuffer();

		context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
		context->IASetIndexBuffer(player->Entities[i]->mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			player->Entities[i]->mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices


	}

	for (UINT i = 0; i < Entities.size(); i++)
		Entities[i]->Render();


	ID3D11Buffer* skyVB = basicGeometry.cube->GetVertexBuffer();
	ID3D11Buffer* skyIB = basicGeometry.cube->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	skyVS->SetMatrix4x4("view", Cam->GetViewMatrix());
	skyVS->SetMatrix4x4("projection", Cam->GetProjectionMatrix());
	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	skyPS->SetShaderResourceView("SkyTexture", skySRV);
	skyPS->SetSamplerState("BasicSampler", sampler);
	skyPS->SetShader();

	context->RSSetState(skyRast);
	context->OMSetDepthStencilState(skyDepth, 0);
	context->DrawIndexed(basicGeometry.cube->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMGetDepthStencilState(0, 0);

	for (UINT i = 0; i < player->Entities.size(); i++)
	{
		float blend[4] = { 1,1,1,1 };
		context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
		context->OMSetDepthStencilState(particleDepthState, 0);

		player->Entities[i]->Draw(context, Cam);

		context->OMSetBlendState(0, blend, 0xffffffff);
		context->OMSetDepthStencilState(0, 0);
	}

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	RECT clientRect;
	SetRect(&clientRect, 0, 0, width, height);

	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
	int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);

	POINT point;
	GetCursorPos(&point);

	Cam->OnMouseMove(desktopRect.right / 2, desktopRect.bottom / 2, point.x, point.y);

	SetCursorPos(desktopRect.right / 2, desktopRect.bottom / 2);
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	player->SetActiveSpell(wheelDelta);
}
#pragma endregion