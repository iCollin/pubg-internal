#pragma once

#include <d3d11.h>

class DXTKStateSaver
{
private:
	bool m_savedState;
	D3D_FEATURE_LEVEL m_featureLevel;
	ID3D11DeviceContext* m_pContext;
	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11BlendState* m_pBlendState;
	float m_blendFactor[4];
	UINT m_sampleMask;
	ID3D11DepthStencilState* m_pDepthStencilState;
	UINT m_stencilRef;
	ID3D11RasterizerState* m_pRasterizerState;
	ID3D11ShaderResourceView* m_pPSSRV;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11VertexShader* m_pVS;
	ID3D11ClassInstance* m_pVSClassInstances[256];
	UINT m_numVSClassInstances;
	ID3D11Buffer* m_pVSConstantBuffer;
	ID3D11GeometryShader* m_pGS;
	ID3D11ClassInstance* m_pGSClassInstances[256];
	UINT m_numGSClassInstances;
	ID3D11Buffer* m_pGSConstantBuffer;
	ID3D11ShaderResourceView* m_pGSSRV;
	ID3D11PixelShader* m_pPS;
	ID3D11ClassInstance* m_pPSClassInstances[256];
	UINT m_numPSClassInstances;
	ID3D11HullShader* m_pHS;
	ID3D11ClassInstance* m_pHSClassInstances[256];
	UINT m_numHSClassInstances;
	ID3D11DomainShader* m_pDS;
	ID3D11ClassInstance* m_pDSClassInstances[256];
	UINT m_numDSClassInstances;
	ID3D11Buffer* m_pVB;
	UINT m_vertexStride;
	UINT m_vertexOffset;
	ID3D11Buffer* m_pIndexBuffer;
	DXGI_FORMAT m_indexFormat;
	UINT m_indexOffset;

	DXTKStateSaver(const DXTKStateSaver&);
	//DXTKStateSaver& operator=(const DXTKStateSaver&);

public:
	DXTKStateSaver();
	~DXTKStateSaver();

	HRESULT saveCurrentState(ID3D11DeviceContext* pContext);
	HRESULT restoreSavedState();
	void ZeroShaders(ID3D11DeviceContext* pContext);
	void releaseSavedState();
};

template<class T> inline void SAFE_DELETE(T *&p)
{
	if (p)
	{
		delete p;
		p = NULL;
	}
}

template<class T> inline void SAFE_DELETE_ARRAY(T *&p)
{
	if (p)
	{
		delete[] p;
		p = NULL;
	}
}

template<class T> inline void SAFE_RELEASE(T *&p)
{
	if (p)
	{
		p->Release();
		p = NULL;
	}
}