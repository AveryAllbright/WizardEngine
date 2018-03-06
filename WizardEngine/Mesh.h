#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Vertex.h"

class Mesh
{
private:

	ID3D11Buffer* m_pVertBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	int m_nIndexCount; // Specifies how many indices are in the IndexBuffer

public:
	
	Mesh( Vertex* a_pVertex, int a_nVertexCount, UINT* a_pIndices, int a_nIndexCount, ID3D11Device* a_pDevice);
	Mesh(char* a_cFileName, ID3D11Device * a_pDevice);

	~Mesh();

	ID3D11Buffer* GetVertexBuffer();

	ID3D11Buffer* GetIndexBuffer();

	int GetIndexCount();

	void CreateBuffers(Vertex* a_pVertex, int a_nVertexCount, UINT* a_pIndices, int a_nIndexCount, ID3D11Device* a_pDevice);
};

