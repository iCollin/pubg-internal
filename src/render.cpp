#include "render.h"

#include "engine.h"
#include "SDK.hpp"

#define MAX_VERTICIES 16384 * 8 * 4 * 3

struct XMCOLOR {
	union {
		struct {
			uint32_t b;
			uint32_t g;
			uint32_t r;
			uint32_t a;
		};
		uint32_t c;
	};

	XMCOLOR(float r, float g, float b, float a)
	{
		uint32_t abyte = (uint32_t)(a * 255.f) << 24;
		uint32_t rbyte = (uint32_t)(r * 255.f) << 16;
		uint32_t gbyte = (uint32_t)(g * 255.f) << 8;
		uint32_t bbyte = (uint32_t)(b * 255.f);

		c = abyte | rbyte | gbyte | bbyte;
	}
};

Render::Render(ID3D11Device * device) : mDevice(device)
{
	mDevice->GetImmediateContext(&mContext);

	/*if (!FAILED(FW1CreateFactory(FW1_VERSION, &mFontFactory)))
	{
		mFontFactory->CreateFontWrapper(mDevice, L"Verdana", &mFontWrapper);
		mFontFactory->CreateTextGeometry(&mTextGeometry);
	}*/

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ ("POSITION"),	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, (size_t)(&((Vertex*)0)->pos),	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ ("COLOR"),	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, (size_t)(&((Vertex*)0)->col),	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	/*
	LPCSTR SemanticName;
    UINT SemanticIndex;
    DXGI_FORMAT Format;
    UINT InputSlot;
    UINT AlignedByteOffset;
    D3D11_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
	*/

	ID3D10Blob* vsBlob;
	GenerateShader(&mPixelShader, 0.8f, 0.2f, 0.2f);
	GenerateShader(&mVertexShader, 0.2f, 0.2f, 0.8f, &vsBlob);

	HRESULT hr = mDevice->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);

	if (FAILED(hr))
		return;

	D3D11_BUFFER_DESC vertex = {};
	vertex.Usage = D3D11_USAGE_DYNAMIC;
	vertex.ByteWidth = sizeof(Vertex) * static_cast<UINT>(MAX_VERTICIES);
	vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertex.MiscFlags = 0;
	mDevice->CreateBuffer(&vertex, nullptr, &mVertexBuffer);

	D3D11_BLEND_DESC blend = {};
	blend.RenderTarget->BlendEnable = TRUE;
	blend.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
	blend.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mDevice->CreateBlendState(&blend, &mBlendState);

	D3D11_BUFFER_DESC screen = {};
	screen.Usage = D3D11_USAGE_DYNAMIC;
	screen.ByteWidth = sizeof(DirectX::XMMATRIX);
	screen.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screen.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screen.MiscFlags = 0;
	mDevice->CreateBuffer(&screen, nullptr, &mScreenBuffer);

	UINT numViewports = 1;

	mContext->RSGetViewports(&numViewports, &mViewport);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.f, mViewport.Width, mViewport.Height, 0.f, -100.f, 100.f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	mContext->Map(mScreenBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	std::memcpy(mappedResource.pData, &proj, sizeof(DirectX::XMMATRIX));

	mContext->Unmap(mScreenBuffer, 0);
}

Render::~Render()
{
	mVertexShader->Release();
	mPixelShader->Release();
	mVertexBuffer->Release();
	mScreenBuffer->Release();
	if (mInputLayout) mInputLayout->Release();
	mBlendState->Release();
}

char d3dcode[] =
"cbuffer spb : register(b0)"
"{"
"matrix proj;"
"};"

"struct V_O"
"{"
"float4 p : SV_POSITION;"
"float4 c : COLOR;"
"};"

"struct V_I"
"{"
"float4 p : POSITION;"
"float4 c : COLOR;"
"};"

"V_O VS(V_I extremelylonginputnamedontsigmepls)"
"{"
"V_O yesimtheoutputandimlongdontjudgeme;"

"yesimtheoutputandimlongdontjudgeme.p = mul(proj, float4(extremelylonginputnamedontsigmepls.p.xy, 0.f, 1.f));"
"yesimtheoutputandimlongdontjudgeme.c = extremelylonginputnamedontsigmepls.c;"

