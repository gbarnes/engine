#pragma once
#include "inc_core.h"
#include "Application.h"

extern Dawn::Application* Dawn::CreateApplication();

int main(int argc, char** argv)
{
	using namespace Dawn;

	Log::Init();
	DWN_CORE_INFO("Initialized Log!");

	
	ReferenceAppPtr app = ReferenceAppPtr(Dawn::CreateApplication());
	app->Run();
}
