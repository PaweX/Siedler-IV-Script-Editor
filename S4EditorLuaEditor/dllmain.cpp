// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"
#include "S4EditorLuaEditor.h"
#include <iostream>
#include <bitset>
#include <array>
#include <thread>
#include "window.h"
/*
typedef void(__stdcall *MapGenerator_CreatePreview_t)(class IMapGeneratorHost*, unsigned short*, int);
typedef bool(__stdcall *MapGenerator_GenerateRandomMap_t)(class IMapGeneratorHost*);
typedef void(__stdcall *MapGenerator_InitRandomMap_t)(class IMapGeneratorHost*, struct SRandomMapParams*);
typedef int(__stdcall *MapGenerator_GetRandomMapGeneratorInterfaceVersion_t)(void);

void onAttachInitFunctions()
{
	MapGeneratorHandle = LoadLibrary(L"MapGenerator2");
	if (MapGeneratorHandle == nullptr)
	{
		std::string str = "Could not Load Library MapGenerator2, error code " + std::to_string(GetLastError()) + ". You will not be able to randomly generate maps!";
		MessageBoxA(nullptr, str.c_str(), "ERROR", MB_OK);
	}
	else
	{
		MapGenerator_CreatePreview = (MapGenerator_CreatePreview_t)GetProcAddress(MapGeneratorHandle, "CreatePreview");
		MapGenerator_GenerateRandomMap = (MapGenerator_GenerateRandomMap_t)GetProcAddress(MapGeneratorHandle, "GenerateRandomMap");
		MapGenerator_InitRandomMap = (MapGenerator_InitRandomMap_t)GetProcAddress(MapGeneratorHandle, "InitRandomMap");
		MapGenerator_GetRandomMapGeneratorInterfaceVersion = (MapGenerator_GetRandomMapGeneratorInterfaceVersion_t)GetProcAddress(MapGeneratorHandle, "GetRandomMapGeneratorInterfaceVersion");

	}
}
void onDetachUninitFunctions()
{
	MapGenerator_CreatePreview = nullptr;
	MapGenerator_GenerateRandomMap = nullptr;
	MapGenerator_InitRandomMap = nullptr;
	MapGenerator_GetRandomMapGeneratorInterfaceVersion = nullptr;
	FreeLibrary(MapGeneratorHandle);
	MapGeneratorHandle = nullptr;

}*/
using std::cout;
using std::endl;

HANDLE workerThread = nullptr;
HANDLE windowThread = nullptr;

constexpr unsigned char NULLTERMINATOR = '\0';
char* luaScript = nullptr;
unsigned short* mapProperties = nullptr;
HANDLE s4 = nullptr;
//set to true on input request
bool writeFlag = false;


/*
	returns whether the property prop in properties is 1 or 0 (boolean conversion)
*/
bool getProperty(unsigned short& properties,unsigned int prop)
{
	return (properties & (1 << prop)) != 0;
}
/*
	returns 1 if property bit differs between both properties, else 0
*/
bool propertyValueDiffer(unsigned short& properties_1, unsigned short& properties_2, unsigned int prop)
{
	return getProperty(properties_1, prop) xor getProperty(properties_2, prop);
}

DWORD WINAPI windowMainEntry(LPVOID lpParam)
{
	{
		HWND window = initWindow((HINSTANCE)lpParam);//lpParam = hmodule
		if (!window)
		{
			//cout << "Error on window creation. Code " << GetLastError() << endl;
		}
		else
		{
			ShowWindow(window, SW_SHOW);
			UpdateWindow(window);
			//cout << "POST SHOW WINDOW" << endl;
		}
	}
	MSG      msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return msg.wParam;
}
extern HWND hwndEdit;
extern HMENU hmenu;

