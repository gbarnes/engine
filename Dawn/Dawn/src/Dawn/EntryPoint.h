#pragma once
#include "Application.h"
#include "Core/Logging/Log.h"

extern Dawn::Application* Dawn::CreateApplication();

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	using namespace Dawn;

	Log::Init();
	DWN_CORE_INFO("Initialized Log!");

	auto app = Dawn::CreateApplication();
	Dawn::g_Application = Shared<Application>(app);
	app->Run();

	return 0;
}
