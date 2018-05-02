#include "Material.h"
//I exist now!
//I currently do nothing!

Material::Material(SimpleVertexShader* a_pVert, SimplePixelShader* a_pPix, ID3D11ShaderResourceView* a_pSrv, ID3D11SamplerState* a_pSamp)
{
	vertexShader = a_pVert;
	pixelShader = a_pPix;
	m_pSamp = a_pSamp;
	m_pSRV = a_pSrv;
	m_hasNormal = false;
	m_uvTiling = DirectX::XMFLOAT2(1.0f, 1.0f);
}

Material::Material(SimpleVertexShader* a_pVert, SimplePixelShader* a_pPix, ID3D11ShaderResourceView* a_pSrv, ID3D11SamplerState* a_pSamp, DirectX::XMFLOAT2 a_uvTiling)
{
	vertexShader = a_pVert;
	pixelShader = a_pPix;
	m_pSamp = a_pSamp;
	m_pSRV = a_pSrv;
	m_hasNormal = false;
	m_uvTiling = DirectX::XMFLOAT2(1.0f, 1.0f);
}

Material::Material(SimpleVertexShader* a_pVert, SimplePixelShader* a_pPix, ID3D11ShaderResourceView* a_pSrv, ID3D11SamplerState* a_pSamp, ID3D11ShaderResourceView* a_pSrvNormal, DirectX::XMFLOAT2 a_uvTiling)
{
	vertexShader = a_pVert;
	pixelShader = a_pPix;
	m_pSamp = a_pSamp;
	m_pSRV = a_pSrv;
	m_pSRVNormal = a_pSrvNormal;
	m_hasNormal = true;
	m_uvTiling = DirectX::XMFLOAT2(1.0f, 1.0f);
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

ID3D11ShaderResourceView * Material::GetSRVNormal()
{
	return m_pSRVNormal;
}