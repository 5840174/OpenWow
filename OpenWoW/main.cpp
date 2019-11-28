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
		std::shared_ptr<IBaseManager> BaseManager = std::make_shared<CBaseManager>();

		std::shared_ptr<CSettings> settings = std::make_shared<CSettings>(BaseManager);
		AddManager<ISettings>(BaseManager, settings);
		settings->AddDefaults();
		

		std::shared_ptr<CLog> log = std::make_shared<CLog>();
		AddManager<ILog>(BaseManager, log);

		std::shared_ptr<CConsole> console = std::make_shared<CConsole>(BaseManager);
		AddManager<IConsole>(BaseManager, console);
		console->AddCommonCommands();
		
		std::shared_ptr<IFilesManager> filesManager = std::make_shared<CFilesManager>(BaseManager);
		AddManager<IFilesManager>(BaseManager, filesManager);

		std::shared_ptr<IFilesStorage> localFilesGamedata = std::make_shared<CLocalFilesStorage>("D:\\_programming\\OpenWow\\_gamedata\\");
		filesManager->RegisterFilesStorage(localFilesGamedata);

		std::shared_ptr<IFilesStorage> mpqFileStorage = std::make_shared<CMPQFilesStorage>("D:\\_games\\World of Warcraft 1.12.1\\Data\\", IFilesStorageEx::PRIOR_HIGH);
		filesManager->RegisterFilesStorage(mpqFileStorage);

        HMODULE hModule = ::GetModuleHandle(NULL);
        std::shared_ptr<IFilesStorage> libraryFileStorage = std::make_shared<CLibraryResourceFileStotage>(hModule);
        filesManager->RegisterFilesStorage(libraryFileStorage);


		std::shared_ptr<IznPluginsManager> pluginsManager = std::make_shared<CznPluginsManager>(BaseManager);
		pluginsManager->RegisterPlugin("znPlugin.dll");

		OpenDBs(BaseManager);

		//--

        HMODULE m_HINSTANCE = ::GetModuleHandle(NULL);

        CWindowObject* windowObject = new CWindowObject();
        windowObject->RegisterWindowClass(m_HINSTANCE);
        windowObject->CreateWindowInstance(1280, 1024);

		Application app(BaseManager);
		_ApplicationInstance = &app;

        std::shared_ptr<IRenderDevice> renderDevice = app.CreateRenderDevice(IRenderDevice::DeviceType::DirectX);
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
