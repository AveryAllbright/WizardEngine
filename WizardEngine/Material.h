#pragma once
#include "SimpleShader.h"
class Material
{

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* m_pSRV;
	ID3D11SamplerState* m_pSamp;

public:
	Material(SimpleVertexShader* a_pVert, SimplePixelShader* a_pPix, ID3D11ShaderResourceView* a_pSrv, ID3D11SamplerState* a_pSamp);
	~Material();

	SimpleVertexShader* GetVertShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetSRV();
	ID3D11SamplerState* GetSampler();

};

