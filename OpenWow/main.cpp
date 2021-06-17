#include "stdafx.h"

#include "SceneWoW.h"

#include "BugTrap/BugTrap.h"

void main_internal(int argumentCount, char* arguments[])
{
	CApplicationNative_PlatformWindows app;

	IRenderDevice& renderDevice = app.CreateRenderDevice(RenderDeviceType::RenderDeviceType_DirectX11);

	app.GetBaseManager().AddManager<IznFontsManager>(MakeShared(FontsManager, renderDevice, app.GetBaseManager()));

	{
		app.GetBaseManager().GetManager<ILoader>()->Start();

		{
			glm::ivec2 windowSize = app.GetBaseManager().GetManager<ISettings>()->GetGroup("Video")->GetPropertyT<glm::vec2>("WindowSize")->Get();
			std::unique_ptr<IznNativeWindow> nativeWindow = app.CreateNativeWindow("Zenon Engine", windowSize);

			const auto& renderWindow = renderDevice.GetObjectsFactory().CreateRenderWindow(std::move(nativeWindow), true);
			app.AddRenderWindow(renderWindow);

			std::shared_ptr<IScene> scene = MakeShared(CSceneWoW, app.GetBaseManager(), *renderWindow);
			renderWindow->SetRenderWindowEventListener(std::dynamic_pointer_cast<IRenderWindowEventListener>(scene));
			renderWindow->SetNativeWindowEventListener(std::dynamic_pointer_cast<IznNativeWindowEventListener>(scene));
			scene->Initialize();
		}

		/*{
			std::unique_ptr<IznNativeWindow> nativeWindow2 = nativeWindowFactory.CreateWindowInstance(L"Zenon Engine", windowWidth, windowHeight);

			const auto& renderWindow2 = renderDevice.GetObjectsFactory().CreateRenderWindow(std::move(nativeWindow2), false);
			app.AddRenderWindow(renderWindow2);

			std::shared_ptr<IScene> scene2 = MakeShared(CSceneDefault, *BaseManager, *renderWindow2);
			renderWindow2->SetRenderWindowEventListener(std::dynamic_pointer_cast<IRenderWindowEventListener>(scene2));
			renderWindow2->SetNativeWindowEventListener(std::dynamic_pointer_cast<IznNativeWindowEventListener>(scene2));
			scene2->Initialize();
		}*/
	}

	app.Run();

	app.Stop();
}


int main(int argumentCount, char* arguments[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	BT_InstallSehFilter();
	BT_SetAppName(L"ZenonEngine");
	//BT_SetSupportEMail(L"your@email.com");
	BT_SetFlags(BTF_DETAILEDMODE /*| BTF_EDITMAIL*/);
	//BT_SetSupportServer(L"localhost", 9999);
	//BT_SetSupportURL(L"http://www.your-web-site.com");

	main_internal(argumentCount, arguments);

	return 0;
}