"return yesimtheoutputandimlongdontjudgeme;"
"}"

"float4 PS(V_O extremelylonginputnamedontsigmepls) : SV_TARGET"
"{"
"return extremelylonginputnamedontsigmepls.c;"
"}";

HRESULT Render::GenerateShader(ID3D11PixelShader** pS, float r, float g, float b, ID3D10Blob** outBlob)
{
	ID3D10Blob* pBlob;
	char szPixelShader[1000];
	sprintf_s(szPixelShader, d3dcode, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	auto hr = D3DCompile(szPixelShader, sizeof(szPixelShader), rxorc("shader"), NULL, NULL, "PS", rxorc("ps_4_0"), NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = mDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pS);

	if (FAILED(hr))
		return hr;

	if (outBlob)
		*outBlob = pBlob;

	return S_OK;
}

HRESULT Render::GenerateShader(ID3D11VertexShader** vS, float r, float g, float b, ID3D10Blob** outBlob)
{
	ID3D10Blob* pBlob;
	char szPixelShader[1000];
	sprintf_s(szPixelShader, d3dcode, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), rxorc("shader"), NULL, NULL, "VS", rxorc("vs_4_0"), NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = mDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, vS);

	if (FAILED(hr))
		return hr;

	if (outBlob)
		*outBlob = pBlob;

	return S_OK;
}

