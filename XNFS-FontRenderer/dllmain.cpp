// NFS MW/Carbon FEngFont renderer hook

// TODO: Word warping, scaling & rotation, optimization
// Currently using D3DXFONT for vector font rendering, which doesn't support scaling or rotation
// also there is a possibility of saving some font draws by keeping them in memory and not redrawing them constantly

#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "includes\injector\injector.hpp"
#include "includes\IniReader.h"

#ifdef GAME_CARBON
#include "FontStuff_Carbon.h"
#endif

#ifdef GAME_MW
#include "FontStuff_MW.h"
#endif

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

RECT rect, window;

D3DVIEWPORT9    g_ViewPort;
LPDIRECT3DDEVICE9 g_D3DDevice;

LPD3DXFONT      g_pFont = NULL; //D3D Font
wchar_t BodyFontName[255] = BODYFONT;


LPD3DXFONT      g_pTitleFont = NULL; //D3D Font 2
wchar_t TitleFontName[255] = TITLEFONT;

#ifdef GAME_MW
LPD3DXFONT      g_pSpeedometerFont = NULL; //D3D Font 3
wchar_t SpeedometerFontName[255] = SPEEDOMETERFONT;
#endif



int winX, winY;

unsigned int pointertosave = 0;

//unsigned int FEngFont_RenderStringOriginal_Address = 0x00599CC0;

unsigned int FontHash = 0;

unsigned int StealCaveExit = STEALCAVEEXITADDRESS;
unsigned int FindFont = FINDFONTFUNCTION;

#ifdef GAME_CARBON
unsigned int DisableRenderExit = DISABLERENDEREXITADDRESS;
#endif

unsigned int FontHeight = DEFAULTBODYHEIGHT;
unsigned int TitleFontHeight = DEFAULTTITLEHEIGHT;

#ifdef GAME_MW
unsigned int SpeedometerFontHeight = DEFAULTSPEEDOHEIGHT;
#endif

struct FEColorStruct
{
	unsigned int Blue;
	unsigned int Green;
	unsigned int Red;
	unsigned int Alpha;
}FEColor;

struct FEString
{
	void *somecallback;
	unsigned int unk1;
	void *unk2;
	unsigned int unk3;
	unsigned int unk4;
	unsigned int unk25;
	unsigned int unk26;
	float unk5;
	unsigned int unk6;
	unsigned int FontHash;
	unsigned int unk7;
	void *unk8;
	unsigned int unk9;
	void *unk10;
	unsigned int unk11;
	unsigned int unk12;
	unsigned int unk13;
	void *unk14;
	unsigned int unk15;
	void *unk16;
	void *unk17;
	void *unk18;
	unsigned int unk19;
	unsigned int unk20;
	unsigned int unk21;
	wchar_t string;
	unsigned int word_warp;
	unsigned int JustificationType;
	unsigned int maxWidth;
}TheFEString;

unsigned int GetNumberOfLines(LPCWSTR text)
{
	unsigned int lines = 1;
	unsigned int len = lstrlenW(text);
	unsigned int pos = 0;
//	const wchar_t *newline;
	while (pos < len)
	{
		if (text[pos] == '\n')
			lines++;
		pos++;
	}

	return lines;
}

