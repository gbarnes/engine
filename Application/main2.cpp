
#include "Core/System/Window.h"
#include "Core\GDI\GfxDevice.h"
#include "Core\GDI\GfxParallel.h"
#include "Renderer\RenderCmd.h"
#include <thread>
#include <mutex>
#include "Core\JobSystem\JobSystem.h"
int x = 1;
//std::mutex cout2_mutex;

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


/*void empty_job2(SJob* InJob)
{
cout2_mutex.lock();
std::cout << "Hello world - Thread Index: " << CJobSystem::ThreadIndex << "\n";
cout2_mutex.unlock();
}

void empty_job(SJob* InJob)
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

/*int main()
{

//CEObjectPtr Ptr(new CEObject());

CWindow Window;
CGfxDevice GfxDevice;
//CGfxCmdList List;
//CGfxQueue;
//CGfxCmdAllocator Alloc;
//CGfxRenderTarget
//CGfxCmd

//GfxParallel::AsyncCompute(&GfxDevice, 0);

Window.Initialize(L"Test", 800, 600, false, 32, 32, 8);

if (Window.Create() != EResult_OK)
{
std::cout << "Couldn't open file!\n";
system("pause");
return 0;
}

if (GfxDevice.Initialize(800, 600, Window.GetHwnd(), false, false) != EResult_OK)
{
std::cout << "Couldn't initialize graphics device!\n";
system("pause");
return 0;
}

CJobSystem::Initialize();

unsigned int ThreadIndex = CJobSystem::ThreadIndex;

SJob* Root = CJobSystem::CreateJob(&empty_job);
for (unsigned int i = 0; i < 500; ++i)
{
SJob* Job = CJobSystem::CreateJobAsChild(Root, &empty_job);
CJobSystem::Run(Job);
}*

//CJobSystem::Run(Root);
//CJobSystem::Wait(Root);

//RenderCommand::Packet p = RenderCommand::Create<Draw>(sizeof(Draw));
//Draw* d = RenderCommand::GetCommand<Draw>(p);

//SGfxResBarrier Barrier;
//ZeroMemory(&Barrier, sizeof(SGfxResBarrier));

while (true)
{
if (Window.PeekMessages())
break;


CGfxCmdList* CmdList = GfxBackend::ObtainCmdListByIndex(0);
CGfxCmdAllocator* Allocator = GfxBackend::ObtainCmdAllocator(0);

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle;
unsigned int RenderTargetViewDescriptorSize = 0;

// Reset (re-use) the memory associated command allocator.
HRESULT Result = Allocator->Reset();
if (FAILED(Result))
continue;

Result = CmdList->Reset(Allocator, GfxDevice.GetPipelineState());
if (FAILED(Result))
continue;

// Record commands in the command list now.
// Start by setting the resource barrier.
D3D12_RESOURCE_BARRIER Barrier;
Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
Barrier.Transition.pResource = GfxDevice.GetCurrentBackbuffer();
Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
CmdList->ResourceBarrier(1, &Barrier);

ID3D12Device* Device = GfxDevice;
// Get the render target view handle for the current back buffer.
RenderTargetViewHandle = GfxDevice.GetBackbufferHeap()->GetCPUDescriptorHandleForHeapStart();
RenderTargetViewDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
if (GfxDevice.GetBufferIndex() == 1)
{
RenderTargetViewHandle.ptr += RenderTargetViewDescriptorSize;
}

CmdList->OMSetRenderTargets(1, &RenderTargetViewHandle, FALSE, NULL);

float color[4];
color[0] = 0.5;
color[1] = 0.5;
color[2] = 0.5;
color[3] = 1.0;
CmdList->ClearRenderTargetView(RenderTargetViewHandle, color, 0, NULL);

Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
CmdList->ResourceBarrier(1, &Barrier);

// Close the list of commands.
Result = CmdList->Close();
if (FAILED(Result))
{
continue;
}

ID3D12CommandList* ppCommandLists[] = { CmdList };
GfxParallel::ObtainQueue()->ExecuteCommandLists(1, ppCommandLists);

GfxDevice.Present();
GfxDevice.WaitForPreviousFrame();
}

GfxDevice.Shutdown();

return 1;
};*/

/*CGfxCmdList* CmdList = CGfxParallel::CmdLists[0];
CGfxCmdAllocator* Allocator = CGfxParallel::CmdAllocators[0];

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle;
unsigned int RenderTargetViewDescriptorSize = 0;

// Reset (re-use) the memory associated command allocator.
HRESULT Result = Allocator->Reset();
if (FAILED(Result))
continue;

Result = CmdList->Reset(Allocator, GfxDevice.GetPipelineState());
if (FAILED(Result))
continue;

// Record commands in the command list now.
// Start by setting the resource barrier.
/*Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
Barrier.Transition.pResource = GfxDevice.GetCurrentBackbuffer();
Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
CmdList->ResourceBarrier(1, &Barrier);

ID3D12Device* Device = GfxDevice;
// Get the render target view handle for the current back buffer.
RenderTargetViewHandle = GfxDevice.GetBackbufferHeap()->GetCPUDescriptorHandleForHeapStart();
RenderTargetViewDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
if (GfxDevice.GetBufferIndex() == 1)
{
RenderTargetViewHandle.ptr += RenderTargetViewDescriptorSize;
}

CmdList->OMSetRenderTargets(1, &RenderTargetViewHandle, FALSE, NULL);

float color[4];
color[0] = 0.5;
color[1] = 0.5;
color[2] = 0.5;
color[3] = 1.0;
CmdList->ClearRenderTargetView(RenderTargetViewHandle, color, 0, NULL);

Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
CmdList->ResourceBarrier(1, &Barrier);

// Close the list of commands.
Result = CmdList->Close();
if (FAILED(Result))
{
continue;
}*/

//ID3D12CommandList* ppCommandLists[] = { CmdList };
//CGfxParallel::CmdQueue->ExecuteCommandLists(1, ppCommandLists);