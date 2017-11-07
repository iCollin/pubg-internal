#pragma once

#include "DXStateSaver.h"

#include <string>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <vector>
#include "colors.h"

#define rxorc(s) s

//#include "FW1/FW1FontWrapper.h"

struct Vertex;
struct Batch;
struct Vec2;
struct Vec3;
struct Vec4;

namespace Classes
{
	struct FVector;
	struct FVector2D;
}

class Render
{
public:
	struct List
	{
		std::vector<Vertex> verticies;
		std::vector<Batch> batches;
	};

	Render(ID3D11Device* device);
	~Render();

	void Begin();
	void Draw();
	void End();

	void Line(float x, float y, float j, float k, const Color &color);
	void Line(Classes::FVector2D origin, Classes::FVector2D dest, Color col);
	void WorldLine(Classes::FVector origin, Classes::FVector dest, Color col);
	// rect: w x y h
	void Clear(const Vec4& rect, const Color& col);
	void Box(float x, float y, float j, float k, const Color& color);
	void Box3D(Classes::FVector origin, Classes::FVector extends, Color col);
	void Plus(float x, float y, float size, Color col);

	void Text(int x, int y, const std::wstring& text, const Color& color, uint32_t flags = 1, float fontSize = 12.f);

	D3D11_VIEWPORT mViewport;

protected:
	HRESULT GenerateShader(ID3D11PixelShader** pS, float r, float g, float b, ID3D10Blob** outBlob = nullptr);
	HRESULT GenerateShader(ID3D11VertexShader** vS, float r, float g, float b, ID3D10Blob** outBlob = nullptr);

	void AddVertex(Vertex& v, D3D11_PRIMITIVE_TOPOLOGY topology);
	void AddVerticies(std::vector<Vertex> v, D3D11_PRIMITIVE_TOPOLOGY topology);

private:
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11BlendState* mBlendState = nullptr;
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mScreenBuffer = nullptr;

	//IFW1Factory* mFontFactory;
	//IFW1FontWrapper* mFontWrapper;
	//IFW1TextGeometry* mTextGeometry;

	List mList;

	bool mRestoreState = false;
	DXTKStateSaver mStateManager;
};

struct Vec2
{
	float x;
	float y;
}; 

struct Vec3
{
	float x;
	float y;
	float z;
	Vec3(float i, float j, float k) : x(i), y(j), z(k) {}
};

struct Vec4
{
	float w;
	float x;
	float y;
	float z;
};

struct Vertex
{
	Vertex() : pos(0.f, 0.f, 0.f) {}
	Vertex(float x, float y, float z, Color color) : pos(x, y, z), col(color) {}

	Vec3 pos;
	Color col = {0.f, 0.f, 0.f, 0.f};
};

struct Batch
{
	Batch(size_t i, D3D11_PRIMITIVE_TOPOLOGY t) : count(i), topology(t) {}

	size_t count;
	D3D11_PRIMITIVE_TOPOLOGY topology;
};