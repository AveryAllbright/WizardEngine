#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace DirectX;
using namespace std;

class Terrain
{
public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool InitialiseTerrain(ID3D11Device*, char*);
	void ShutDown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	float GetHeight(float, float);
	

private:

	struct VertexTerrain
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT4 m_vColour;
	};

	struct HeightMap
	{
		float x, y, z;
	};

	struct Model
	{
		float x, y, z;
	};

	bool InitialiseBuffer(ID3D11Device*);
	void ShutdownBuffer();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	bool BuildTerrain();
	void ShutdownTerrain();
	void GenerateHeights();

	int m_nTerrainWidth, m_nTerrainHeight;
	int m_nVertexCount, m_nIndexCount;
	ID3D11Buffer *m_vertexBuffer, *m_IndexBuffer;

	float m_fHeightScale;
	char* m_terrainFile;
	HeightMap* m_HeightMap;
	Model* m_Model;

	vector<vector<float>> Heights;
	
};