#include "SandboxApp.h"
#include "EntitySystem/Camera/Camera.h"
#include "Rendering/RenderResourceHelper.h"
#include "ResourceSystem/Resources.h"
#include "Core/GDI/inc_gfx.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"

using namespace Dawn;

SandboxApp::SandboxApp(AppSettings& InSettings)
	: Application(InSettings)
	 
{
}


SandboxApp::~SandboxApp()
{
}

// note-- these will be removed once we get a better structure into the "game"
Camera* g_Camera;
GfxRenderBuffer* g_RenderBuffer;
GfxVertexArray* g_ScreenQuadVAO;

void SandboxApp::Load()
{
	RenderResourceHelper::LoadCommonShaders(ResourceSystem.get());

	g_Camera = CreateCamera(GetWorld().get(),
		"Cam0",
		Settings.Width,
		Settings.Height,
		0.1f, 10000.0f, 65.0f,
		vec4(0.4f, 0.6f, 0.9f, 1.0f),
		vec3(0, 3, 10)
	);

	CameraUtils::CalculatePerspective(g_Camera);

	auto Cam1 = CreateCamera(GetWorld().get(),
		"Cam1",
		Settings.Width,
		Settings.Height,
		0.0f, 10000.0f, 65.0f,
		vec4(0.4f, 0.6f, 0.9f, 1.0f),
		vec3(0, 0, 0),
		quat(1, 0, 0, 0)
	);

	Cam1->bIsOrthographic = true;
	CameraUtils::CalculateOthographic(Cam1);

	auto Id = ResourceSystem->LoadFile("Textures/grid.png");
	if (auto GridImage = ResourceSystem->FindImage(Id))
	{
		GDI->GetPrimitiveHelper()->AllocateBuffers
		(
			GridImage,
			ResourceSystem->FindShader(CommonShaderHandles::DebugPrim),
			ResourceSystem->FindShader(EditorShaderHandles::Grid)
		);
	}

	auto IdBuffer = GDI->CreateRenderBuffer(&g_RenderBuffer);
	if (IdBuffer.IsValid)
	{
		g_RenderBuffer->AttachColorTarget(0, Settings.Width, Settings.Height);
		g_RenderBuffer->AttachDepthStencilTarget(Settings.Width, Settings.Height);
	}

	g_ScreenQuadVAO = GfxPrimitiveFactory::AllocateQuad(GDI.get(), vec2(1.0f, -1.0f), 1.0f);

	ImGuiWrapper::Create(this->Settings.Hwnd, this->GetGDI().get());
}

void SandboxApp::Resize(int InWidth, int InHeight)
{
	Application::Resize(InWidth, InHeight);

	if (g_RenderBuffer)
	{
		g_RenderBuffer->AttachColorTarget(0, InWidth, InHeight);
		g_RenderBuffer->AttachDepthStencilTarget(InWidth, InHeight);
	}
}

void SandboxApp::Update(float InDeltaTime)
{
	++Application::FrameCount;

	for (Layer* layer : Layers)
	{
		layer->Update(InDeltaTime);
	}
}

void SandboxApp::Render()
{

	// Render to "G-Buffer"
	{
		g_RenderBuffer->Bind();
		GDI->SetViewport(0, 0, Settings.Width, Settings.Height);
		GDI->SetClearColor(g_Camera->ClearColor);
		GDI->Clear();

		for (Layer* layer : Layers)
		{
			layer->Render();
		}

		g_RenderBuffer->Unbind();
	}

	// Final Pass Render Targets to Backbuffer!
	{
		GDI->SetClearColor(g_Camera->ClearColor);
		GDI->Clear();

		auto cam = GetWorld()->GetCamera(1);

		auto FinalPassShader = GDI->GetShader(ResourceSystem->FindShader(CommonShaderHandles::FinalPass)->ResourceId);
		if (FinalPassShader)
		{
			FinalPassShader->Bind();
			GDI->ActivateTextureSlot(0);
			g_RenderBuffer->BindColorTarget(0);
			FinalPassShader->SetInt("screenTexture", 0);
			GDI->DrawIndexed(g_ScreenQuadVAO->GetId());
			g_RenderBuffer->UnbindColorTarget(0);
			FinalPassShader->Unbind();
		}
	}

	// Render IMGUI!
	{
		ImGuiWrapper::BeginNewFrame();
		RenderEditorUI();
		ImGuiWrapper::Render();

	}

	GDI->Present();
}

void SandboxApp::Cleanup()
{
	ImGuiWrapper::Shutdown();
}