void WriteText(INT x, INT y, DWORD color, LPCWSTR text, unsigned int InFontHash, FEString InFEString)
{
	unsigned int FontJustify = DT_LEFT;
	unsigned int TimesToHeighten = (InFEString.JustificationType / 4) % 4;
	unsigned int NumberOfLines = GetNumberOfLines(text);
	//y -= (FontHeight / 2) * TimesToHeighten;

	switch (InFEString.JustificationType % 4)
	{
	case 0:
		FontJustify = DT_LEFT;
		break;
	case 1:
		FontJustify = DT_CENTER;
		break;
	case 2:
		FontJustify = DT_RIGHT;
		break;
	case 3:
		FontJustify = DT_CENTER;
		break;
	default:
		break;
	}

	
	switch (InFontHash)
	{
#ifdef GAME_MW
	case SPEEDOMETERFONTHASH:
		if (TimesToHeighten)
			y -= FontHeight * 2 * TimesToHeighten * NumberOfLines;
		SetRect(&rect, x, y, x, y);
		if (g_pSpeedometerFont)
			g_pSpeedometerFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP | FontJustify, color);
		break;
#endif
	case TITLEFONTHASH:
		if (TimesToHeighten)
			y -= FontHeight * 2 * TimesToHeighten * NumberOfLines;
		SetRect(&rect, x, y, x, y);
		if (g_pTitleFont)
			g_pTitleFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP | FontJustify, color);
		break;
	case BODYFONTHASH:
		if (TimesToHeighten)
			y -= (FontHeight / 2) * TimesToHeighten * NumberOfLines;
		SetRect(&rect, x, y, x, y);
		if (g_pFont)
			g_pFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP | FontJustify, color);
		break;
	default:
		if (TimesToHeighten)
			y -= (FontHeight / 2) * TimesToHeighten * NumberOfLines;
		SetRect(&rect, x, y, x, y);
		if (g_pFont)
			g_pFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP | FontJustify, color);
	}
}

void SearchNewlines(wchar_t* Input)
{
	wchar_t *newline;

	while (newline = wcschr(Input, '^'))
		*newline = '\n';
}

void __stdcall WriteTextOSD(FEColorStruct *InputFEColor, D3DXMATRIX *InputMatrix, wchar_t *InText)
{
	float RenderMatrixX = 426.66666;
	float RenderMatrixY = 240;
	D3DXMATRIX TheMatrix = *InputMatrix;
	FEColor = *InputFEColor;

	SearchNewlines(InText);

	//printf("Matrix:\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n", TheMatrix._11, TheMatrix._12, TheMatrix._13, TheMatrix._14, TheMatrix._21, TheMatrix._22, TheMatrix._23, TheMatrix._24, TheMatrix._31, TheMatrix._32, TheMatrix._33, TheMatrix._34, TheMatrix._41, TheMatrix._42, TheMatrix._43, TheMatrix._44);
	winX = (INT)round((TheMatrix._41 * (g_ViewPort.Width / ((RenderMatrixX * 2) + 1))));

	if (FontHash == TITLEFONTHASH)
	{
		winX = (INT)round((TheMatrix._41 * (g_ViewPort.Width / ((RenderMatrixX * 2) + 1)) * 1.012));
		winY = (INT)round((TheMatrix._42 * (g_ViewPort.Height / ((RenderMatrixY * 2) - 1)) * 1.017));
	}
	else
	{
		winX = (INT)round((TheMatrix._41 * (g_ViewPort.Width / ((RenderMatrixX * 2) + 1))));
		winY = (INT)round((TheMatrix._42 * (g_ViewPort.Height / ((RenderMatrixY * 2) - 1))));
	}

	WriteText(winX, winY, D3DCOLOR_ARGB(FEColor.Alpha, FEColor.Red, FEColor.Green, FEColor.Blue), InText, FontHash, TheFEString);
	//g_pFont->Release();
}

int __stdcall GetD3DDevice()
{
	g_D3DDevice = *(LPDIRECT3DDEVICE9*)D3DDEVICE_ADDRESS;

	// BROKEN IN MW!! Why is it reporting 40x22 viewport on a 720p surface beats me...
#ifdef GAME_MW
	unsigned int MW_ViewportWidth = 0;
	unsigned int MW_ViewportHeight = 0;
	GetCurrentRes(&MW_ViewportWidth, &MW_ViewportHeight);

	g_ViewPort.Width = MW_ViewportWidth;
	g_ViewPort.Height = MW_ViewportHeight;
#else
	g_D3DDevice->GetViewport(&g_ViewPort);
#endif
	D3DXCreateFont(g_D3DDevice, FontHeight, 0, FW_SEMIBOLD, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, BodyFontName, &g_pFont); //Create fonts																																												   //D3DXCreateFont(g_D3DDevice, FontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MicroSquare Bold", &g_pFont); //Create fonts
	D3DXCreateFont(g_D3DDevice, TitleFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TitleFontName, &g_pTitleFont); //Create fonts
#ifdef GAME_MW
	D3DXCreateFont(g_D3DDevice, SpeedometerFontHeight, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, SpeedometerFontName, &g_pSpeedometerFont); //Create fonts
#endif

	return 0;
}


