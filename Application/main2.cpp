
#include "Core/System/Window.h"
#include "Core\GDI\GfxDevice.h"
#include "Core\GDI\GfxParallel.h"
#include "Renderer\RenderCmd.h"
#include <thread>
#include <mutex>
#include "Core\JobSystem\JobSystem.h"

int x = 1;
std::mutex cout2_mutex;

/*CJobSystem::Initialize();

unsigned int ThreadIndex = CJobSystem::ThreadIndex;

SJob* Root = CJobSystem::CreateJob(&empty_job);
for (unsigned int i = 0; i < 500; ++i)
{
SJob* Job = CJobSystem::CreateJobAsChild(Root, &empty_job);
CJobSystem::Run(Job);
}

CJobSystem::Run(Root);
CJobSystem::Wait(Root);*/


void empty_job2(SJob* InJob)
{
	cout2_mutex.lock();
	std::cout << "Hello world - Thread Index: " << CJobSystem::ThreadIndex << "\n";
	cout2_mutex.unlock();
}

/*void empty_job(SJob* InJob)
{
cout2_mutex.lock();
int i = 12 + 12;
std::cout << "#" << x <<": 12 + 12 = " << i << ", Thread Index: " << CJobSystem::ThreadIndex << "\n";

OutputDebugString(L"");

x++;
cout2_mutex.unlock();

/*SJob* newJob = CJobSystem::CreateJob(&empty_job2);
CJobSystem::Run(newJob);
CJobSystem::Wait(newJob);
}*/

u32 g_ClientWidth = 1280;
u32 g_ClientHeight = 720;

void Update()
{
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	if (elapsedSeconds > 1.0)
	{
		wchar_t buffer[500];
		auto fps = frameCounter / elapsedSeconds;

		swprintf(buffer, 500, L"FPS: %f\n", fps);
		OutputDebugString(buffer);

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
}

DirectX::XMFLOAT4 g_ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };

int main()
{
	CWindow Window;
	
	Window.Initialize(L"Test", g_ClientWidth, g_ClientHeight, false, 32, 32, 8);
	Window.OnWindowPaint = []()
	{
		Update();
		
		if (!GfxBackend::IsInitialized())
			return;

		GfxBackend::Reset();
		GfxBackend::ClearBackbuffer(g_ClearColor);
		GfxBackend::Present();
	};

	if (Window.Create() != EResult_OK)
	{
		std::cout << "Couldn't open window!\n";
		system("pause");
		return 0;
	}

	if (GfxBackend::Initialize(g_ClientWidth, g_ClientHeight, Window.GetHwnd(), true, false) != EResult_OK)
	{
		std::cout << "Couldn't initialize graphics device!\n";
		system("pause");
		return 0;
	}

	/*CJobSystem::Initialize();

	unsigned int ThreadIndex = CJobSystem::ThreadIndex;

	SJob* Root = CJobSystem::CreateJob(&empty_job2);
	for (unsigned int i = 0; i < 500; ++i)
	{
		SJob* Job = CJobSystem::CreateJobAsChild(Root, &empty_job2);
		CJobSystem::Run(Job);
	}

	CJobSystem::Run(Root);
	CJobSystem::Wait(Root);*/

	//RenderCommand::Packet p = RenderCommand::Create<Draw>(sizeof(Draw));
	//Draw* d = RenderCommand::GetCommand<Draw>(p);

	//SGfxResBarrier Barrier;
	//ZeroMemory(&Barrier, sizeof(SGfxResBarrier));

	while (true)
	{
		if (Window.PeekMessages())
			break;
	}

	//CJobSystem::Shutdown();
	GfxBackend::Shutdown();

	return 1;
};