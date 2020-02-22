#pragma once
#include "Core/GDI/Base/GfxGDI.h"
#include "inc_dx11.h"

namespace Dawn
{
	class DX11PipelineStateObject;

	class DAWN_API DX11GDI : public GfxGDI
	{
	public:
		virtual bool Init(const AppSettings& InSettings) override;
		virtual void Present() override;
		virtual void Shutdown() override;
		virtual void Resize(i32 InWidth, i32 InHeight)  override;
	public:
		virtual void ActivateTextureSlot(u32 InIndex) override;
		virtual void DrawIndexed(const GfxResId& InVertexArrayId) override;
		virtual void DrawArray(const GfxResId& VertexArrayId) override;
		virtual void DrawInstanced(const GfxResId& VertexArrayId, u32 InAmount) override;
		virtual void SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom) override;
		virtual void ClearWithColor(const vec4& InColor) override;
		virtual void Clear() override;

		virtual void SetPipelineState(const GfxResId& InId) override;
		virtual void SetVAO(const GfxResId& InId) override;
		virtual void BindPipelineShaders() override;
		virtual void CommitShaderResources(const GfxResId& InPSOId) override;
	public:
		virtual GfxResId CreateBuffer(const GfxBufferDesc& InDesc, const GfxBufferData& InData, GfxBuffer** OutBuffer) override;
		virtual GfxResId CreateVertexArrayObject(GfxVertexArrayObject** OutBuffer) override;
		virtual GfxResId CreateShader(const GfxShaderDesc& InDesc, const GfxShaderData& InData, GfxShader** OutShader) override;
		virtual GfxResId CreateTexture(const GfxTextureDesc& InDesc, const GfxTextureData& InData, GfxTexture** OutTexture) override;
		virtual GfxResId CreateTextureView(const GfxTextureViewDesc& InDesc, GfxTextureView** OutTexture) override;
		virtual GfxResId CreateSampler(const GfxSamplerDesc& InDesc, GfxSampler** OutTexture) override;
		virtual GfxResId CreatePSO(const GfxPipelineStateObjectDesc& InDesc, GfxPipelineStateObject** OutSampler) override;
	//	virtual GfxResId CreateRenderBuffer(GfxRenderBuffer** OutTexture) override;

		inline ID3D11Device* GetD3D11Device() const {
			return Device.Get();
		}

		inline ID3D11DeviceContext* GetD3D11Context() const {
			return Context.Get();
		}

		inline IDXGISwapChain* GetD3D11SwapChain() const {
			return SwapChain.Get();
		}

		
	private:
		ComPtr<ID3D11Device> Device;
		ComPtr<ID3D11DeviceContext> Context;
		ComPtr<IDXGISwapChain> SwapChain;
		ComPtr<ID3D11Debug> Debug;
		GfxResId BackbufferId;
		DX11PipelineStateObject* CurrentPipelineState;

		void CreateBackbufferRT();
	};
};