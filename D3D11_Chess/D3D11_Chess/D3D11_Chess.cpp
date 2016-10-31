//--------------------------------------------------------------------------------------
//
//                  2014-2015 Bahar Dönemi BIL320 Bilgisayar Grafikleri I
//                  Dönem Projesi  : "DirectX 11 ile 3D Satranç Oyunu"
//					Son Güncelleme : 19 Mart 2015
//
//--------------------------------------------------------------------------------------

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"

#include <DXGI.h>
#include <dinput.h>

#include <fstream>
#include <sstream>
using namespace std;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

struct ConstantBuffer
{  
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 vEyePos;
	XMFLOAT4 vLightPos[2];
	XMFLOAT4 vMeshColor;
	XMFLOAT4 vLightColor[2];
}cBuffer;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11PixelShader*                  g_pPixelShader_Textured = NULL;
ID3D11PixelShader*                  g_pPixelShader_Solid = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Pointer = NULL;
ID3D11Buffer*                       g_pVertexBuffer_CrossHair = NULL;
ID3D11Buffer*                       g_pVertexBuffer_chessBoard = NULL;
ID3D11Buffer*                       g_pVertexBuffer_piyon = NULL;
ID3D11Buffer*                       g_pVertexBuffer_kale = NULL;
ID3D11Buffer*                       g_pVertexBuffer_at = NULL;;;;
ID3D11Buffer*                       g_pVertexBuffer_fil = NULL;
ID3D11Buffer*                       g_pVertexBuffer_vezir = NULL;
ID3D11Buffer*                       g_pVertexBuffer_sah = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Light = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;
ID3D11Buffer*                       g_pConstantBuffer = NULL;
ID3D11ShaderResourceView*           g_pTexturepiyon = NULL;
ID3D11ShaderResourceView*           g_pTextureat = NULL;
ID3D11ShaderResourceView*           g_pTexturekale = NULL;
ID3D11ShaderResourceView*           g_pTexturefil = NULL;
ID3D11ShaderResourceView*           g_pTexturevezir = NULL;
ID3D11ShaderResourceView*           g_pTexturesah = NULL;
ID3D11ShaderResourceView*           g_pTexturepointerWhite = NULL;
ID3D11ShaderResourceView*           g_pTexturepointerBlack = NULL;
ID3D11ShaderResourceView*           g_pTextureselectblackptr = NULL;
ID3D11ShaderResourceView*           g_pTextureselectwhiteptr = NULL;

ID3D11ShaderResourceView*           g_pTexturedangerWhiteptr = NULL;
ID3D11ShaderResourceView*           g_pTexturedangerBlackptr = NULL;

ID3D11ShaderResourceView*           g_pTexturechessBoard = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;


int vertexCountpiyon;
int vertexCountkale;
int vertexCountat;
int vertexCountfil;
int vertexCountvezir;
int vertexCountsah;
int vertexCountpointer;
int vertexCountchessBoard;
int vertexCountlight;
int vertexCountcrosshair;

SimpleVertex* verticespointer;
SimpleVertex* verticeschessBoard;
SimpleVertex* verticespiyon;
SimpleVertex* verticeskale;
SimpleVertex* verticesat;
SimpleVertex* verticesfil;
SimpleVertex* verticesvezir;
SimpleVertex* verticessah;
SimpleVertex* verticesLight;
SimpleVertex* verticesCrossHair;

XMVECTOR Ro;
XMVECTOR Rd ;

// Oyuncu sýrasý (true:beyaz, false:siyah)
bool turn = true;

bool leftButtonUp = true;
bool rightButtonUp = true;


//-------------------------------------------------------------------------------------
// DIRECT INPUT
//-------------------------------------------------------------------------------------
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rot				= 0.01f;
double countsPerSecond	= 0.0;
__int64 CounterStart	= 0;
int frameCount			= 0;
int fps					= 0;
__int64 frameTimeOld	= 0;
double frameTime;

bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);
void UpdateScene(double time);
void StartTimer();
double GetTime();
double GetFrameTime();

int label_matrix[8][8] =
{
	1, 2,  3, 4,  5,  3,  2, 1,   // black stones
	6, 6,  6, 6,  6,  6,  6, 6, 
	0, 0,  0, 0,  0,  0,  0, 0,
	0, 0,  0, 0,  0,  0,  0, 0,
	0, 0,  0, 0,  0,  0,  0, 0,
	0, 0,  0, 0,  0,  0,  0, 0,
	7, 7,  7, 7,  7,  7,  7, 7,   // white stones
	8, 9, 10, 11, 12, 10, 9, 8	  
};

// Movement vectors  

int Pawn1forward[2]  = { 1, 0 };		// PAWN VECTORS
int Pawn2forward[2]  = { 2, 0 };
int Pawn1back[2]     = { -1, 0 };
int Pawn2back[2]     = { -2, 0 };
int HorseP2x_P1y[2]  = { 2, 1 };		//HORSE VECTORS
int HorseP1x_P2y[2]  = { 1, 2 };
int HorseN1x_P2y[2]  = {-1, 2 };
int HorseN2x_P1y[2]  = {-2, 1 };
int HorseN2x_N1y[2]  = {-2, -1};
int HorseN1x_N2y[2]  = {-1, -2 };
int HorseP1x_N2y[2]  = { 1, -2 };
int HorseP2x_N1y[2]  = { 2, -1 };
int BishopP1x_P1y[2] = { 1, 1 };		// BÝSHOP VECTORS N=1..7
int BishopN1x_P1y[2] = { -1, 1 };
int BishopP1x_N1y[2] = { 1, -1 };
int BishopN1x_N1y[2] = { -1, -1 };
int RookP1x_Zy[2]	 = { 1, 0 };		//ROOK VECTORS N=1..7
int RookZx_P1y[2]	 = { 0, 1 };
int RookN1x_Zy[2]	 = {-1, 0 };
int RookZx_N1y[2]	 = { 0, -1 };

//*********************************************************************************************************\\
// NOTE = VIZIER VECTORS = Bishop AND Rook vectors for n=1..7 && SHAH VECTORS = All vizier vectors for n=1 \\
//*********************************************************************************************************\\

int *labelmatPtr;     //  label matrix pointer
int labelbuffer1, Lx, Ly, Rx, Ry, labelbuffer2[32] = { 0 };		// label buffers ..
int ind = 0;  // labelbuffer2 indisi
int Black_or_White[8][8] = 
{
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
};

//-------------------------------------------------------------------------------------
// FIRST PERSON CAMERA
//-------------------------------------------------------------------------------------

XMVECTOR Eye ;
XMVECTOR At ;
XMVECTOR Up ;

XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight	= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight		= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMVECTOR crosshairPosition= XMVectorSet(0.0f,0.0f,0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;

void UpdateCamera();


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render(double time);

XMVECTOR testIntersection(XMVECTOR rayOrigin, XMVECTOR rayDir, SimpleVertex* verticesModel, int vertexCount, bool transform, XMMATRIX& g_World);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

	if(!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			frameCount++;
			if(GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}	

			frameTime = GetFrameTime();

			DetectInput(frameTime);

            Render(frameTime);
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1280, 720 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"DirectX11 ile 3D Satranç Oyunu", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

bool InitDirectInput(HINSTANCE hInstance)
{
	HRESULT HR = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL); 
	HR = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	HR = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	HR = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	HR = DIKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR = DIMouse->SetDataFormat(&c_dfDIMouse);
	HR = DIMouse->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}

void DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];
	DIKeyboard->Acquire();
	DIMouse->Acquire();

	XMVECTOR translatePointer = testIntersection(Ro, Rd, verticeschessBoard, vertexCountchessBoard, false, cBuffer.mWorld);
	XMFLOAT3 translateP;
	XMStoreFloat3(&translateP, translatePointer);

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);
	if(keyboardState[DIK_ESCAPE] & 0x80) PostMessage(g_hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0F * time;

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}

	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}

	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}

	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}

	if(mouseCurrState.rgbButtons[0] & 0x80)  // mouse left click
	{
		Lx = (17.5 - translateP.z) / 5.0F;
		Ly = (17.5 + translateP.x) / 5.0F;
		labelbuffer1 = label_matrix[Lx][Ly]; 
		
		
	}

	if(mouseCurrState.rgbButtons[1] & 0x80)  // mouse right click
	{
		
		Rx = (17.5 - translateP.z) / 5.0F;
		Ry = (17.5 + translateP.x) / 5.0F;
		

		switch (labelbuffer1)
		{
			// siyah kalenin hareket indisleri ...
		case 1:
		{
				  int castleLx = Lx, castleLy = Ly;
				 
				 
				  for (int i = 0; i < 8; i++)
				  {
					  castleLx += RookP1x_Zy[0];
					  castleLy += RookP1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] < 13 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  //forward
					  if (label_matrix[castleLx][castleLy] > 6 && Rx == castleLx && Ry == castleLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0 && Rx == castleLx && Ry == castleLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
				  }

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {
					  castleLx += RookN1x_Zy[0];
					  castleLy += RookN1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] < 13 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  //back
					  if (label_matrix[castleLx][castleLy] == 0 && Rx == castleLx && Ry == castleLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[castleLx][castleLy] > 6 && Rx == castleLx && Ry == castleLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
					  }
				  }

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {
					  castleLx += RookZx_N1y[0];
					  castleLy += RookZx_N1y[1];
					  if (label_matrix[castleLx][castleLy] < 13 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  //left
					  if (label_matrix[castleLx][castleLy] == 0 && Rx == castleLx && Ry == castleLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[castleLx][castleLy] > 6 && Rx == castleLx && Ry == castleLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
				  }


				  castleLx = Lx;
				  castleLy = Ly; 
				  for (int i = 0; i < 8; i++)
				  {
					  castleLx += RookZx_P1y[0];
					  castleLy += RookZx_P1y[1];
					  if (label_matrix[castleLx][castleLy] < 13 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break; 
					  //right
					  if (label_matrix[castleLx][castleLy] == 0 && Rx == castleLx && Ry == castleLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[castleLx][castleLy] > 6 && Rx == castleLx && Ry == castleLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];	
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
				  }
				  break;
		}
			// siyah at hareket kýsýtlamasý
		case 2:
		{
				  int fhorseLx = Lx, fhorseLy = Ly;

				  for (int i = 0; i < 8; i++)
				  {
					  switch (i)
					  {
						  case 0:
						  {
									fhorseLx += HorseN1x_N2y[0];
									fhorseLy += HorseN1x_N2y[1];
									break;
						  }
						  case 1:
						  {
									fhorseLx += HorseN2x_N1y[0];
									fhorseLy += HorseN2x_N1y[1];
									break;
						  }
						  case 2:
						  {
									fhorseLx += HorseN2x_P1y[0];
									fhorseLy += HorseN2x_P1y[1];
									break;

						  }
						  case 3:
						  {
									fhorseLx += HorseN1x_P2y[0];
									fhorseLy += HorseN1x_P2y[1];
									break;
						  }
						  case 4:
						  {
									fhorseLx += HorseP1x_P2y[0];
									fhorseLy += HorseP1x_P2y[1];
									break;
						  }
						  case 5:
						  {
									fhorseLx += HorseP2x_P1y[0];
									fhorseLy += HorseP2x_P1y[1];
									break;
						  }
						  case 6:
						  {
									fhorseLx += HorseP2x_N1y[0];
									fhorseLy += HorseP2x_N1y[1];
									break;
						  }
						  case 7:
						  {
									fhorseLx += HorseP1x_N2y[0];
									fhorseLy += HorseP1x_N2y[1];
									break;
						  }
					  default:
						  break;
					  }

					  if (label_matrix[fhorseLx][fhorseLy] == 0 && Rx == fhorseLx && Ry == fhorseLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[fhorseLx][fhorseLy] > 6 && Rx == fhorseLx && Ry == fhorseLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  fhorseLx = Lx;
					  fhorseLy = Ly;
				  }
				  break;
		}

			// fil hareket kýsýtlamasý tamamlanacak.
		case 3:
		{	
				  int bishopLxNP = Lx, bishopLyNP = Ly,
					  bishopLxNN = Lx, bishopLyNN = Ly,
					  bishopLxPN = Lx, bishopLyPN = Ly,
					  bishopLxPP = Lx, bishopLyPP = Ly;
				  
				  for (int i = 0; i < 8; i++)
				  {
					  bishopLxNN += BishopN1x_N1y[0];
					  bishopLyNN += BishopN1x_N1y[1];

					  if (label_matrix[bishopLxNN][bishopLyNN] < 13 && label_matrix[bishopLxNN][bishopLyNN] != 0)  // Üstünden atlama engellenir...
						  break;

					  // NP
					  if (label_matrix[bishopLxNP][bishopLyNP] == 0 && Rx == bishopLxNP && Ry == bishopLyNP) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  if (label_matrix[bishopLxNP][bishopLyNP] > 6 && Rx == bishopLxNP && Ry == bishopLyNP) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
				   }
				for (int i = 0; i < 8; i++)
				{
					bishopLxNN += BishopN1x_N1y[0];
					bishopLyNN += BishopN1x_N1y[1];

					if (label_matrix[bishopLxNN][bishopLyNN] < 13 && label_matrix[bishopLxNN][bishopLyNN] != 0)  // Üstünden atlama engellenir...
						break;
					// NN
					if (label_matrix[bishopLxNN][bishopLyNN] == 0 && Rx == bishopLxNN && Ry == bishopLyNN) // boþ alan
					{
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
						labelbuffer1 = 0;
					}
					else if (label_matrix[bishopLxNN][bishopLyNN] > 6 && Rx == bishopLxNN && Ry == bishopLyNN) // rakip
					{
						labelbuffer2[ind++] = label_matrix[Rx][Ry];
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
					}
				}

				for (int i = 0; i < 8; i++)
				{
					bishopLxPN += BishopP1x_N1y[0];
					bishopLyPN += BishopP1x_N1y[1];
					if (label_matrix[bishopLxPN][bishopLyPN] < 13 && label_matrix[bishopLxPN][bishopLyPN] != 0)  // Üstünden atlama engellenir...
						break;
					// PN
					if (label_matrix[bishopLxPN][bishopLyPN] == 0 && Rx == bishopLxPN && Ry == bishopLyPN) // boþ alan
					{
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
						labelbuffer1 = 0;
					}
					else if (label_matrix[bishopLxPN][bishopLyPN] > 6 && Rx == bishopLxPN && Ry == bishopLyPN) // rakip
					{
						labelbuffer2[ind++] = label_matrix[Rx][Ry];
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
					}
				}

				for (int i = 0; i < 8; i++)
				{
					bishopLxPP += BishopP1x_P1y[0];
					bishopLyPP += BishopP1x_P1y[1];

					if (label_matrix[bishopLxPP][bishopLyPP] < 13 && label_matrix[bishopLxPP][bishopLyPP] != 0)  // Üstünden atlama engellenir...
						break;
					// PP
					if (label_matrix[bishopLxPP][bishopLyPP] == 0 && Rx == bishopLxPP && Ry == bishopLyPP) // boþ alan
					{
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
						labelbuffer1 = 0;
					}
					else if (label_matrix[bishopLxPP][bishopLyPP] > 6 && Rx == bishopLxPP && Ry == bishopLyPP) // rakip
					{
						labelbuffer2[ind++] = label_matrix[Rx][Ry];
						label_matrix[Rx][Ry] = labelbuffer1;
						label_matrix[Lx][Ly] = 0;
					}
				}
				  break;
		}


			// siyah vezir hareket kýsýtlamasý
		case 4:
		{
				  int bishopLxNP = Lx, bishopLyNP = Ly,
					  bishopLxNN = Lx, bishopLyNN = Ly,
					  bishopLxPN = Lx, bishopLyPN = Ly,
					  bishopLxPP = Lx, bishopLyPP = Ly;

		
					int fLx = Lx, fLy = Ly;
					int bLx = Lx, bLy = Ly;
					int lLx = Lx, lLy = Ly;
					int rLx = Lx, rLy = Ly;

				  for (int i = 0; i < 8; i++)
				  {
					  bishopLxNP += BishopN1x_P1y[0];
					  bishopLyNP += BishopN1x_P1y[1];

					  bishopLxNN += BishopN1x_N1y[0];
					  bishopLyNN += BishopN1x_N1y[1];

					  bishopLxPN += BishopP1x_N1y[0];
					  bishopLyPN += BishopP1x_N1y[1];

					  bishopLxPP += BishopP1x_P1y[0];
					  bishopLyPP += BishopP1x_P1y[1];
					  // NP
					  if (label_matrix[bishopLxNP][bishopLyNP] == 0 && Rx == bishopLxNP && Ry == bishopLyNP) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxNP][bishopLyNP] > 6 && Rx == bishopLxNP && Ry == bishopLyNP) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // NN
					  if (label_matrix[bishopLxNN][bishopLyNN] == 0 && Rx == bishopLxNN && Ry == bishopLyNN) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxNN][bishopLyNN] > 6 && Rx == bishopLxNN && Ry == bishopLyNN) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // PN
					  if (label_matrix[bishopLxPN][bishopLyPN] == 0 && Rx == bishopLxPN && Ry == bishopLyPN) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxPN][bishopLyPN] > 6 && Rx == bishopLxPN && Ry == bishopLyPN) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // PP
					  if (label_matrix[bishopLxPP][bishopLyPP] == 0 && Rx == bishopLxPP && Ry == bishopLyPP) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxPP][bishopLyPP] > 6 && Rx == bishopLxPP && Ry == bishopLyPP) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }


					  fLx += RookP1x_Zy[0];
					  fLy += RookP1x_Zy[1];

					  bLx += RookN1x_Zy[0];
					  bLy += RookN1x_Zy[1];

					  lLx += RookZx_N1y[0];
					  lLy += RookZx_N1y[1];

					  rLx += RookZx_P1y[0];
					  rLy += RookZx_P1y[1];
					  //forward
					  if (label_matrix[fLx][fLy] == 0 && Rx == fLx && Ry == fLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[fLx][fLy] > 6 && Rx == fLx && Ry == fLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }

					  //back
					  if (label_matrix[bLx][bLy] == 0 && Rx == bLx && Ry == bLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bLx][bLy] > 6 && Rx == bLx && Ry == bLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
					  }

					  //left
					  if (label_matrix[lLx][lLy] == 0 && Rx == lLx && Ry == lLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[lLx][lLy] > 6 && Rx == lLx && Ry == lLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }

					  //right
					  if (label_matrix[rLx][rLy] == 0 && Rx == rLx && Ry == rLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[rLx][rLy] > 6 && Rx == rLx && Ry == rLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
				  }
				  break;
		}

		// siyah þah hareket kýsýtlamalarý
		case 5:
		{
				  int bishopLxNP = Lx, bishopLyNP = Ly,
					  bishopLxNN = Lx, bishopLyNN = Ly,
					  bishopLxPN = Lx, bishopLyPN = Ly,
					  bishopLxPP = Lx, bishopLyPP = Ly;


				  int fLx = Lx, fLy = Ly,
					  bLx = Lx, bLy = Ly,
					  lLx = Lx, lLy = Ly,
					  rLx = Lx, rLy = Ly;

					  bishopLxNP += BishopN1x_P1y[0];
					  bishopLyNP += BishopN1x_P1y[1];

					  bishopLxNN += BishopN1x_N1y[0];
					  bishopLyNN += BishopN1x_N1y[1];

					  bishopLxPN += BishopP1x_N1y[0];
					  bishopLyPN += BishopP1x_N1y[1];

					  bishopLxPP += BishopP1x_P1y[0];
					  bishopLyPP += BishopP1x_P1y[1];
					  // NP
					  if (label_matrix[bishopLxNP][bishopLyNP] == 0 && Rx == bishopLxNP && Ry == bishopLyNP) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxNP][bishopLyNP] > 6 && Rx == bishopLxNP && Ry == bishopLyNP) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // NN
					  if (label_matrix[bishopLxNN][bishopLyNN] == 0 && Rx == bishopLxNN && Ry == bishopLyNN) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxNN][bishopLyNN] > 6 && Rx == bishopLxNN && Ry == bishopLyNN) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // PN
					  if (label_matrix[bishopLxPN][bishopLyPN] == 0 && Rx == bishopLxPN && Ry == bishopLyPN) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxPN][bishopLyPN] > 6 && Rx == bishopLxPN && Ry == bishopLyPN) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  // PP
					  if (label_matrix[bishopLxPP][bishopLyPP] == 0 && Rx == bishopLxPP && Ry == bishopLyPP) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bishopLxPP][bishopLyPP] > 6 && Rx == bishopLxPP && Ry == bishopLyPP) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }


					  fLx += RookP1x_Zy[0];
					  fLy += RookP1x_Zy[1];

					  bLx += RookN1x_Zy[0];
					  bLy += RookN1x_Zy[1];

					  lLx += RookZx_N1y[0];
					  lLy += RookZx_N1y[1];

					  rLx += RookZx_P1y[0];
					  rLy += RookZx_P1y[1];
					  //forward
					  if (label_matrix[fLx][fLy] == 0 && Rx == fLx && Ry == fLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[fLx][fLy] > 6 && Rx == fLx && Ry == fLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }

					  //back
					  if (label_matrix[bLx][bLy] == 0 && Rx == bLx && Ry == bLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bLx][bLy] > 6 && Rx == bLx && Ry == bLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
					  }

					  //left
					  if (label_matrix[lLx][lLy] == 0 && Rx == lLx && Ry == lLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[lLx][lLy] > 6 && Rx == lLx && Ry == lLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }

					  //right
					  if (label_matrix[rLx][rLy] == 0 && Rx == rLx && Ry == rLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[rLx][rLy] > 6 && Rx == rLx && Ry == rLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
				  

				  break;
		}

		// siyah piyon hareket kýsýtlamalarý
		case 6 :
		{
				int pawnf1Lx = Lx, pawnf1Ly = Ly, 
					pawnf2Lx = Lx, pawnf2Ly = Ly, 
					pawnLxPP = Lx, pawnLyPP = Ly, 
					pawnLxPN = Lx, pawnLyPN = Ly;


				 

				  pawnf1Lx += Pawn1forward[0];
				  pawnf1Ly += Pawn1forward[1];

				  pawnf2Lx += Pawn2forward[0];
				  pawnf2Ly += Pawn2forward[1];


				  pawnLxPP += BishopP1x_P1y[0];
				  pawnLyPP += BishopP1x_P1y[1];
				  

				  pawnLxPN += BishopP1x_N1y[0];
				  pawnLyPN += BishopP1x_N1y[1];
				  if (label_matrix[pawnf1Lx][pawnf1Ly] == 0 && Rx == pawnf1Lx && Ry == pawnf1Ly) // boþ alan varsa 1 ileri
				  {
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
					  labelbuffer1 = 0;
				  }
				  else if (label_matrix[pawnf2Lx][pawnf2Ly] == 0 && Rx == pawnf2Lx && Ry == pawnf2Ly) // boþ alan varsa 2 ileri
				  {
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
					  labelbuffer1 = 0;
				  }		  
				  else if (label_matrix[pawnLxPP][pawnLyPP] > 6 && Rx == pawnLxPP && Ry == pawnLyPP) // rakip varsa çapraz
				  {
					  labelbuffer2[ind++] = label_matrix[Rx][Ry];
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
				  }
				  else if (label_matrix[pawnLxPN][pawnLyPN] > 6 && Rx == pawnLxPN && Ry == pawnLyPN) // rakip varsa çapraz
				  {
					  labelbuffer2[ind++] = label_matrix[Rx][Ry];
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
				  }
				  break;
		}
			//white pawn
		case 7:
		{
				  int pawnb1Lx = Lx, pawnb1Ly = Ly,
					  pawnb2Lx = Lx, pawnb2Ly = Ly,
					  pawnLxNN = Lx, pawnLyNN = Ly,
					  pawnLxNP = Lx, pawnLyNP = Ly;




				  pawnb1Lx += Pawn1back[0];
				  pawnb1Ly += Pawn1back[1];

				  pawnb2Lx += Pawn2back[0];
				  pawnb2Ly += Pawn2back[1];


				  pawnLxNN += BishopN1x_N1y[0];
				  pawnLyNN += BishopN1x_N1y[1];


				  pawnLxNP += BishopN1x_P1y[0];
				  pawnLyNP += BishopN1x_P1y[1];
				  if (label_matrix[pawnb1Lx][pawnb1Ly] == 0 && Rx == pawnb1Lx && Ry == pawnb1Ly) // boþ alan varsa 1 ileri
				  {
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
					  labelbuffer1 = 0;
				  }
				  else if (label_matrix[pawnb2Lx][pawnb2Ly] == 0 && Rx == pawnb2Lx && Ry == pawnb2Ly) // boþ alan varsa 2 ileri
				  {
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
					  labelbuffer1 = 0;
				  }
				  else if (label_matrix[pawnLxNN][pawnLyNN] < 7 && label_matrix[pawnLxNP][pawnLyNP] != 0 && Rx == pawnLxNN && Ry == pawnLyNN) // rakip varsa çapraz
				  {
					  labelbuffer2[ind++] = label_matrix[Rx][Ry];
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
				  }
				  else if (label_matrix[pawnLxNP][pawnLyNP] < 7 && label_matrix[pawnLxNP][pawnLyNP] != 0 && Rx == pawnLxNP && Ry == pawnLyNP) // rakip varsa çapraz
				  {
					  labelbuffer2[ind++] = label_matrix[Rx][Ry];
					  label_matrix[Rx][Ry] = labelbuffer1;
					  label_matrix[Lx][Ly] = 0;
				  }
				  break;
		}

			// white castle hareket sýnýrlamalarý ... ( tam deðil )
		case 8:
		{
				  int fLx = Lx, fLy = Ly;
				  int bLx = Lx, bLy = Ly;
				  int lLx = Lx, lLy = Ly;
				  int rLx = Lx, rLy = Ly;

				  for (int i = 0; i < 8; i++)
				  {
					  bLx += RookP1x_Zy[0];
					  bLy += RookP1x_Zy[1];

					  fLx += RookN1x_Zy[0];
					  fLy += RookN1x_Zy[1];

					  lLx += RookZx_N1y[0];
					  lLy += RookZx_N1y[1];

					  rLx += RookZx_P1y[0];
					  rLy += RookZx_P1y[1];
					 
						 
					  //forward
					  if (label_matrix[fLx][fLy] > 6 || label_matrix[bLx][bLy] > 6 || label_matrix[lLx][lLy] > 6 || label_matrix[rLx][rLy] > 6)
						   break;
					  else if (label_matrix[fLx][fLy] == 0 && Rx == fLx && Ry == fLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[fLx][fLy] <= 6 && label_matrix[fLx][fLy] != 0 && Rx == fLx && Ry == fLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  //back
					 
					  else if (label_matrix[bLx][bLy] == 0 && Rx == bLx && Ry == bLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[bLx][bLy] <=6 && label_matrix[fLx][fLy] != 0 && Rx == bLx && Ry == bLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
					  }
					  //right
					 
					  else if (label_matrix[rLx][rLy] == 0 && Rx == rLx && Ry == rLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[rLx][rLy] <= 6 && label_matrix[fLx][fLy] != 0 && Rx == rLx && Ry == rLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  //left
				
					  else if (label_matrix[lLx][lLy] == 0 && Rx == lLx && Ry == lLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[lLx][lLy] <= 6 && label_matrix[fLx][fLy] != 0 && Rx == lLx && Ry == lLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }

				  }
				  break;
		}
		// white horse
		case 9:
		{
				  int fhorseLx = Lx, fhorseLy = Ly;

				  for (int i = 0; i < 8; i++)
				  {
					  switch (i)
					  {
					  case 0:
					  {
								fhorseLx += HorseN1x_N2y[0];
								fhorseLy += HorseN1x_N2y[1];
								break;
					  }
					  case 1:
					  {
								fhorseLx += HorseN2x_N1y[0];
								fhorseLy += HorseN2x_N1y[1];
								break;
					  }
					  case 2:
					  {
								fhorseLx += HorseN2x_P1y[0];
								fhorseLy += HorseN2x_P1y[1];
								break;

					  }
					  case 3:
					  {
								fhorseLx += HorseN1x_P2y[0];
								fhorseLy += HorseN1x_P2y[1];
								break;
					  }
					  case 4:
					  {
								fhorseLx += HorseP1x_P2y[0];
								fhorseLy += HorseP1x_P2y[1];
								break;
					  }
					  case 5:
					  {
								fhorseLx += HorseP2x_P1y[0];
								fhorseLy += HorseP2x_P1y[1];
								break;
					  }
					  case 6:
					  {
								fhorseLx += HorseP2x_N1y[0];
								fhorseLy += HorseP2x_N1y[1];
								break;
					  }
					  case 7:
					  {
								fhorseLx += HorseP1x_N2y[0];
								fhorseLy += HorseP1x_N2y[1];
								break;
					  }
					  default:
						  break;
					  }

					  if (label_matrix[fhorseLx][fhorseLy] == 0 && Rx == fhorseLx && Ry == fhorseLy) // boþ alan
					  {
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
						  labelbuffer1 = 0;
					  }
					  else if (label_matrix[fhorseLx][fhorseLy] < 7 && label_matrix[fhorseLx][fhorseLy] != 0 && Rx == fhorseLx && Ry == fhorseLy) // rakip
					  {
						  labelbuffer2[ind++] = label_matrix[Rx][Ry];
						  label_matrix[Rx][Ry] = labelbuffer1;
						  label_matrix[Lx][Ly] = 0;
					  }
					  fhorseLx = Lx;
					  fhorseLy = Ly;
				  }
				  break;
		}
			//white bishop
		case 10:
		{
				   break;
		}
			//white vizier
		case 11:
		{
				   break;
		}
			//white shah
		case 12:
		{
				   break;
		}

		default:
			break;
		}
		
	}

	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		if (turn)
			camPitch += mouseCurrState.lY * 0.001f;
		else
			camPitch -= mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();

	return;
}

