#pragma once

#include <Dawn.h>

class SandboxApp : public Dawn::Application
{
public:
	SandboxApp(Dawn::SAppSettings& InSettings);
	~SandboxApp();
};

Dawn::Application* Dawn::CreateApplication()
{
	Dawn::SAppSettings Settings = {
		L"Sandbox Game", 
		1280,
		720,
		false,
		true
	};

	return new SandboxApp(Settings);
}