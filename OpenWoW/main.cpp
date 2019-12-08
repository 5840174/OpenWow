#include "stdafx.h"

// Additional (OW)
#include "GameState_Client.h"
#include "GameState_Menu.h"
#include "GameState_World.h"

IApplication * _ApplicationInstance = nullptr;

int main(int argumentCount, char* arguments[])
{
#ifdef _DEBUG 
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
	{
		IBaseManager* BaseManager = std::make_shared<CBaseManager>();

		std::shared_ptr<IznPluginsManager> pluginsManager = std::make_shared<CznPluginsManager>(BaseManager);
		pluginsManager->RegisterPlugin("znPlugin.dll");
		pluginsManager->RegisterPlugin("znEngine.dll");
		pluginsManager->RegisterPlugin("znRenderDX11.dll");
		pluginsManager->RegisterPlugin("znRenderOpenGL.dll");

		GetManager<IFilesManager>(BaseManager)->RegisterFilesStorage(std::make_shared<CLocalFilesStorage>("D:\\_programming\\OpenWow\\_gamedata\\"));
		GetManager<IFilesManager>(BaseManager)->RegisterFilesStorage(std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::PRIOR_HIGH));
		GetManager<IFilesManager>(BaseManager)->RegisterFilesStorage(std::make_shared<CLibraryResourceFileStotage>(::GetModuleHandle(NULL)));

		OpenDBs(BaseManager);

		//--

        HMODULE m_HINSTANCE = ::GetModuleHandle(NULL);

        CWindowObject* windowObject = new CWindowObject();
        windowObject->RegisterWindowClass(m_HINSTANCE);
        windowObject->CreateWindowInstance(1280, 1024);

		Application app(BaseManager);
		_ApplicationInstance = &app;

        std::shared_ptr<IRenderDevice> renderDevice = app.CreateRenderDevice(RenderDeviceType::RenderDeviceType_OpenGL);
        std::shared_ptr<RenderWindow> renderWindow = app.CreateRenderWindow(windowObject, true);


        std::shared_ptr<IFontsManager> fontsManager = std::make_shared<FontsManager>(BaseManager);
        AddManager<IFontsManager>(BaseManager, fontsManager);

        app.AddGameState(GameStatesNames::GAME_STATE_MENU, std::make_shared<CGameState_Menu>(&app));
		app.AddGameState(GameStatesNames::GAME_STATE_WORLD, std::make_shared<CGameState_World>(&app));
		app.AddGameState(GameStatesNames::GAME_STATE_CLIENT, std::make_shared<CGameState_Client>(&app));
		app.SetGameState(GameStatesNames::GAME_STATE_WORLD);
		app.Run();
	}

	return 0;
}
