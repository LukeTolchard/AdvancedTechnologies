//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>
#include <fstream>
#include <istream>

//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D10Device1* d3d101Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;
ID2D1RenderTarget* D2DRenderTarget;
ID3D11DepthStencilView* depthStencilView;
ID3D11DepthStencilState* DSLessEqual;
ID3D11Texture2D* depthStencilBuffer;
ID3D11Texture2D* BackBuffer11;
ID3D11Texture2D* sharedTex11;
ID3D11VertexShader* VS;
ID3D11VertexShader* SKYMAP_VS;
ID3D11PixelShader* PS;
ID3D11PixelShader* SKYMAP_PS;
ID3D11PixelShader* D2D_PS;
ID3D10Blob* D2D_PS_Buffer;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D10Blob* SKYMAP_VS_Buffer;
ID3D10Blob* SKYMAP_PS_Buffer;
ID3D11Buffer* cbPerObjectBuffer;
ID3D11Buffer* d2dVertBuffer;
ID3D11Buffer* d2dIndexBuffer;
ID3D11Buffer* cbPerFrameBuffer;
ID3D11Buffer* sphereIndexBuffer;
ID3D11Buffer* sphereVertBuffer;
ID3D11Buffer* meshVertBuff;
ID3D11Buffer* meshIndexBuff;
ID3D11BlendState* d2dTransparency;
ID3D11BlendState* Transparency;
ID3D11BlendState* leafTransparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* RSCullNone;
ID3D11SamplerState* CubesTexSamplerState;
ID2D1SolidColorBrush* Brush;
ID3D11ShaderResourceView* d2dTexture;
ID3D11ShaderResourceView* smrv;
ID3D11InputLayout* vertLayout;
IDWriteFactory* DWriteFactory;
IDWriteTextFormat* TextFormat;
IDXGIKeyedMutex* keyedMutex11;
IDXGIKeyedMutex* keyedMutex10;
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

XMMATRIX meshWorld;
int meshSubsets = 0;
std::vector<int> meshSubsetIndexStart;
std::vector<int> meshSubsetTexture;
std::vector<ID3D11ShaderResourceView*> meshSRV;
std::vector<std::wstring> textureNameArray;

std::wstring printText;

//Function Prototypes//
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void DrawScene();
bool InitD2D_D3D101_DWrite(IDXGIAdapter1* Adapter);
void InitD2DScreenTexture();
void UpdateScene(double time);
void UpdateCamera();
void RenderText(std::wstring text, int inInt);
void StartTimer();
double GetTime();
double GetFrameTime();
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
int messageloop();
bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);
void CreateSphere(int LatLines, int LongLines);

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);