void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = double(frequencyCount.QuadPart);
	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/countsPerSecond;
}

void UpdateScene(double time)
{
}

void UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	At = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	At = XMVector3Normalize(At);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	Up = XMVector3TransformCoord(Up, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	Eye += moveLeftRight * camRight;
	Eye += moveBackForward * camForward;	

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	At = Eye + At;	

	crosshairPosition = Eye + 0.5 * XMVector3Normalize(At - Eye); 

	g_View = XMMatrixLookAtLH( Eye, At, Up );

}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"D3D11_Chess.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"D3D11_Chess.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )	return hr;

    // Compile the pixel shader Textured
    ID3DBlob* pPStextureBlob = NULL;
    hr = CompileShaderFromFile( L"D3D11_Chess.fx", "PS_Textured", "ps_4_0", &pPStextureBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPStextureBlob->GetBufferPointer(), pPStextureBlob->GetBufferSize(), NULL, &g_pPixelShader_Textured );
    pPStextureBlob->Release();
    if( FAILED( hr ) )	return hr;


    // Compile the pixel shader Solid
    ID3DBlob* pPSsolidBlob = NULL;
    hr = CompileShaderFromFile( L"D3D11_Chess.fx", "PS_Solid", "ps_4_0", &pPSsolidBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSsolidBlob->GetBufferPointer(), pPSsolidBlob->GetBufferSize(), NULL, &g_pPixelShader_Solid );
    pPSsolidBlob->Release();
    if( FAILED( hr ) )	return hr;

	ifstream fin;

	// load the chessboard
	fin.open("chessBoard.txt");
	fin >> vertexCountchessBoard;
	verticeschessBoard = new SimpleVertex[vertexCountchessBoard];

	
	for (int i = 0; i<vertexCountchessBoard; i++)
	{
		fin >> verticeschessBoard[i].Pos.x >> verticeschessBoard[i].Pos.y >> verticeschessBoard[i].Pos.z;
		fin >> verticeschessBoard[i].Tex.x >> verticeschessBoard[i].Tex.y;
		fin >> verticeschessBoard[i].Normal.x >> verticeschessBoard[i].Normal.y >> verticeschessBoard[i].Normal.z;
	}	
	fin.close();

	// Buradaki genel setlemelerin bazýlarý aþaðýdaki diðer setlemelerde kullanýlmayabilir.
	// Diðer setlemeler burdaki gibi uzun deðil, mesela piyon için bd.ByteWidth... ile  if(FAILED(hr))... arasý yani sadece 5 satýr. 
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountchessBoard;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticeschessBoard;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_chessBoard);
	if (FAILED(hr))     return hr;
	/////////////////

	// load the pawn
	fin.open("piyon.txt");
	fin >> vertexCountpiyon;
	verticespiyon = new SimpleVertex[vertexCountpiyon];

	for(int i=0; i<vertexCountpiyon; i++)
	{
		fin >> verticespiyon[i].Pos.x >> verticespiyon[i].Pos.y >> verticespiyon[i].Pos.z;
		fin >> verticespiyon[i].Tex.x >> verticespiyon[i].Tex.y;
		fin >> verticespiyon[i].Normal.x >> verticespiyon[i].Normal.y >> verticespiyon[i].Normal.z;
	}	
	fin.close();

    bd.ByteWidth = sizeof( SimpleVertex ) * vertexCountpiyon;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = verticespiyon;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer_piyon );
    if( FAILED( hr ) )     return hr;
	/////////////////////////////////////////////////////////////////////////
	
	// load the castle
	fin.open("kale.txt");
	fin >> vertexCountkale;
	verticeskale = new SimpleVertex[vertexCountkale];

	for (int i = 0; i<vertexCountkale; i++)
	{
		fin >> verticeskale[i].Pos.x >> verticeskale[i].Pos.y >> verticeskale[i].Pos.z;
		fin >> verticeskale[i].Tex.x >> verticeskale[i].Tex.y;
		fin >> verticeskale[i].Normal.x >> verticeskale[i].Normal.y >> verticeskale[i].Normal.z;
	}
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountkale;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticeskale;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_kale);
	if (FAILED(hr))     return hr;
	//////////////////////////////////////////////////////////////////////////
	
	// load the horse
	fin.open("at.txt");
	fin >> vertexCountat;
	verticesat = new SimpleVertex[vertexCountat];

	for (int i = 0; i<vertexCountat; i++)
	{
		fin >> verticesat[i].Pos.x >> verticesat[i].Pos.y >> verticesat[i].Pos.z;
		fin >> verticesat[i].Tex.x >> verticesat[i].Tex.y;
		fin >> verticesat[i].Normal.x >> verticesat[i].Normal.y >> verticesat[i].Normal.z;
	}
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountat;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesat;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_at);
	if (FAILED(hr))     return hr;
	//////////////////////////////////////////////////////////////////////////////

	// load the elephant
	fin.open("fil.txt");
	fin >> vertexCountfil;
	verticesfil = new SimpleVertex[vertexCountfil];

	for (int i = 0; i<vertexCountfil; i++)
	{
		fin >> verticesfil[i].Pos.x >> verticesfil[i].Pos.y >> verticesfil[i].Pos.z;
		fin >> verticesfil[i].Tex.x >> verticesfil[i].Tex.y;
		fin >> verticesfil[i].Normal.x >> verticesfil[i].Normal.y >> verticesfil[i].Normal.z;
	}
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountfil;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesfil;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_fil);
	if (FAILED(hr))     return hr;
	/////////////////////////////////////////////////////////////////////////////
	
	// load the vizier
	fin.open("vezir.txt");
	fin >> vertexCountvezir;
	verticesvezir = new SimpleVertex[vertexCountvezir];

	for (int i = 0; i<vertexCountvezir; i++)
	{
		fin >> verticesvezir[i].Pos.x >> verticesvezir[i].Pos.y >> verticesvezir[i].Pos.z;
		fin >> verticesvezir[i].Tex.x >> verticesvezir[i].Tex.y;
		fin >> verticesvezir[i].Normal.x >> verticesvezir[i].Normal.y >> verticesvezir[i].Normal.z;
	}
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountvezir;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesvezir;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_vezir);
	if (FAILED(hr))     return hr;
	/////////////////////////////////////////////////////////////////////////////

	// load the shah
	fin.open("sah.txt");
	fin >> vertexCountsah;
	verticessah = new SimpleVertex[vertexCountsah];

	for (int i = 0; i<vertexCountsah; i++)
	{
		fin >> verticessah[i].Pos.x >> verticessah[i].Pos.y >> verticessah[i].Pos.z;
		fin >> verticessah[i].Tex.x >> verticessah[i].Tex.y;
		fin >> verticessah[i].Normal.x >> verticessah[i].Normal.y >> verticessah[i].Normal.z;
	}
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountsah;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticessah;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_sah);
	if (FAILED(hr))     return hr;
	/////////////////////////////////////////////////////////////////////////////

	fin.open("Light.txt");
	int vertexCountlight;
	fin >> vertexCountlight;
	verticesLight = new SimpleVertex[vertexCountlight];

	for (int i = 0; i<vertexCountlight; i++)
	{
		fin >> verticesLight[i].Pos.x >> verticesLight[i].Pos.y >> verticesLight[i].Pos.z;
		fin >> verticesLight[i].Tex.x >> verticesLight[i].Tex.y;
		fin >> verticesLight[i].Normal.x >> verticesLight[i].Normal.y >> verticesLight[i].Normal.z;
	}

	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountlight;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesLight;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Light);
	if (FAILED(hr))     return hr;


	fin.open("pointer.txt");

	fin >> vertexCountpointer;
	verticespointer = new SimpleVertex[vertexCountpointer];

	for (int i = 0; i<vertexCountpointer; i++)
	{
		fin >> verticespointer[i].Pos.x >> verticespointer[i].Pos.y >> verticespointer[i].Pos.z;
		fin >> verticespointer[i].Tex.x >> verticespointer[i].Tex.y;
		fin >> verticespointer[i].Normal.x >> verticespointer[i].Normal.y >> verticespointer[i].Normal.z;
	}

	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex) * vertexCountpointer;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticespointer;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Pointer);
	if (FAILED(hr))     return hr;



	fin.open("CrossHair.txt");

	fin >> vertexCountcrosshair;
	verticesCrossHair = new SimpleVertex[vertexCountcrosshair];

	for (int i = 0; i<vertexCountcrosshair; i++)
	{
		fin >> verticesCrossHair[i].Pos.x >> verticesCrossHair[i].Pos.y >> verticesCrossHair[i].Pos.z;
		fin >> verticesCrossHair[i].Tex.x >> verticesCrossHair[i].Tex.y;
		fin >> verticesCrossHair[i].Normal.x >> verticesCrossHair[i].Normal.y >> verticesCrossHair[i].Normal.z;
	}
	
	fin.close();

	bd.ByteWidth = sizeof(SimpleVertex) * vertexCountcrosshair;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = verticesCrossHair;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer_CrossHair );
    if( FAILED( hr ) )     return hr;

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

 
	// Create the constant buffers
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pConstantBuffer );
    if( FAILED( hr ) )	return hr;


	// Load the Texture pointer White
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"pointer_White.jpg", NULL, NULL, &g_pTexturepointerWhite, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture pointer Black
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"pointer_Black.jpg", NULL, NULL, &g_pTexturepointerBlack, NULL);
	if (FAILED(hr))	return hr;
	
	// Load The Texture Danger White Pointer
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"white_danger_ptr.jpg", NULL, NULL, &g_pTexturedangerWhiteptr, NULL);
	if (FAILED(hr))	return hr;

	// Load The Texture Danger Black Pointer
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"black_danger_ptr.jpg", NULL, NULL, &g_pTexturedangerBlackptr, NULL);
	if (FAILED(hr))	return hr;
	
	// Load the Texture select_black_ptr  
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"select_black_ptr.jpg", NULL, NULL, &g_pTextureselectblackptr, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture select_white_ptr
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"select_white_ptr.jpg", NULL, NULL, &g_pTextureselectwhiteptr, NULL);
	if (FAILED(hr))	return hr;

    // Load the Texture chessBoard
    hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, L"chessBoard.jpg", NULL, NULL, &g_pTexturechessBoard, NULL );
    if( FAILED( hr ) )	return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) )	return hr;

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    Eye = XMVectorSet( 0.0f, 15.0f, -60.0f, 0.0f );
    At = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
    Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f );
    
    cBuffer.mProjection = XMMatrixTranspose( g_Projection );
    g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );


	Ro = Eye;
	Rd = XMVector3Normalize(At - Eye);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext )		g_pImmediateContext->ClearState();
    if( g_pSamplerLinear )			g_pSamplerLinear->Release();
    if( g_pTexturepiyon )			g_pTexturepiyon->Release();
	if (g_pTexturekale)				g_pTexturekale->Release();
	if (g_pTextureat)				g_pTextureat->Release();
	if (g_pTexturefil)				g_pTexturefil->Release();
	if (g_pTexturevezir)			g_pTexturevezir->Release();
	if (g_pTexturesah)				g_pTexturesah->Release();
    if( g_pTexturechessBoard )		g_pTexturechessBoard->Release();
    if( g_pConstantBuffer )			g_pConstantBuffer->Release();
	if( g_pVertexBuffer_CrossHair ) g_pVertexBuffer_CrossHair->Release();
	if( g_pVertexBuffer_piyon )	g_pVertexBuffer_piyon->Release();
	if( g_pVertexBuffer_Light )		g_pVertexBuffer_Light->Release();
    if( g_pIndexBuffer )			g_pIndexBuffer->Release();
    if( g_pVertexLayout )			g_pVertexLayout->Release();
    if( g_pVertexShader )			g_pVertexShader->Release();
    if( g_pPixelShader )			g_pPixelShader->Release();
	if( g_pPixelShader )			g_pPixelShader_Solid->Release();
    if( g_pDepthStencil )			g_pDepthStencil->Release();
    if( g_pDepthStencilView )		g_pDepthStencilView->Release();
    if( g_pRenderTargetView )		g_pRenderTargetView->Release();
    if( g_pSwapChain )				g_pSwapChain->Release();
    if( g_pImmediateContext )		g_pImmediateContext->Release();
    if( g_pd3dDevice )				g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}
