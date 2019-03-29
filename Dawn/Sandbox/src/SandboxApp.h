#pragma once
#include <Dawn.h>

class SandboxApp : public Dawn::Application
{
public:
	SandboxApp(AppSettings& InSettings);
	~SandboxApp();
};

Dawn::Application* Dawn::CreateApplication()
{
	AppSettings Settings = {
		0, // HWND will be provided by the app later on!
		L"Sandbox Game", 
		1280,
		720,
		false,
		true,
		32,
		32,
		8
	};

	return new SandboxApp(Settings);
}