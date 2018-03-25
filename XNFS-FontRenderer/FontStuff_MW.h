#pragma once
#define TITLEFONTHASH 0xE0463AFA
#define BODYFONTHASH 0x545570C6
#define SPEEDOMETERFONTHASH 0x1424892

#define TITLEFONT L"Impact"
#define BODYFONT L"Microsquare"
#define SPEEDOMETERFONT L"DSEG7 Classic"

#define TITLEFONT_MB "Impact"
#define BODYFONT_MB "Microsquare"
#define SPEEDOMETERFONT_MB "DSEG7 Classic"

#define DEFAULTBODYHEIGHT 24
#define DEFAULTTITLEHEIGHT 38
#define DEFAULTSPEEDOHEIGHT 20

#define D3DDEVICE_ADDRESS 0x00982BDC
#define STEALCAVEEXITADDRESS 0x005A43B1
#define FINDFONTFUNCTION 0x00585B50

#define RENDERSTRING_ADDRESS 0x005A45A1
#define STEALFONT_ADDRESS 0x005A43AC
#define GETD3DDEV_ADDRESS 0x0066609D
//#define STOPRENDERING_ADDRESS 0x005CB059

float(__thiscall* FEngFont_GetLineWidth)(void* FEngFont, wchar_t *pcString, unsigned int flags, unsigned int maxWidth, bool word_wrap) = (float(__thiscall*)(void*, wchar_t*, unsigned int, unsigned int, bool))0x00586130;
void(__cdecl* GetCurrentRes)(unsigned int *OutWidth, unsigned int *OutHeight) = (void(__cdecl*)(unsigned int*, unsigned int*))0x6C27D0;
