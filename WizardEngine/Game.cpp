#include "Game.h"
#include "Vertex.h"
#include "ColliderBox.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

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
	delete skyVS;
	delete skyPS;
	delete pixelShader;
	delete Melon;
	delete melonMat;
	delete skyCube;

	delete floor;
	
	delete Cam;
	delete player;

	skySRV->Release();
	skyDepth->Release();
	skyRast->Release();

	if (sampler) { sampler->Release(); sampler = 0; }
	if (melonTexture) { melonTexture->Release(); melonTexture = 0; }	

	delete basicGeometry.cone;
	delete basicGeometry.cube;
	delete basicGeometry.cylinder;
	delete basicGeometry.helix;
	delete basicGeometry.sphere;
	delete basicGeometry.torus;
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
	CreateMatrices();
	CreateBasicGeometry();

	DirLight.AmbientColour = XMFLOAT4(.1f, .1f, .1f, 1.f);
	DirLight.DiffuseColour = XMFLOAT4(.5f, 0.1f, 0.1f, 1.f);
	DirLight.Direction = XMFLOAT3(1.f, 0.f, 1.f);

	TopLight.AmbientColour = XMFLOAT4(.1f, .1f, .1f, .1f);
	TopLight.DiffuseColour = XMFLOAT4(0.1f, 0.1f, .5f, 1.f);
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

	D3D11_DEPTH_STENCIL_DESC ds = {}; 
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &skyDepth);

	Cam = new Camera(width, height);
	player = new Player(Cam, device,context, vertexShader, pixelShader);
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

	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");
}

// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	//TODO: autoload all assets in folder and store as filename
	basicGeometry.cone = new Mesh("../../Assets/Models/cone.obj", device);
	basicGeometry.cube = new Mesh("../../Assets/Models/cube.obj", device);
	basicGeometry.cylinder = new Mesh("../../Assets/Models/cylinder.obj", device);
	basicGeometry.helix = new Mesh("../../Assets/Models/helix.obj", device);
	basicGeometry.sphere = new Mesh("../../Assets/Models/sphere.obj", device);
	basicGeometry.torus = new Mesh("../../Assets/Models/torus.obj", device);

	XMFLOAT3 standRot = XMFLOAT3(0, 0, 0);
	XMFLOAT3 standScale = XMFLOAT3(1, 1, 1);
	

	CreateWICTextureFromFile(device, context, L"..//..//Assets//Textures//melon.tif", 0, &melonTexture);
	
	D3D11_SAMPLER_DESC sd = {};
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 16;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sd, &sampler);

	melonMat = new Material(vertexShader, pixelShader, melonTexture, sampler);
	//crateMat = new Material(vertexShader, pixelShader, crateTexture, sampler);

	Melon = new Mesh("..//..//Assets//Models//melon.obj", device);
	//Crate = new Mesh("Models//cube.obj", device);
	
	skyCube = new Mesh("..//..//Assets//Models//cube.obj", device);

	floor = new Mesh("..//..//Assets//Models//floor.obj", device);
	
	Entities.push_back(Entity(Melon, melonMat, worldMatrix, XMFLOAT3(0, 0, 0), standRot, standScale));

	Entities[0].SetScale(XMFLOAT3(.125, .125, .125));

	Entities[0].UpdateWorldView();

	

	ColliderBox* melonCollider = new ColliderBox(DirectX::XMFLOAT3(0, 0, 0));
	melonCollider->isTrigger = false;
	Entities[0].AddComponent(melonCollider);
	melonCollider->onCollisionEnterFunction = &Entity::HandleCollision;

	Entities.push_back(Entity(Melon, melonMat, worldMatrix, XMFLOAT3(0, 0, 0), standRot, standScale));
	
	ColliderBox* melonCollider2 = new ColliderBox(DirectX::XMFLOAT3(0, 0, 0));
	melonCollider2->isTrigger = false;
	Entities[1].AddComponent(melonCollider2);
	melonCollider2->onCollisionEnterFunction = &Entity::HandleCollision;
	Entities[1].SetScale(XMFLOAT3(.125, .125, .125));

	for (int i = -5; i < 5; i++)
	{
		for (int j = -5; j < 5; j++)
		{
			Entities.push_back(Entity(floor, melonMat, worldMatrix, XMFLOAT3(i * 20, -2.5, j * 20), standRot, standScale));
			Entities[Entities.size() - 1].UpdateWorldView();
		}
	}
	
	
	Entities[1].UpdateWorldView();
	
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
	DirectX::XMFLOAT3 e1pos = Entities[1].GetPosition();
	float speed = 10;
	float change = sinf(totalTime * speed) * .02;
	e1pos.x += change;
	Entities[1].getComponent<ColliderBox>()->SetCenter(e1pos);
	Entities[1].SetPosition(e1pos);
	Entities[1].UpdateWorldView();
	Entities[1].Update(deltaTime);
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	Cam->Update(deltaTime, totalTime);
	player->Update(deltaTime);
	
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

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

		Entities[i].PrepareMaterial(Cam->GetViewMatrix(), Cam->GetProjectionMatrix());
		
		pixelShader->SetSamplerState("basicSampler", sampler);
		pixelShader->SetShaderResourceView("diffuseTexture", Entities[i].GetMaterial()->GetSRV());
		
		pixelShader->SetData(			"topLight",			&TopLight,			sizeof(DirectionalLight)		);

		pixelShader->SetData(			"light",			&DirLight,			sizeof(DirectionalLight)		);

		pixelShader->CopyAllBufferData();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		vert = Entities[i].GetMesh()->GetVertexBuffer();

		context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
		context->IASetIndexBuffer(Entities[i].GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			Entities[i].GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	for (UINT i = 0; i < player->EntitiesOne.size(); i++)
	{
		
		player->EntitiesOne[i].PrepareMaterial(Cam->GetViewMatrix(), Cam->GetProjectionMatrix());

		pixelShader->SetSamplerState("basicSampler", sampler);
		pixelShader->SetShaderResourceView("diffuseTexture", player->EntitiesOne[i].GetMaterial()->GetSRV());

		pixelShader->SetData("topLight", &TopLight, sizeof(DirectionalLight));

		pixelShader->SetData("light", &DirLight, sizeof(DirectionalLight));

		pixelShader->CopyAllBufferData();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		vert = player->EntitiesOne[i].GetMesh()->GetVertexBuffer();

		context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
		context->IASetIndexBuffer(player->EntitiesOne[i].GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			player->EntitiesOne[i].GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	for (UINT i = 0; i < player->EntitiesTwo.size(); i++)
	{

		player->EntitiesTwo[i].PrepareMaterial(Cam->GetViewMatrix(), Cam->GetProjectionMatrix());

		pixelShader->SetSamplerState("basicSampler", sampler);
		pixelShader->SetShaderResourceView("diffuseTexture", player->EntitiesTwo[i].GetMaterial()->GetSRV());

		pixelShader->SetData("topLight", &TopLight, sizeof(DirectionalLight));

		pixelShader->SetData("light", &DirLight, sizeof(DirectionalLight));

		pixelShader->CopyAllBufferData();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		vert = player->EntitiesTwo[i].GetMesh()->GetVertexBuffer();

		context->IASetVertexBuffers(0, 1, &vert, &stride, &offset);
		context->IASetIndexBuffer(player->EntitiesTwo[i].GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(
			player->EntitiesTwo[i].GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	ID3D11Buffer* skyVB = skyCube->GetVertexBuffer();
	ID3D11Buffer* skyIB = skyCube->GetIndexBuffer();

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
	context->DrawIndexed(skyCube->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMGetDepthStencilState(0, 0);
	
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