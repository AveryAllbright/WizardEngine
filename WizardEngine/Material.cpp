#include "Material.h"
#include "override_new.h"
//I exist now!
//I currently do nothing!

Material::Material(SimpleVertexShader* a_pVert, SimplePixelShader* a_pPix, ID3D11ShaderResourceView* a_pSrv, ID3D11SamplerState* a_pSamp)
{
	vertexShader = a_pVert;
	pixelShader = a_pPix;
	m_pSamp = a_pSamp;
	m_pSRV = a_pSrv;
}

Material::~Material()
{
}

SimpleVertexShader * Material::GetVertShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetSRV()
{
	return m_pSRV;
}

ID3D11SamplerState * Material::GetSampler()
{
	return m_pSamp;
}
