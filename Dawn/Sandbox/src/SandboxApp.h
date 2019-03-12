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
		L"Test", 
		1280,
		720,
		false,
		false
	};

	return new SandboxApp(Settings);
}