void __stdcall FEngFont_RenderStringHook(FEColorStruct *Color, wchar_t *pcString, FEString *obj, D3DXMATRIX *matrix, unsigned int cached)
{
	_asm mov pointertosave, ecx

	if (!g_pFont)
		D3DXCreateFont(g_D3DDevice, FontHeight, 0, FW_SEMIBOLD, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, BodyFontName, &g_pFont); //Create fonts																																												   //D3DXCreateFont(g_D3DDevice, FontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MicroSquare Bold", &g_pFont); //Create fonts
	if (!g_pTitleFont)
		D3DXCreateFont(g_D3DDevice, TitleFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TitleFontName, &g_pTitleFont); //Create fonts

#ifdef GAME_MW
	if (!g_pSpeedometerFont)
		D3DXCreateFont(g_D3DDevice, SpeedometerFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, SpeedometerFontName, &g_pSpeedometerFont); //Create fonts
#endif

	//wprintf(L"FEngFont::RenderString(\nFEColor&: %X\nwchar_t *Text: %s\nFEString *Unk2: %X\nbMatrix4 *Unk3: %X\nFERenderObject *Unk4: %X\n)\n", a1, a2, a3, a4, a5);
	TheFEString = *obj;
	
	WriteTextOSD(Color, matrix, pcString);
}

void __declspec(naked) StealFontHash()
{
	_asm
	{
		mov FontHash, eax
		call FindFont
		jmp StealCaveExit
	}
}

#ifdef GAME_CARBON

void __stdcall DisableRendering()
{
	unsigned int PreviousECX, PreviousESI;
	_asm mov PreviousECX, ecx
	_asm mov PreviousESI, esi
	if (g_pFont)
	{
		g_pFont->Release();
		g_pFont = 0;
	}
	if (g_pTitleFont)
	{
		g_pTitleFont->Release();
		g_pTitleFont = 0;
	}
	_asm mov esi, PreviousESI
	_asm mov ecx, PreviousECX
}

void __declspec(naked) StopRenderingCave()
{
	DisableRendering();
	_asm
	{
		push 0x0FFFFFFFF
		push 0x0FFFFFFFF
		push 0x0FFFFFFFF
		jmp DisableRenderExit
	}
}

#endif

int InitConfig()
{
	CIniReader inireader("");
	char *MBString;

	MBString = inireader.ReadString("Fonts", "BodyFont", BODYFONT_MB);
	mbstowcs(BodyFontName, MBString, 255);

	MBString = inireader.ReadString("Fonts", "TitleFont", TITLEFONT_MB);
	mbstowcs(TitleFontName, MBString, 255);

#ifdef GAME_MW
	MBString = inireader.ReadString("Fonts", "SpeedometerFont", SPEEDOMETERFONT_MB);
	mbstowcs(SpeedometerFontName, MBString, 255);
#endif

	FontHeight = inireader.ReadInteger("Heights", "BodyHeight", DEFAULTBODYHEIGHT);
	TitleFontHeight = inireader.ReadInteger("Heights", "TitleHeight", DEFAULTTITLEHEIGHT);

#ifdef GAME_MW
	SpeedometerFontHeight = inireader.ReadInteger("Heights", "SpeedometerHeight", DEFAULTSPEEDOHEIGHT);
#endif

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		injector::MakeCALL(RENDERSTRING_ADDRESS, FEngFont_RenderStringHook, true);
		injector::MakeJMP(STEALFONT_ADDRESS, StealFontHash, true);
		injector::MakeCALL(GETD3DDEV_ADDRESS, GetD3DDevice, true);
#ifndef GAME_MW // needs to be done for MW specifically to avoid crashing during video settings changes
		injector::MakeJMP(STOPRENDERING_ADDRESS, StopRenderingCave, true);
#endif
		InitConfig();
		return TRUE;
	}

	return FALSE;
}