void yellow_red_point(int Lx, int Ly, UINT stride, UINT offset)
{
		// render yellow and red points
	switch (label_matrix[Lx][Ly])
	{

		/* BLACK CASTLE */

		case 1: 
		{
				// FORWARD

				int castleLx = Lx, castleLy = Ly;
				for (int i = 0; i < 8; i++) 
				{
					 
					castleLx += RookP1x_Zy[0];
					castleLy += RookP1x_Zy[1];
					if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[castleLx][castleLy] > 6 && castleLx < 8)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[castleLx][castleLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[castleLx][castleLy] == 0)
					{
						// sarý kareye boya ...
						if (castleLx>7) // tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[castleLx][castleLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				} 

				// BACK

				castleLx = Lx;
				castleLy = Ly;
				for (int i = 0; i < 8; i++) 
				{
						
					castleLx += RookN1x_Zy[0];
					castleLy += RookN1x_Zy[1];
					if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünten atlamayý engeller...
							break;
					else if (label_matrix[castleLx][castleLy] > 6 && castleLx > 0)
						{
							// kýrmýzý kareye boya ...
							if (Black_or_White[castleLx][castleLy] == 1)
								g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
							else
								g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

							XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
							cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
							g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
							cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
							g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
							g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
							g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
							g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
							g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
							g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
							g_pImmediateContext->Draw(vertexCountpointer, 0);
							break;
						}
						else if (label_matrix[castleLx][castleLy] == 0)
						{
							if (castleLx < 0)		// tahta dýþdaki alana point koyma engellenir...
								break;

							// sarý kareye boya ...
							if (Black_or_White[castleLx][castleLy] != 0)
								g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
							else
								g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

							XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
							cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
							g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
							cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
							g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
							g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
							g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
							g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
							g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
							g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
							g_pImmediateContext->Draw(vertexCountpointer, 0);
							
						}
				}

				// LEFT

				castleLx = Lx;
				castleLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					castleLx += RookZx_N1y[0];
					castleLy += RookZx_N1y[1];
					if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[castleLx][castleLy] > 6 && castleLy > 0)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[castleLx][castleLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[castleLx][castleLy] == 0)
					{
						// sarý kareye boya ...
						if (castleLy < 0)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[castleLx][castleLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				// RÝGHT

				castleLx = Lx;
				castleLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					castleLx += RookZx_P1y[0];
					castleLy += RookZx_P1y[1];
					if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[castleLx][castleLy] > 6 && castleLy < 8)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[castleLx][castleLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[castleLx][castleLy] == 0)
					{
						// sarý kareye boya ...
						if (castleLy > 7)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[castleLx][castleLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}
					break;
			}

		/* BLACK HORSE */

		case 2: 
		{
				int fhorseLx = Lx, fhorseLy = Ly;

				for (int i = 0; i < 8; i++)
				{
					switch (i)
					{
					case 0:
						{
							fhorseLx += HorseN1x_N2y[0];
							fhorseLy += HorseN1x_N2y[1];
							break;
						}
					case 1:
						{
							fhorseLx += HorseN2x_N1y[0];
							fhorseLy += HorseN2x_N1y[1];
							break;
						}
					case 2:
						{
							fhorseLx += HorseN2x_P1y[0];
							fhorseLy += HorseN2x_P1y[1];
							break;

						}
					case 3:
						{
							fhorseLx += HorseN1x_P2y[0];
							fhorseLy += HorseN1x_P2y[1];
							break;
						}
					case 4:
						{
							fhorseLx += HorseP1x_P2y[0];
							fhorseLy += HorseP1x_P2y[1];
							break;
						}
					case 5:
						{
							fhorseLx += HorseP2x_P1y[0];
							fhorseLy += HorseP2x_P1y[1];
							break;
						}
					case 6:
						{
							fhorseLx += HorseP2x_N1y[0];
							fhorseLy += HorseP2x_N1y[1];
							break;
						}
					case 7:
						{
							fhorseLx += HorseP1x_N2y[0];
							fhorseLy += HorseP1x_N2y[1];
							break;
						}
					default:
						break;
					}
					  
					if (fhorseLx < 0 || fhorseLy < 0 || fhorseLx > 7 || fhorseLy > 7)		// tahta dýþdaki alana point koyma engellenir...
					{
						fhorseLx = Lx;
						fhorseLy = Ly;
						continue;
					}

					if (label_matrix[fhorseLx][fhorseLy] > 6)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[fhorseLx][fhorseLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * fhorseLy), 0, (17.5F - 5 * fhorseLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						  
					}
					else if (label_matrix[fhorseLx][fhorseLy] == 0)
					{
						// sarý kareye boya ...
						 
						if (Black_or_White[fhorseLx][fhorseLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * fhorseLy), 0, (17.5F - 5 * fhorseLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
					fhorseLx = Lx;
					fhorseLy = Ly;
				}
				break;
		}

		/* BLACK BÝSHOP */
		
		case 3:
		{
				  /* BÝSHOP +X +Y */

				int bishopLx = Lx, bishopLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					bishopLx += BishopP1x_P1y[0];
					bishopLy += BishopP1x_P1y[1];
					if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy < 8)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[bishopLx][bishopLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[bishopLx][bishopLy] == 0)
					{
						// sarý kareye boya ...
						if (bishopLy > 7 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[bishopLx][bishopLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				/* BISHOP +X -Y */

				bishopLx = Lx;
				bishopLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					bishopLx += BishopP1x_N1y[0];
					bishopLy += BishopP1x_N1y[1];
					if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy >= 0)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[bishopLx][bishopLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[bishopLx][bishopLy] == 0)
					{
						// sarý kareye boya ...
						if (bishopLy < 0 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[bishopLx][bishopLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				/* BISHOP -X -Y */

				bishopLx = Lx;
				bishopLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					bishopLx += BishopN1x_N1y[0];
					bishopLy += BishopN1x_N1y[1];
					if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // siyahlar için üstünden atlama engellenir...
						break;
					else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy >= 0)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[bishopLx][bishopLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[bishopLx][bishopLy] == 0)
					{
						// sarý kareye boya ...
						if (bishopLy < 0 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[bishopLx][bishopLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				/* BISHOP -X +Y */

				bishopLx = Lx;
				bishopLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					bishopLx += BishopN1x_P1y[0];
					bishopLy += BishopN1x_P1y[1];
					if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy < 8)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[bishopLx][bishopLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[bishopLx][bishopLy] == 0)
					{
						// sarý kareye boya ...
						if (bishopLy > 7 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[bishopLx][bishopLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				break;
		}
		/* BLACK VIZIER */

		case 4:
		{
				  //	Düz
				  // FORWARD

				  int castleLx = Lx, castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  castleLx += RookP1x_Zy[0];
					  castleLy += RookP1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[castleLx][castleLy] > 6 && castleLx < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (castleLx > 7) // tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  // BACK

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  castleLx += RookN1x_Zy[0];
					  castleLy += RookN1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünten atlamayý engeller...
						  break;
					  else if (label_matrix[castleLx][castleLy] > 6 && castleLx > 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  if (castleLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							  break;

						  // sarý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);

					  }
				  }

				  // LEFT

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  castleLx += RookZx_N1y[0];
					  castleLy += RookZx_N1y[1];
					  if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[castleLx][castleLy] > 6 && castleLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (castleLy < 0)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  // RÝGHT

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  castleLx += RookZx_P1y[0];
					  castleLy += RookZx_P1y[1];
					  if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[castleLx][castleLy] > 6 && castleLy < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (castleLy > 7)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  //	Çapraz
				  int bishopLx = Lx, bishopLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  bishopLx += BishopP1x_P1y[0];
					  bishopLy += BishopP1x_P1y[1];
					  if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (bishopLy > 7 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  /* BISHOP +X -Y */

				  bishopLx = Lx;
				  bishopLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  bishopLx += BishopP1x_N1y[0];
					  bishopLy += BishopP1x_N1y[1];
					  if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (bishopLy < 0 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  /* BISHOP -X -Y */

				  bishopLx = Lx;
				  bishopLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  bishopLx += BishopN1x_N1y[0];
					  bishopLy += BishopN1x_N1y[1];
					  if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // siyahlar için üstünden atlama engellenir...
						  break;
					  else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (bishopLy < 0 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  /* BISHOP -X +Y */

				  bishopLx = Lx;
				  bishopLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  bishopLx += BishopN1x_P1y[0];
					  bishopLy += BishopN1x_P1y[1];
					  if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (bishopLy > 7 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }
				  break;
		}

		/* BLACK SHAH */

		case 5:
		{
				  //	Düz
				  // FORWARD
				  int castleLx = Lx, castleLy = Ly;

					  castleLx += RookP1x_Zy[0];
					  castleLy += RookP1x_Zy[1];
					 
					   if (label_matrix[castleLx][castleLy] > 6 && castleLx < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  
					  }
					  else if (label_matrix[castleLx][castleLy] == 0 && castleLx < 8)
					  {
						  // sarý kareye boya ...

						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  // BACK

				  castleLx = Lx;
				  castleLy = Ly;
					  castleLx += RookN1x_Zy[0];
					  castleLy += RookN1x_Zy[1];
					  
					   if (label_matrix[castleLx][castleLy] > 6 && castleLx > 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						 
					  }
					   else if (label_matrix[castleLx][castleLy] == 0 && castleLx > 0)
					  {
						  // sarý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);

					  }
				  

				  // LEFT

				  castleLx = Lx;
				  castleLy = Ly;
				  castleLx += RookZx_N1y[0];
				  castleLy += RookZx_N1y[1];
					  
					  if (label_matrix[castleLx][castleLy] > 6 && castleLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  
					  }
					  else if (label_matrix[castleLx][castleLy] == 0 && castleLy >= 0)
					  {
						  // sarý kareye boya ...
			
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  // RÝGHT

				  castleLx = Lx;
				  castleLy = Ly;
					  castleLx += RookZx_P1y[0];
					  castleLy += RookZx_P1y[1];
					 
					  if (label_matrix[castleLx][castleLy] > 6 && castleLy < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						 
					  }
					  else if (label_matrix[castleLx][castleLy] == 0 && castleLy < 8)
					  {
						  // sarý kareye boya ...
				
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  //	Çapraz
				  /* BISHOP +X +Y */

				  int bishopLx = Lx, bishopLy = Ly;
					  bishopLx += BishopP1x_P1y[0];
					  bishopLy += BishopP1x_P1y[1];
					  if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy < 8)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						 
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx < 8 && bishopLy < 8)
					  {
						  // sarý kareye boya ...
		
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  /* BISHOP +X -Y */

				  bishopLx = Lx;
				  bishopLy = Ly;
					  bishopLx += BishopP1x_N1y[0];
					  bishopLy += BishopP1x_N1y[1];
					  if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx < 8 && bishopLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx < 8 && bishopLy >= 0)
					  {
						  // sarý kareye boya ...
				
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  /* BISHOP -X -Y */

				  bishopLx = Lx;
				  bishopLy = Ly;
					  bishopLx += BishopN1x_N1y[0];
					  bishopLy += BishopN1x_N1y[1];
					  if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy >= 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[bishopLx][bishopLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						
					  }
					  else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLy >= 0 && bishopLx >= 0)
					  {
						  // sarý kareye boya ...
						  
						  if (Black_or_White[bishopLx][bishopLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  

				  /* BISHOP -X +Y */

					bishopLx = Lx;
					bishopLy = Ly;
					bishopLx += BishopN1x_P1y[0];
					bishopLy += BishopN1x_P1y[1];
					if (label_matrix[bishopLx][bishopLy] > 6 && bishopLx >= 0 && bishopLy < 8)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[bishopLx][bishopLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						
					}
					else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx >= 0 && bishopLy < 8)
					{
						// sarý kareye boya ...
						
						if (Black_or_White[bishopLx][bishopLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				 

				  break;
		}

		/* BLACK PAWN */

		case 6:
		{
				// black pawn
				  int pawnLx = Lx, pawnLy = Ly;

				  // 1 forward

				  pawnLx += Pawn1forward[0];
				  pawnLy += Pawn1forward[1];
				  if (label_matrix[pawnLx][pawnLy] == 0 && pawnLx < 8)
				  {
					  // sarý kareye boya ...

					  if (Black_or_White[pawnLx][pawnLy] != 0)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }

				   // 2 forward ( ilk çýkýþ )
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += Pawn2forward[0];
				  pawnLy += Pawn2forward[1];
				  if (label_matrix[pawnLx][pawnLy] == 0 && pawnLx == 3 && label_matrix[pawnLx-1][pawnLy] == 0)
				  {
					  // sarý kareye boya ...

					  if (Black_or_White[pawnLx][pawnLy] != 0)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }

				  // çapraz yeme
				  // +X +Y
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += BishopP1x_P1y[0];
				  pawnLy += BishopP1x_P1y[1];
				  if (label_matrix[pawnLx][pawnLy] > 6 && pawnLx < 8 && pawnLy < 8)
				  {
					  // kýrmýzý kareye boya ...
					  if (Black_or_White[pawnLx][pawnLy] == 1)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }
				 // çapraz 
				 // +X -Y
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += BishopP1x_N1y[0];
				  pawnLy += BishopP1x_N1y[1];
				  if (label_matrix[pawnLx][pawnLy] > 6 && pawnLy >= 0 )
				  {
					  // kýrmýzý kareye boya ...
					  if (Black_or_White[pawnLx][pawnLy] == 1)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }
				  break;

		}
		
		/* WHITE PAWN */

		case 7:
		{
				  int pawnLx = Lx, pawnLy = Ly;

				  // 1 forward

				  pawnLx += Pawn1back[0];
				  pawnLy += Pawn1back[1];
				  if (label_matrix[pawnLx][pawnLy] == 0 && pawnLx > 0)
				  {
					  // sarý kareye boya ...

					  if (Black_or_White[pawnLx][pawnLy] != 0)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }

				  // 2 forward ( ilk çýkýþ )
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += Pawn2back[0];
				  pawnLy += Pawn2back[1];
				  if (label_matrix[pawnLx][pawnLy] == 0 && pawnLx == 4 && label_matrix[pawnLx + 1][pawnLy] == 0)
				  {
					  // sarý kareye boya ...

					  if (Black_or_White[pawnLx][pawnLy] != 0)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }

				  // çapraz yeme
				  // -X -Y
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += BishopN1x_N1y[0];
				  pawnLy += BishopN1x_N1y[1];
				  if (label_matrix[pawnLx][pawnLy] < 7 && label_matrix[pawnLx][pawnLy] != 0 && pawnLy >= 0 && pawnLx >= 0)
				  {
					  // kýrmýzý kareye boya ...
					  if (Black_or_White[pawnLx][pawnLy] == 1)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }
				  // çapraz 
				  // -X +Y
				  pawnLx = Lx;
				  pawnLy = Ly;
				  pawnLx += BishopN1x_P1y[0];
				  pawnLy += BishopN1x_P1y[1];
				  if (label_matrix[pawnLx][pawnLy] < 7 && label_matrix[pawnLx][pawnLy] != 0 && pawnLx >=0)
				  {
					  // kýrmýzý kareye boya ...
					  if (Black_or_White[pawnLx][pawnLy] == 1)
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					  else
						  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * pawnLy), 0, (17.5F - 5 * pawnLx));
					  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					  g_pImmediateContext->Draw(vertexCountpointer, 0);
				  }

				  break;
		}
		
		/* WHITE CASTLE */

		case 8:
		{
				  int castleLx = Lx, castleLy = Ly;
				  
				  // FORWARD
				  for (int i = 0; i < 8; i++)
				  {
					  castleLx += RookN1x_Zy[0];
					  castleLy += RookN1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] > 6)  // Üstünten atlamayý engeller...
						  break;
					  else if (label_matrix[castleLx][castleLy] < 7 && castleLx >= 0 && label_matrix[castleLx][castleLy] != 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  if (castleLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							  break;

						  // sarý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);

					  }
				  }

				  // BACK

				  castleLx = Lx;
				  castleLy = Ly;
				  for (int i = 0; i < 8; i++)
				  {

					  castleLx += RookP1x_Zy[0];
					  castleLy += RookP1x_Zy[1];
					  if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						  break;
					  else if (label_matrix[castleLx][castleLy] < 7 && castleLx < 8 && label_matrix[castleLx][castleLy] != 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[castleLx][castleLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
						  break;
					  }
					  else if (label_matrix[castleLx][castleLy] == 0)
					  {
						  // sarý kareye boya ...
						  if (castleLx > 7) // tahta dýþdaki alana point koyma engellenir...
							  break;
						  if (Black_or_White[castleLx][castleLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
				  }

				  // LEFT

				castleLx = Lx;
				castleLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					castleLx += RookZx_N1y[0];
					castleLy += RookZx_N1y[1];
					if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[castleLx][castleLy] < 7 && castleLy >= 0 && label_matrix[castleLx][castleLy] !=0)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[castleLx][castleLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[castleLx][castleLy] == 0)
					{
						// sarý kareye boya ...
						if (castleLy < 0)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[castleLx][castleLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}

				// RÝGHT

				castleLx = Lx;
				castleLy = Ly;
				for (int i = 0; i < 8; i++)
				{

					castleLx += RookZx_P1y[0];
					castleLy += RookZx_P1y[1];
					if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						break;
					else if (label_matrix[castleLx][castleLy] < 7 && castleLy < 8 && label_matrix[castleLx][castleLy] != 0)
					{
						// kýrmýzý kareye boya ...
						if (Black_or_White[castleLx][castleLy] == 1)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
						break;
					}
					else if (label_matrix[castleLx][castleLy] == 0)
					{
						// sarý kareye boya ...
						if (castleLy > 7)		// tahta dýþdaki alana point koyma engellenir...
							break;
						if (Black_or_White[castleLx][castleLy] != 0)
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						else
							g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						g_pImmediateContext->Draw(vertexCountpointer, 0);
					}
				}


				  break;
		}

		/* WHITE HORSE */

		case 9:
		{
				  int fhorseLx = Lx, fhorseLy = Ly;

				  for (int i = 0; i < 8; i++)
				  {
					  switch (i)
					  {
					  case 0:
					  {
								fhorseLx += HorseN1x_N2y[0];
								fhorseLy += HorseN1x_N2y[1];
								break;
					  }
					  case 1:
					  {
								fhorseLx += HorseN2x_N1y[0];
								fhorseLy += HorseN2x_N1y[1];
								break;
					  }
					  case 2:
					  {
								fhorseLx += HorseN2x_P1y[0];
								fhorseLy += HorseN2x_P1y[1];
								break;

					  }
					  case 3:
					  {
								fhorseLx += HorseN1x_P2y[0];
								fhorseLy += HorseN1x_P2y[1];
								break;
					  }
					  case 4:
					  {
								fhorseLx += HorseP1x_P2y[0];
								fhorseLy += HorseP1x_P2y[1];
								break;
					  }
					  case 5:
					  {
								fhorseLx += HorseP2x_P1y[0];
								fhorseLy += HorseP2x_P1y[1];
								break;
					  }
					  case 6:
					  {
								fhorseLx += HorseP2x_N1y[0];
								fhorseLy += HorseP2x_N1y[1];
								break;
					  }
					  case 7:
					  {
								fhorseLx += HorseP1x_N2y[0];
								fhorseLy += HorseP1x_N2y[1];
								break;
					  }
					  default:
						  break;
					  }

					  if (fhorseLx < 0 || fhorseLy < 0 || fhorseLx > 7 || fhorseLy > 7)		// tahta dýþdaki alana point koyma engellenir...
					  {
						  fhorseLx = Lx;
						  fhorseLy = Ly;
						  continue;
					  }

					  if (label_matrix[fhorseLx][fhorseLy] < 7 && label_matrix[fhorseLx][fhorseLy] != 0)
					  {
						  // kýrmýzý kareye boya ...
						  if (Black_or_White[fhorseLx][fhorseLy] == 1)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * fhorseLy), 0, (17.5F - 5 * fhorseLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);

					  }
					  else if (label_matrix[fhorseLx][fhorseLy] == 0)
					  {
						  // sarý kareye boya ...

						  if (Black_or_White[fhorseLx][fhorseLy] != 0)
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						  else
							  g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						  XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * fhorseLy), 0, (17.5F - 5 * fhorseLx));
						  cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						  g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						  g_pImmediateContext->Draw(vertexCountpointer, 0);
					  }
					  fhorseLx = Lx;
					  fhorseLy = Ly;
				  }

				  break;
		}

		/* WHITE BÝSHOP */

		case 10:
		{
				   /* BÝSHOP +X +Y */

				   int bishopLx = Lx, bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopP1x_P1y[0];
					   bishopLy += BishopP1x_P1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx < 8 && bishopLy < 8 && label_matrix[bishopLx][bishopLy]!=0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy > 7 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP +X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopP1x_N1y[0];
					   bishopLy += BishopP1x_N1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx < 8 && bishopLy >= 0 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy < 0 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP -X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopN1x_N1y[0];
					   bishopLy += BishopN1x_N1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // BEYAZLAR için üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx >= 0 && bishopLy >= 0 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy < 0 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP -X +Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopN1x_P1y[0];
					   bishopLy += BishopN1x_P1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  //  BEYAZLAR için üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx >= 0 && bishopLy < 8 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy > 7 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   break;
		}
		/* WHITE VIZIER */

		case 11:
		{
				   
				   // FORWARD
				   int castleLx = Lx, castleLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {
					   castleLx += RookN1x_Zy[0];
					   castleLy += RookN1x_Zy[1];
					   if (label_matrix[castleLx][castleLy] > 6)  // Üstünten atlamayý engeller...
						   break;
					   else if (label_matrix[castleLx][castleLy] < 7 && castleLx >= 0 && label_matrix[castleLx][castleLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[castleLx][castleLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[castleLx][castleLy] == 0)
					   {
						   if (castleLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;

						   // sarý kareye boya ...
						   if (Black_or_White[castleLx][castleLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);

					   }
				   }

				   // BACK

				   castleLx = Lx;
				   castleLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   castleLx += RookP1x_Zy[0];
					   castleLy += RookP1x_Zy[1];
					   if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[castleLx][castleLy] < 7 && castleLx < 8 && label_matrix[castleLx][castleLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[castleLx][castleLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[castleLx][castleLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (castleLx > 7) // tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[castleLx][castleLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   // LEFT

				   castleLx = Lx;
				   castleLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   castleLx += RookZx_N1y[0];
					   castleLy += RookZx_N1y[1];
					   if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[castleLx][castleLy] < 7 && castleLy >= 0 && label_matrix[castleLx][castleLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[castleLx][castleLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[castleLx][castleLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (castleLy < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[castleLx][castleLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   // RÝGHT

				   castleLx = Lx;
				   castleLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   castleLx += RookZx_P1y[0];
					   castleLy += RookZx_P1y[1];
					   if (label_matrix[castleLx][castleLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[castleLx][castleLy] < 7 && castleLy < 8 && label_matrix[castleLx][castleLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[castleLx][castleLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[castleLx][castleLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (castleLy > 7)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[castleLx][castleLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   // CROSS

				   /* BÝSHOP +X +Y */

				   int bishopLx = Lx, bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopP1x_P1y[0];
					   bishopLy += BishopP1x_P1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx < 8 && bishopLy < 8 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy > 7 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP +X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopP1x_N1y[0];
					   bishopLy += BishopP1x_N1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // Üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx < 8 && bishopLy >= 0 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy < 0 || bishopLx > 7)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP -X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopN1x_N1y[0];
					   bishopLy += BishopN1x_N1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  // BEYAZLAR için üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx >= 0 && bishopLy >= 0 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy < 0 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }

				   /* BISHOP -X +Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   for (int i = 0; i < 8; i++)
				   {

					   bishopLx += BishopN1x_P1y[0];
					   bishopLy += BishopN1x_P1y[1];
					   if (label_matrix[bishopLx][bishopLy] > 6)  //  BEYAZLAR için üstünden atlama engellenir...
						   break;
					   else if (label_matrix[bishopLx][bishopLy] < 7 && bishopLx >= 0 && bishopLy < 8 && label_matrix[bishopLx][bishopLy] != 0)
					   {
						   // kýrmýzý kareye boya ...
						   if (Black_or_White[bishopLx][bishopLy] == 1)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
						   break;
					   }
					   else if (label_matrix[bishopLx][bishopLy] == 0)
					   {
						   // sarý kareye boya ...
						   if (bishopLy > 7 || bishopLx < 0)		// tahta dýþdaki alana point koyma engellenir...
							   break;
						   if (Black_or_White[bishopLx][bishopLy] != 0)
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
						   else
							   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

						   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
						   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
						   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
						   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
						   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
						   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
						   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
						   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
						   g_pImmediateContext->Draw(vertexCountpointer, 0);
					   }
				   }
				   break;
		}

		/* WHITE SHAH */

		case 12:
		{
				   int castleLx = Lx, castleLy = Ly;

				   castleLx += RookP1x_Zy[0];
				   castleLy += RookP1x_Zy[1];

				   if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0 && castleLx < 8)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[castleLx][castleLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[castleLx][castleLy] == 0 && castleLx < 8)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[castleLx][castleLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   // BACK

				   castleLx = Lx;
				   castleLy = Ly;
				   castleLx += RookN1x_Zy[0];
				   castleLy += RookN1x_Zy[1];

				   if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0 && castleLx > 0)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[castleLx][castleLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[castleLx][castleLy] == 0 && castleLx > 0)
				   {
					   // sarý kareye boya ...
					   if (Black_or_White[castleLx][castleLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }


				   // LEFT

				   castleLx = Lx;
				   castleLy = Ly;
				   castleLx += RookZx_N1y[0];
				   castleLy += RookZx_N1y[1];

				   if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0 && castleLy >= 0)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[castleLx][castleLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[castleLx][castleLy] == 0 && castleLy >= 0)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[castleLx][castleLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   // RÝGHT

				   castleLx = Lx;
				   castleLy = Ly;
				   castleLx += RookZx_P1y[0];
				   castleLy += RookZx_P1y[1];

				   if (label_matrix[castleLx][castleLy] < 7 && label_matrix[castleLx][castleLy] != 0 && castleLy < 8)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[castleLx][castleLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[castleLx][castleLy] == 0 && castleLy < 8)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[castleLx][castleLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * castleLy), 0, (17.5F - 5 * castleLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   //	Çapraz
				   /* BISHOP +X +Y */

				   int bishopLx = Lx, bishopLy = Ly;
				   bishopLx += BishopP1x_P1y[0];
				   bishopLy += BishopP1x_P1y[1];
				   if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0 && bishopLx < 8 && bishopLy < 8)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[bishopLx][bishopLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx < 8 && bishopLy < 8)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[bishopLx][bishopLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   /* BISHOP +X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   bishopLx += BishopP1x_N1y[0];
				   bishopLy += BishopP1x_N1y[1];
				   if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0 && bishopLx < 8 && bishopLy >= 0)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[bishopLx][bishopLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx < 8 && bishopLy >= 0)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[bishopLx][bishopLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   /* BISHOP -X -Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   bishopLx += BishopN1x_N1y[0];
				   bishopLy += BishopN1x_N1y[1];
				   if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0 && bishopLx >= 0 && bishopLy >= 0)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[bishopLx][bishopLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLy >= 0 && bishopLx >= 0)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[bishopLx][bishopLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   /* BISHOP -X +Y */

				   bishopLx = Lx;
				   bishopLy = Ly;
				   bishopLx += BishopN1x_P1y[0];
				   bishopLy += BishopN1x_P1y[1];
				   if (label_matrix[bishopLx][bishopLy] < 7 && label_matrix[bishopLx][bishopLy] != 0 && bishopLx >= 0 && bishopLy < 8)
				   {
					   // kýrmýzý kareye boya ...
					   if (Black_or_White[bishopLx][bishopLy] == 1)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerBlackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturedangerWhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);

				   }
				   else if (label_matrix[bishopLx][bishopLy] == 0 && bishopLx >= 0 && bishopLy < 8)
				   {
					   // sarý kareye boya ...

					   if (Black_or_White[bishopLx][bishopLy] != 0)
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectblackptr);
					   else
						   g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureselectwhiteptr);

					   XMMATRIX mTranslatePointer = XMMatrixTranslation((-17.5F + 5 * bishopLy), 0, (17.5F - 5 * bishopLx));
					   cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
					   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
					   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
					   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					   g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
					   g_pImmediateContext->Draw(vertexCountpointer, 0);
				   }


				   break;
		}

		default:
		break;
		}

	
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render(double time)
{
    // Update our time
	rot += 1.0f * time;
	if(rot > 6.28f) rot = 0.0f;

	Ro = Eye;
	Rd = XMVector3Normalize(At - Eye);


    // Clear the back buffer
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );


    cBuffer.mView = XMMatrixTranspose( g_View );
	XMStoreFloat4( &cBuffer.vEyePos, Eye );
    g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );

    XMFLOAT4 vLightPos[2] =
    {
        XMFLOAT4( 0.0f,  0.0f,   0.0f,  1.0f ),
		XMFLOAT4( 0.0f,  0.0f,   0.0f,  1.0f )
    };

    XMFLOAT4 vLightColors[2] =
    {
        XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ),
		XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f )
    };

	cBuffer.vLightColor[0] = vLightColors[0];
	cBuffer.vLightColor[1] = vLightColors[1];
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );

	UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer_Light, &stride, &offset );

	XMMATRIX mRotate;
	XMMATRIX mTranslate1, mTranslate2, mTranslate3;

	mTranslate1	= XMMatrixTranslation( -5.0f, 0.0f, 0.0f );
	mTranslate2	= XMMatrixTranslation( -10.0f, 20.0f, 10.0f );
	mRotate 	= XMMatrixRotationY( rot );
	
	XMVECTOR vLightPos0 = XMLoadFloat4( &vLightPos[0] );
	vLightPos0 = XMVector3Transform( vLightPos0, mTranslate1 );
	vLightPos0 = XMVector3Transform( vLightPos0, mRotate );
	vLightPos0 = XMVector3Transform( vLightPos0, mTranslate2 );
	XMStoreFloat4( &vLightPos[0], vLightPos0 );

	cBuffer.mWorld = XMMatrixTranspose(mTranslate1 * mRotate * mTranslate2); 
	cBuffer.vLightPos[0] = vLightPos[0];
	cBuffer.vMeshColor = vLightColors[0];
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );

	// Render White Light
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
	g_pImmediateContext->PSSetShader( g_pPixelShader_Solid, NULL, 0 );
	g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
	g_pImmediateContext->Draw(vertexCountlight, 0);

	mTranslate1	= XMMatrixTranslation(  5.0f, 0.0f, 0.0f );
	mTranslate3	= XMMatrixTranslation(  10.0f, 20.0f, -10.0f );

	XMVECTOR vLightPos1 = XMLoadFloat4( &vLightPos[1] );
	vLightPos1 = XMVector3Transform( vLightPos1, mTranslate1 );
	vLightPos1 = XMVector3Transform( vLightPos1, mRotate );
	vLightPos1 = XMVector3Transform( vLightPos1, mTranslate3 );
	XMStoreFloat4( &vLightPos[1], vLightPos1 );

	cBuffer.mWorld = XMMatrixTranspose(mTranslate1 * mRotate * mTranslate3); 
	cBuffer.vLightPos[1] = vLightPos[1];
	cBuffer.vMeshColor = vLightColors[1];
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );

	// Render Yellow Light
	g_pImmediateContext->Draw(vertexCountlight, 0);
	

	// Render CrossHair
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_CrossHair, &stride, &offset);
	XMFLOAT4 sPosition; 
	XMStoreFloat4( &sPosition, crosshairPosition );
	XMMATRIX mTranslate	= XMMatrixTranslation( sPosition.x, sPosition.y, sPosition.z );
    cBuffer.mWorld = XMMatrixTranspose( mTranslate );	
	cBuffer.vMeshColor = XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ); 
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pConstantBuffer );
	g_pImmediateContext->PSSetShader( g_pPixelShader_Solid, NULL, 0 );
	g_pImmediateContext->Draw(vertexCountcrosshair, 0);

	g_World = XMMatrixIdentity();
    cBuffer.mWorld = XMMatrixTranspose( g_World );


    // Render ChessBoard
	g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer_chessBoard, &stride, &offset );
	cBuffer.vMeshColor = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ); 
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0 );
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pConstantBuffer );
    g_pImmediateContext->PSSetShader( g_pPixelShader_Textured, NULL, 0 );
	g_pImmediateContext->PSSetConstantBuffers( 2, 1, &g_pConstantBuffer );
	g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTexturechessBoard );
	g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );
	g_pImmediateContext->Draw(vertexCountchessBoard, 0);


	// Render Pointer
	XMVECTOR translatePointer = testIntersection(Ro, Rd, verticeschessBoard, vertexCountchessBoard, false, cBuffer.mWorld);
	XMFLOAT3 translateP;
	XMStoreFloat3(&translateP, translatePointer);
	if (translateP.x != 0.0F)
	{
		int x, y;

		x = (translateP.x + 20) / 5.0f;
		y = (translateP.z + 20) / 5.0f;

		if (Black_or_White[x][y] == 1)			
			g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturepointerWhite);
		else
			g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexturepointerBlack);

		XMMATRIX mTranslatePointer = XMMatrixTranslation(translateP.x, 0, translateP.z);
		cBuffer.mWorld = XMMatrixTranspose(mTranslatePointer);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Pointer, &stride, &offset);
		cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountpointer, 0);
	}

	// render yellow and red point
	yellow_red_point(Lx, Ly, stride, offset);   // SARI VE KIRMIZI NOKTALARI RENDER EDEN FONKSÝYON LX VE LY ÝLK TIKLANAN YERÝN MATRÝSDEKÝ ÝNDÝS DEÐERÝ

	// render stones

	labelmatPtr = &label_matrix[0][0];    // label pointer...
	for (int i = 0; i < 8; i++)							// RENDER 
	{
		for (int j = 0; j < 8; j++)
		{
			switch (*labelmatPtr)
			{
				case 0:
				{
					  labelmatPtr++;
					  continue;
				}
				case 1:
				{
					 //Black castles rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_kale, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black  
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountkale, 0);
					labelmatPtr++;
					break;
				}
				case 2:
				{
					// Black horses rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_at, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountat, 0);
					labelmatPtr++;
					break;
				}
				case 3:
				{
					// Black elephants rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_fil, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountfil, 0);
					labelmatPtr++;
					break;
				}
				case 4:
				{
					// Black shah rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sah, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountsah, 0);
					labelmatPtr++;
					break;
				}
				case 5:
				{
					// Black vizier rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_vezir, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountvezir, 0);
					labelmatPtr++;
					break;
				}
				case 6:
				{
					// Black pawn rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_piyon, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountpiyon, 0);
					labelmatPtr++;
					break;
				}
				case 7:
				{
					// White pawn rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_piyon, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // White 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(((-17.5F) + (j*(5.0F))), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountpiyon, 0);
					labelmatPtr++;
					break;
				}
				case 8:
				{
					// White castles rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_kale, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountkale, 0);
					labelmatPtr++;
					break;
				}
				case 9:
				{
				// White horses rendering.
				g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_at, &stride, &offset);
				cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
				g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				g_pImmediateContext->Draw(vertexCountat, 0);
				labelmatPtr++;
				break;
				}
				case 10:
				{
					// White elephants rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_fil, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountfil, 0);
					labelmatPtr++;
					break;
				}
				case 11:
				{
					// White shah rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sah, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountsah, 0);
					labelmatPtr++;
					break;
				}
				case 12:
				{
					// White vizier rendering.
					g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_vezir, &stride, &offset);
					cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
					cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-17.5F + (j*5.0F), 0, 17.5F - (i*5.0F)));
					g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
					g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
					g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
					g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
					g_pImmediateContext->Draw(vertexCountvezir, 0);
					labelmatPtr++;
					break;
				}
			default:
				break;
			}
		}
	}

	int *labelPtr;
	labelPtr = &labelbuffer2[0];
	int k = 0, h = 0, b = 0, p = 0, wp = 0;
	for (int i = 0; i <= 32; i++)
	{
	
		// yenen taþlarýn renderý
	
		switch (*labelPtr)
		{
		case 0:
		{
				  labelPtr++;
				  break;
		}
		case 1:
		{
			  
				  //Black castles rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_kale, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black  
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation((-22.5F - k * 5.0F), 0, 17.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountkale, 0);
				  k++;
				  labelPtr++;
				  break;
		}
		case 2:
		{
			  
				  // Black horses rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_at, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-22.5F - (h*5.0F), 0, 12.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountat, 0);
				  h++;
				  labelPtr++;
				  break;
		}
		case 3:
		{
				  // Black elephants rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_fil, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-22.5F - (b*5.0F), 0, 7.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountfil, 0);
				  b++; 
				  labelPtr++;
				  break;
		}
		case 4:
		{
				  // Black shah rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sah, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-22.5F, 0, 2.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountsah, 0);
				  labelPtr++;
				  break;
		}
		case 5:
		{
				  // Black vizier rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_vezir, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(-27.5F, 0, 2.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountvezir, 0);
				  labelPtr++;
				  break;
		}
		case 6:
		{
				  // Black pawn rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_piyon, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // black
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation((p==0 ? -22.5F : (p==1 ? -27.5F : (p==2 ? -22.5F :(p==3 ? -27.5F :(p==4 ? -22.5F : false))))), 0, -2.5F - p*5.0F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountpiyon, 0);
				  p++;
				  labelPtr++;
				  break;
		}
		case 7:
		{
				  // White pawn rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_piyon, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // White 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation((wp == 0 ? 22.5F : (wp == 1 ? 27.5F : (wp == 2 ? 22.5F : (wp == 3 ? 27.5F : (wp == 4 ? 22.5F : false))))), 0, -2.5F - wp*5.0F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountpiyon, 0);
				  wp++;
				  labelPtr++;
				  break;
		}
		case 8:
		{
				  // White castles rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_kale, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation((17.5F + k * 5.0F), 0, 17.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountkale, 0);
				  labelPtr++;
				  break;
		}
		case 9:
		{
				  // White horses rendering.
				  g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_at, &stride, &offset);
				  cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				  cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(17.5F + (h*5.0F), 0, 12.5F));
				  g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				  g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				  g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				  g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				  g_pImmediateContext->Draw(vertexCountat, 0);
				  labelPtr++;
				  break;
		}
		case 10:
		{
				   // White elephants rendering.
				   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_fil, &stride, &offset);
				   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				   cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(17.5F + (b*5.0F), 0, 7.5F));
				   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->Draw(vertexCountfil, 0);
				   labelPtr++;
				   break;
		}
		case 11:
		{
				   // White shah rendering.
				   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sah, &stride, &offset);
				   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				   cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(22.5F, 0, 2.5F));
				   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->Draw(vertexCountsah, 0);
				   labelPtr++;
				   break;
		}
		case 12:
		{
				   // White vizier rendering.
				   g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_vezir, &stride, &offset);
				   cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // white 
				   cBuffer.mWorld = XMMatrixTranspose(XMMatrixTranslation(27.5F, 0, 2.5F));
				   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
				   g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
				   g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
				   g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
				   g_pImmediateContext->Draw(vertexCountvezir, 0);
				   labelPtr++;
				   break;
		}
		default:
			break;
		}
	}
	// Print position
	std::wostringstream outs;
	outs << " Cell Position = (" << translateP.x << ", " << translateP.y << ", " << translateP.z << ")";
	SetWindowText(g_hWnd, outs.str().c_str());
	

	// Present our back buffer to our front buffer
	g_pSwapChain->Present( 0, 0 );

}

XMVECTOR testIntersection(XMVECTOR Ro, XMVECTOR Rd, SimpleVertex* verticesModel, int vertexCount, bool transform, XMMATRIX& g_World)
{
	for(int i = 0; i < vertexCount; i+=3)
	{
		XMVECTOR V0 = XMLoadFloat3(&verticesModel[i].Pos );
		XMVECTOR V1 = XMLoadFloat3(&verticesModel[i+1].Pos );
		XMVECTOR V2 = XMLoadFloat3(&verticesModel[i+2].Pos );

		if(transform)
		{
			V0 = XMVector3Transform(V0, g_World);
			V1 = XMVector3Transform(V1, g_World);
			V2 = XMVector3Transform(V2, g_World);
		}

		XMVECTOR Edge1 = V1 - V0;
		XMVECTOR Edge2 = V2 - V0;
		XMVECTOR Normal = XMVector3Cross(Edge1,Edge2);

		//Backface test
		XMVECTOR toEye = Eye - V0;
		if(XMVectorGetX(XMVector3Dot(Normal,toEye)) < 0.0F) continue;
		
		float D = -XMVectorGetX( XMVector3Dot(Normal,V0));

		float t = -(XMVectorGetX( XMVector3Dot(Normal,Ro)) + D) / XMVectorGetX( XMVector3Dot(Normal,Rd));

		if(t > 0.0F)
		{
			XMVECTOR R = Ro + t * Rd;

			XMVECTOR Edge3 = V2 - V1;
			XMVECTOR Edge4 = R - V0;
			XMVECTOR Edge5 = R - V1;

			float S  = XMVectorGetX(XMVector3Length(XMVector3Cross(Edge1,Edge2)));
			float s1 = XMVectorGetX(XMVector3Length(XMVector3Cross(Edge4,Edge2)));
			float s2 = XMVectorGetX(XMVector3Length(XMVector3Cross(Edge4,Edge1)));
			float s3 = XMVectorGetX(XMVector3Length(XMVector3Cross(Edge5,Edge3)));

			float fark = abs(S - (s1 + s2 + s3));

			float epsilon = 0.005F;
			 
			if (fark <= epsilon)
			{
				XMVECTOR Edge = V2 - V1;
				if (XMVectorGetX(XMVector3Length(Edge)) > 6.0F)
					return XMVECTOR((V1 + V2)/2);
				else
					return XMVECTOR((V0 + V1) / 2);
			}
		}		
	}	
	return XMVECTOR();
}