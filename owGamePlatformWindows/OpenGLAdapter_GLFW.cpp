#include "stdafx.h"

// General
#include "OpenGLAdapter_GLFW.h"

// Error callback
void GLFWErrorCallback(int error, const char* description)
{
	Log::Error("GLFW[]: Error [%d] (%s).", error, description);
}

// Resize callback
void GLFWFramebufferCallback(GLFWwindow* _window, int _width, int _height)
{
	//_Engine->GetAdapter()->SetWindowSize(_width, _height);
}

Input* _input = nullptr;

// Input callbacks
void GLFWMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	_input->MousePositionCallback(vec2(static_cast<int>(xpos), static_cast<int>(ypos)));
}
void GLFWMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	_input->MouseCallback(button, action, mods);
}
void GLFWMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	_input->MouseScrollCallback(static_cast<int>(yoffset));
}
void GLFWKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	_input->KeyboardCallback(key, scancode, action, mods);
}
void GLFWCharCallback(GLFWwindow* window, unsigned int _char)
{
	_input->CharCallback(_char);
}

//---------------------------------------------------------

bool OpenGLAdapter_GLFW::Init()
{
	glfwSetErrorCallback(GLFWErrorCallback);

	if (!glfwInit())
	{
		Log::Error("GLFW[]: Error while init GLFW!");
		return false;
	}
	Log::Print("GLFW[]: Version [%s].", glfwGetVersionString());

	// Set window options

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Monitor

	primaryMonitor = glfwGetPrimaryMonitor();
	//const char* primaryMonitorName = glfwGetMonitorName(primaryMonitor);
	//Log::Print("GLFW[]: Primary monitor name [%s]", primaryMonitorName);

	// Videomode contain settings

	const GLFWvidmode* primaryMonitorMode = glfwGetVideoMode(primaryMonitor);
	Log::Print("GLFW[]: Primary monitor resolution [%d, %d]", primaryMonitorMode->width, primaryMonitorMode->height);

	// Physical size in mm
	//int32 primaryMonitorPhysicalWidth, primaryMonitorPhysicalHeight;
	//glfwGetMonitorPhysicalSize(primaryMonitor, &primaryMonitorPhysicalWidth, &primaryMonitorPhysicalHeight);
	//Log::Print("GLFW[]: Primary monitor physical size [%d, %d]", primaryMonitorPhysicalWidth, primaryMonitorPhysicalHeight);

	// Create GLFW window

	window = glfwCreateWindow(_Config.windowSizeX, _Config.windowSizeY, "Default window title.", nullptr, nullptr);

	// Move window to center
	//uint32 windowPositionX = (primaryMonitorMode->width / 2) - (_Config.windowSizeX / 2);
	//uint32 windowPositionY = (primaryMonitorMode->height / 2) - (_Config.windowSizeY / 2);
	//glfwSetWindowPos(window, windowPositionX, windowPositionY);
	//Log::Print("GLFW[]: Window position [%d, %d]", windowPositionX, windowPositionY);

	//nativeWindow = glfwGetWin32Window(window);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Time
	glfwSetTime(0.0);

	// Resize callback
	glfwSetFramebufferSizeCallback(window, GLFWFramebufferCallback);

	// Input
	m_Input = new Input();
	_input = m_Input;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 0);
	glfwSetMouseButtonCallback(window, GLFWMouseCallback);
	glfwSetCursorPosCallback(window, GLFWMousePositionCallback);
	glfwSetScrollCallback(window, GLFWMouseScrollCallback);
	glfwSetKeyCallback(window, GLFWKeyboardCallback);
	glfwSetCharCallback(window, GLFWCharCallback);

	return true;
}

void OpenGLAdapter_GLFW::Destroy()
{
	glfwTerminate();
}

bool OpenGLAdapter_GLFW::SwapWindowBuffers()
{
	glfwSwapBuffers(window);
	glfwPollEvents();

	return !glfwWindowShouldClose(window);
}

double OpenGLAdapter_GLFW::GetTime()
{
	return glfwGetTime();
}

Input* OpenGLAdapter_GLFW::GetInput()
{
	return m_Input;
}

//

void OpenGLAdapter_GLFW::SetWindowSize(int32 _width, int32 _height)
{
	glfwSetWindowSize(window, _width, _height);
	//_Render->OnWindowResized(_width, _height);
}

void OpenGLAdapter_GLFW::SetWindowTitle(cstring _title)
{
	glfwSetWindowTitle(window, _title.c_str());
}

void OpenGLAdapter_GLFW::ShowCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OpenGLAdapter_GLFW::HideCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void OpenGLAdapter_GLFW::SetMousePosition(cvec2 _mousePosition)
{
	glfwSetCursorPos(window, static_cast<double>(_mousePosition.x), static_cast<double>(_mousePosition.y));
}