void Render::Begin()
{
	mRestoreState = false;
	mStateManager.saveCurrentState(mContext);
	mRestoreState = true;

	mContext->VSSetShader(mVertexShader, nullptr, 0);
	mContext->PSSetShader(mPixelShader, nullptr, 0);

	mContext->OMSetBlendState(mBlendState, nullptr, 0xffffffff);
	mContext->VSSetConstantBuffers(0, 1, &mScreenBuffer);
	mContext->IASetInputLayout(mInputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	//mFontWrapper->DrawString(mContext, L"", 0.f, 0.f, 0.f, 0xff000000, FW1_RESTORESTATE | FW1_NOFLUSH);
}

void Render::Draw()
{
	if (mList.verticies.size() > 0)
	{
		D3D11_MAPPED_SUBRESOURCE res;
		mContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
		std::memcpy(res.pData, mList.verticies.data(), sizeof(Vertex) * mList.verticies.size());
		mContext->Unmap(mVertexBuffer, 0);
	}

	size_t pos = 0;

	for (const auto& batch : mList.batches)
	{
		mContext->IASetPrimitiveTopology(batch.topology);
		mContext->Draw(batch.count, pos);
		pos += batch.count;
	}

	//mFontWrapper->Flush(mContext);
	//mFontWrapper->DrawGeometry(mContext, mTextGeometry, nullptr, nullptr, FW1_RESTORESTATE);
}

void Render::End()
{
	mList.batches.clear();
	mList.verticies.clear();
	//mTextGeometry->Clear();

	if (mRestoreState) mStateManager.restoreSavedState();
}

void Render::AddVertex(Vertex& v, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if (mList.verticies.size() >= MAX_VERTICIES)
		return;

	if (mList.batches.empty() || mList.batches.back().topology != topology)
		mList.batches.emplace_back(0, topology);

	mList.batches.back().count += 1;
	mList.verticies.push_back(v);
}

void Render::AddVerticies(std::vector<Vertex> v, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	size_t endSize = mList.verticies.size() + v.size();
	if (endSize >= MAX_VERTICIES)
		return;

	if (mList.batches.empty() || mList.batches.back().topology != topology)
		mList.batches.emplace_back(0, topology);

	mList.batches.back().count += v.size();

	mList.verticies.resize(endSize);
	std::memcpy(&mList.verticies[mList.verticies.size() - v.size()], &v[0], v.size() * sizeof(Vertex));

	switch (topology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
	{
		Vertex seperator{};
		AddVertex(seperator, D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
		break;
	}
	default:
		break;
	}
}

void Render::Line(float x, float y, float j, float k, const Color &color)
{
	std::vector<Vertex> v
	{
		{ x, y, 0.0f, color },
		{ j, k, 0.0f, color }
	};

	AddVerticies(v, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}


void Render::Line(Classes::FVector2D origin, Classes::FVector2D dest, Color col)
{
	Line(origin.X, origin.Y, dest.X, dest.Y, col);
}

void Render::Box(float x, float y, float j, float k, const Color& color)
{
	Line( x,y, j, y, color);
	Line( j, y, j, k, color);
	Line( j,k, x, k, color);
	Line( x, k, x, y, color);
}

void Render::Clear(const Vec4& rect, const Color& color)
{
	std::vector<Vertex> v
	{
		{ rect.x,			rect.y,				0.f, color },
		{ rect.x + rect.z,	rect.y,				0.f, color },
		{ rect.x,			rect.y + rect.w,	0.f, color },

		{ rect.x + rect.z,	rect.y,				0.f, color },
		{ rect.x + rect.z,	rect.y + rect.w,	0.f, color },
		{ rect.x,			rect.y + rect.w,	0.f, color }
	};

	AddVerticies(v, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Render::Text(int x, int y, const std::wstring& text, const Color& color, uint32_t flags, float fontSize)
{
	//auto shadowColor = XMCOLOR(0.1f, 0.1f, 0.1f, 0.85f);
	//FW1_RECTF shadowRect = { x + 1.0f, y + 1.0f, x + 1.0f, y + 1.0f };

	//mFontWrapper->AnalyzeString(nullptr, text.c_str(), L"Verdana", fontSize, &shadowRect, shadowColor.c, flags | FW1_NOFLUSH | FW1_NOWORDWRAP, mTextGeometry);

	//auto realColor = XMCOLOR(color.f[0], color.f[1], color.f[2], color.f[3]);
	//FW1_RECTF realRect = { x, y, x, y };

	//mFontWrapper->AnalyzeString(nullptr, text.c_str(), L"Verdana", fontSize, &realRect, realColor.c, flags | FW1_NOFLUSH | FW1_NOWORDWRAP, mTextGeometry);
}

void Render::WorldLine(Classes::FVector origin, Classes::FVector dest, Color col)
{
	Classes::FVector2D sOrigin;
	Classes::FVector2D sDest;

	if (!Engine::W2S(origin, &sOrigin) || !Engine::W2S(dest, &sDest))
		return;

	Line(sOrigin.X, sOrigin.Y, sDest.X, sDest.Y, col);
}

void Render::Box3D(Classes::FVector origin, Classes::FVector extends, Color col)
{
	origin -= extends / 2.f;

	// bottom plane
	Classes::FVector one = origin;
	Classes::FVector two = origin; two.X += extends.X;
	Classes::FVector three = origin; three.X += extends.X; three.Y += extends.Y;
	Classes::FVector four = origin; four.Y += extends.Y;

	Classes::FVector five = one; five.Z += extends.Z;
	Classes::FVector six = two; six.Z += extends.Z;
	Classes::FVector seven = three; seven.Z += extends.Z;
	Classes::FVector eight = four; eight.Z += extends.Z;

	Classes::FVector2D s1, s2, s3, s4, s5, s6, s7, s8;
	if (!Engine::W2S(one, &s1) || !Engine::W2S(two, &s2) || !Engine::W2S(three, &s3) || !Engine::W2S(four, &s4))
		return;

	if (!Engine::W2S(five, &s5) || !Engine::W2S(six, &s6) || !Engine::W2S(seven, &s7) || !Engine::W2S(eight, &s8))
		return;

	// 3 copies of each vertex? trash
	Line(s1, s2, col);
	Line(s2, s3, col);
	Line(s3, s4, col);
	Line(s4, s1, col);

	Line(s5, s6, col);
	Line(s6, s7, col);
	Line(s7, s8, col);
	Line(s8, s5, col);

	Line(s1, s5, col);
	Line(s2, s6, col);
	Line(s3, s7, col);
	Line(s4, s8, col);
}

void Render::Plus(float x, float y, float size, Color col)
{
	Line(x - size, y, x + size, y, col);
	Line(x, y - size, x, y + size, col);
}