DWORD WINAPI workerMainEntry(LPVOID lpParam)
{
	
	// Initiate addresses
	s4 = GetModuleHandle(nullptr);
	luaScript = (char*)((unsigned int)s4 + 0x7512C);
	mapProperties= (unsigned short*)((unsigned int)s4 + 0x5719C);
	*mapProperties = 4;

	//cout << "Hello from worker thread" << endl;
	//cout << std::hex << "base addr of s4 is " << s4 << endl
	//	<< "lua string addr is " << (void*)luaScript << endl;

	// race condition almost not possible, only write on luaScript is on command and when does luascript change pfft ha almost never
	char buf[0xFFFE];
	char bufTmp[0xFFFE];
	strcpy_s<0xFFFE>(buf, luaScript);
	while (hwndEdit == nullptr)
	{
		if (strcmp(buf, luaScript) != 0)
		{
			//std::cout << "Script changed!---\n" << luaScript << endl;
			strcpy_s<0xFFFE>(buf, const_cast<const char*>(luaScript));
		}
		Sleep(1000);

	}
	SetWindowTextA(hwndEdit, luaScript);
	strcpy_s<0xFFFE>(buf, luaScript);
	unsigned short lastMapProperties = *mapProperties;
	while (hwndEdit != nullptr)
	{
		if (lastMapProperties != *mapProperties)
		{
			//std::cout << "props changed: " << std::bitset<16>(*mapProperties) << std::endl;

			//manually decide which values get checked for change
			if(propertyValueDiffer(lastMapProperties,*mapProperties,MMAP_NEW_WORLD))
			{
				//std::cout << "10th bit changed" << std::endl;

				auto MF_now = (bool)(getProperty(*mapProperties,MMAP_NEW_WORLD)) ? MF_CHECKED : MF_UNCHECKED;
				CheckMenuItem(hmenu, CHECK_NEW_WORLD, MF_now);
			}
			if (propertyValueDiffer(lastMapProperties, *mapProperties, MMAP_HIDE_MAP_PREVIEW))
			{
				auto MF_now = (bool)(getProperty(*mapProperties, MMAP_HIDE_MAP_PREVIEW)) ? MF_CHECKED : MF_UNCHECKED;
				CheckMenuItem(hmenu, CHECK_MAP_PREVIEW, MF_now);
			}
			lastMapProperties = *mapProperties;
		}
		if (strcmp(buf, luaScript) != 0)
		{
			/*	buf			(changed to luascript after differenct), 
				bufTmp		(changes to hwndEdit content after difference), 
				luaScript	(changes asynchronously and not on this thread)
				*/
			//STUFF CHANGED
			//ONLY NOTIFY IF SCRIPT != WINDOW TEXT
			// if from user:
			GetWindowTextA(hwndEdit, bufTmp, 0xFFFE);
			if (strcmp(bufTmp, luaScript) != 0)
			{
				//on both not equal 
				//std::cout << "bufTmp: " << bufTmp << std::endl << "buf: " << buf << std::endl;
				if (strcmp(buf, bufTmp) != 0)
				{
					int answer = MessageBox(hwndEdit, L"Das Script wurde im Editor geändert, möchtest du das Script vom Editor öffnen? (Dein derzeitiges Script wird dabei entfernt)", L"Achtung!", MB_YESNO);
					if (answer == IDYES)
					{
						SetWindowTextA(hwndEdit, luaScript);
					}
				}
				else
				{
					SetWindowTextA(hwndEdit, luaScript);
				}
			}
			strcpy_s<0xFFFE>(buf, const_cast<const char*>(luaScript));
		}

		Sleep(1000);

	}
	return EXIT_SUCCESS;
}
void initConsole()
{
	if (AllocConsole())
	{
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		//std::cout << "Hello we now have a console to work with!" << std::endl;
		SetConsoleTitle(L"S4Editor+ DEBUG Console");
	}
}
void onProcessAttach(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//for dll function tunneling
	initOriginalModule();
	initFunctions();

	//initConsole();

	//std::cout << "Init worker thread" << endl;
	workerThread = CreateThread(NULL, 0, workerMainEntry, nullptr, NULL, nullptr);
	windowThread = CreateThread(NULL, 0, windowMainEntry, hModule, NULL, nullptr);
	//cout << "Post init " << endl;
	
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		onProcessAttach(hModule,ul_reason_for_call,lpReserved);
		break;
    case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:	
		freeAll();
		//cout << "Waiting for worker thread termination" << endl;
		WaitForSingleObject(workerThread, 5000);
		CloseHandle(workerThread);
		CloseHandle(windowThread);
		//FreeConsole();
        break;
    }
    return TRUE;
}


