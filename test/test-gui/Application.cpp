//////////////////////////////////////////////////////////////////////
/// Desc  Application
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "Application.hpp"
#include "debug/LogTool.hpp"

#include <cstdlib>
#include <cassert>

#include <thread>
#include <chrono>

Application *gApp = nullptr;

static void errorCallback(int error, const char *description)
{
    if(gApp != nullptr)
    {
        gApp->onError(error, description);
    }

    volatile int a = 1;
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(window == gApp->getWindow())
    {
        gApp->onKey(key, scancode, action, mods);
    }
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseButton(button, action, mods);
    }
}

static void mouseMoveCallback(GLFWwindow *window, double x, double y)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseMove(x, y);
    }
}

static void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if(window == gApp->getWindow())
    {
        gApp->onMouseScroll(xoffset, yoffset);
    }
}

static void windowSizeChangeCallback(GLFWwindow *window, int width, int height)
{
	if (window == gApp->getWindow())
	{
		gApp->onSizeChange(width, height);
	}
}

static void frameBufferSizeChangeCallback(GLFWwindow *window, int width, int height)
{
    if(window == gApp->getWindow())
    {
        gApp->onFrameBufferSizeChange(width, height);
    }
}

static void charCallback(GLFWwindow* window, unsigned int ch)
{
	if (window == gApp->getWindow())
	{
		gApp->onChar(ch);
	}
}

Application::Application()
: pWindow_(nullptr)
, deltaTime_(0.0f)
, lockFrameTime_(-1.0f)
{
    gApp = this;
    
    glfwSetErrorCallback(errorCallback);
    if(!glfwInit())
    {
        exit(0);
    }
}

Application::~Application()
{
    if(pWindow_ != nullptr)
    {
        onDestroy();
        glfwDestroyWindow(pWindow_);
    }
   
    glfwTerminate();
    if(gApp == this)
    {
        gApp = nullptr;
    }
}

bool Application::createWindow(int width, int height, const std::string &title)
{
    assert(pWindow_ == nullptr);
   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(nullptr == pWindow_)
    {
        return false;
    }
    
    makeCurrent();

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed initialize OpenGL.");
        return false;
    }
	LOG_INFO("GL Version: %d.%d", GLVersion.major, GLVersion.minor);
    if(GLVersion.major < 3)
    {
        LOG_ERROR("Unsupported OpenGL version.");
        return false;
    }
    
    glfwSetKeyCallback(pWindow_, keyCallback);
    glfwSetMouseButtonCallback(pWindow_, mouseButtonCallback);
    glfwSetScrollCallback(pWindow_, mouseScrollCallback);
    glfwSetCursorPosCallback(pWindow_, mouseMoveCallback);
	glfwSetWindowSizeCallback(pWindow_, windowSizeChangeCallback);
    glfwSetFramebufferSizeCallback(pWindow_, frameBufferSizeChangeCallback);
	glfwSetCharCallback(pWindow_, charCallback);
    
    // 开启垂直同步
    glfwSwapInterval(1);
    // 屏幕清成蓝色
    glClearColor(0.15f, 0.24f, 0.24f, 0.0f);
	glClearStencil(0);
	glClearDepth(1.0f);
    
	glEnable(GL_DEPTH_TEST);

    if (!onCreate())
    {
        return false;
    }
    
    onSizeChange(width, height);

    return true;
}

void Application::makeCurrent()
{
    glfwMakeContextCurrent(pWindow_);
}

float Application::getAspect() const
{
    int width, height;
    glfwGetWindowSize(pWindow_, &width, &height);
    return float(width) / float(height);
}

Vector2f Application::getWindowSize()
{
	int width, height;
	glfwGetWindowSize(pWindow_, &width, &height);
	return Vector2f(float(width), float(height));
}

Vector2f Application::getFrameBufferSize()
{
    int width, height;
    glfwGetFramebufferSize(pWindow_, &width, &height);
    return Vector2f(float(width), float(height));
}

Vector2f Application::getCursorPos()
{
	double x, y;
	glfwGetCursorPos(pWindow_, &x, &y);
	return Vector2f(float(x), float(y));
}

void Application::mainLoop()
{
	double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(pWindow_))
    {
		double curTime = glfwGetTime();
		deltaTime_ = float(curTime - lastTime);
        if (lockFrameTime_ > 0.f && deltaTime_ < lockFrameTime_)
        {
            float sleepTime = lockFrameTime_ - deltaTime_;
            std::this_thread::sleep_for(std::chrono::milliseconds(int(sleepTime * 1000)));

            curTime = glfwGetTime();
            deltaTime_ = float(curTime - lastTime);
        }
		lastTime = curTime;

		onTick(deltaTime_);
        onDraw();

        glfwSwapBuffers(pWindow_);
        glfwPollEvents();
    }
}

void Application::onTick(float elapse)
{

}

void Application::onDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Application::onCreate()
{
	return true;
}

void Application::onDestroy()
{
    
}

void Application::onKey(int key, int scancode, int action, int modes)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(pWindow_, GLFW_TRUE);
}

void Application::onError(int error, const char *description)
{
    fprintf(stderr, "Error(%d): %s\n", error, description);
}

void Application::onSizeChange(int width, int height)
{
}

void Application::onFrameBufferSizeChange(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Application::onMouseButton(int button, int action, int mods)
{
    
}

void Application::onMouseMove(double x, double y)
{
    
}

void Application::onMouseScroll(double xoffset, double yoffset)
{
    
}

void Application::onChar(uint32_t ch)
{

}
