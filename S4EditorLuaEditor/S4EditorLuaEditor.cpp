// S4EditorLuaEditor.cpp: Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"
#include "S4EditorLuaEditor.h"
#include <stdio.h>
/*
	?CreatePreview@@YGXPAVIMapGeneratorHost@@PAGH@Z
	?GenerateRandomMap@@YG_NPAVIMapGeneratorHost@@@Z
	?GetRandomMapGeneratorInterfaceVersion@@YGHXZ
	?InitRandomMap@@YGXPAVIMapGeneratorHost@@PAUSRandomMapParams@@@Z
*/
//"PSEUDO" class that just tunnels function calls to another function
struct SRandomMapParams;
class IMapGeneratorHost;

typedef void(__stdcall *MapGenerator_CreatePreview_t)(class IMapGeneratorHost*, unsigned short*, int);
typedef bool(__stdcall *MapGenerator_GenerateRandomMap_t)(class IMapGeneratorHost*);
typedef void(__stdcall *MapGenerator_InitRandomMap_t)(class IMapGeneratorHost*, struct SRandomMapParams*);
typedef int(__stdcall *MapGenerator_GetRandomMapGeneratorInterfaceVersion_t)(void);

static HMODULE MapGeneratorModule = nullptr;

static MapGenerator_CreatePreview_t MapGenerator_CreatePreview = nullptr;
static MapGenerator_GenerateRandomMap_t MapGenerator_GenerateRandomMap = nullptr;
static MapGenerator_InitRandomMap_t MapGenerator_InitRandomMap = nullptr;
static MapGenerator_GetRandomMapGeneratorInterfaceVersion_t MapGenerator_GetRandomMapGeneratorInterfaceVersion = nullptr;

void initOriginalModule()
{
	MapGeneratorModule = LoadLibrary(L"MapGenerator2");
	if (MapGeneratorModule == nullptr)
	{
		MessageBox(NULL, L"MapGenerator2.dll wurde nicht im Editorverzeichnis gefunden.\r\nDas Programm funktioniert noch, aber du kannst keine Zufallsgenerierten Maps erstellen, bis die DLL im Verzeichnis enthalten ist. (Wenn du die Datei rein tust, ist kein Neustart erforderlich", L"DLL Nicht Gefunden!", MB_OK);
	}
}
void freeAll()
{
	if (FreeLibrary(MapGeneratorModule))
	{
		MapGenerator_CreatePreview = nullptr;
		MapGenerator_GenerateRandomMap = nullptr;
		MapGenerator_InitRandomMap = nullptr;
		MapGenerator_GetRandomMapGeneratorInterfaceVersion = nullptr;
		MapGeneratorModule = nullptr;
	}
}
void initFunctions()
{
	if (MapGeneratorModule)
	{
		MapGenerator_CreatePreview = (MapGenerator_CreatePreview_t)GetProcAddress(MapGeneratorModule, "?CreatePreview@@YGXPAVIMapGeneratorHost@@PAGH@Z");
		MapGenerator_GenerateRandomMap = (MapGenerator_GenerateRandomMap_t)GetProcAddress(MapGeneratorModule, "?GenerateRandomMap@@YG_NPAVIMapGeneratorHost@@@Z");
		MapGenerator_InitRandomMap = (MapGenerator_InitRandomMap_t)GetProcAddress(MapGeneratorModule, "?InitRandomMap@@YGXPAVIMapGeneratorHost@@PAUSRandomMapParams@@@Z");
		MapGenerator_GetRandomMapGeneratorInterfaceVersion = (MapGenerator_GetRandomMapGeneratorInterfaceVersion_t)GetProcAddress(MapGeneratorModule, "?GetRandomMapGeneratorInterfaceVersion@@YGHXZ");
		/* //DEBUG
		char buf[2048];
		sprintf_s(buf, "%s: %x\n\n%s: %x\n%s: %x\n%s: %x\n%s: %x\n%s: %x", "MapGeneratorModule", MapGeneratorModule, "CreatePreview", (int)MapGenerator_CreatePreview, "GenerateRandomMap", (int)MapGenerator_GenerateRandomMap,
			"InitRandomMap", (int)MapGenerator_InitRandomMap, "GetRandomMapGeneratorInterfaceVersion", (int)MapGenerator_GetRandomMapGeneratorInterfaceVersion,"^ Return value",MapGenerator_GetRandomMapGeneratorInterfaceVersion());
		MessageBoxA(NULL, buf, "DEBUG", MB_OK);*/
	}
}
// ON ATTACH
void  __declspec(dllexport) __stdcall CreatePreview(class IMapGeneratorHost* host, unsigned short* us1, int i1)
{
	if (MapGenerator_CreatePreview == nullptr)
	{
		if (MapGeneratorModule != nullptr)
		{
			initFunctions();
		}
		else
		{
			initOriginalModule();
			initFunctions();
		}
	}
	//not else { ... } but new if because check and then execute aftersards nonetheless
	if (MapGenerator_CreatePreview != nullptr)
	{
		MapGenerator_CreatePreview(host, us1, i1);
	}
}
bool __declspec(dllexport) __stdcall GenerateRandomMap(class IMapGeneratorHost* host)
{

	if (MapGenerator_GenerateRandomMap == nullptr)
	{
		if (MapGeneratorModule != nullptr)
		{
			initFunctions();
		}
		else
		{
			initOriginalModule();
			initFunctions();
		}
	}
	//not else because check and then execute aftersards
	if (MapGenerator_GenerateRandomMap != nullptr)
	{
		return MapGenerator_GenerateRandomMap(host);
	}
	return false;
}

void __declspec(dllexport) __stdcall InitRandomMap(class IMapGeneratorHost* host, struct SRandomMapParams* params)
{
	if (MapGenerator_InitRandomMap == nullptr)
	{
		if (MapGeneratorModule != nullptr)
		{
			initFunctions();
		}
		else
		{
			initOriginalModule();
			initFunctions();
		}
	}
	//not else because check and then execute aftersards
	if (MapGenerator_InitRandomMap != nullptr)
	{
		MapGenerator_InitRandomMap(host,params);
	}
}
int __declspec(dllexport) __stdcall GetRandomMapGeneratorInterfaceVersion(void)
{
	if (MapGenerator_GetRandomMapGeneratorInterfaceVersion == nullptr)
	{
		if (MapGeneratorModule != nullptr)
		{
			initFunctions();
		}
		else
		{
			initOriginalModule();
			initFunctions();
		}
	}
	//not else because check and then execute aftersards
	if (MapGenerator_GetRandomMapGeneratorInterfaceVersion != nullptr)
	{
		return MapGenerator_GetRandomMapGeneratorInterfaceVersion();
	}
	return 69;
}
