#pragma once
#include "Core/Container/HandleObjectArray.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxBuffer;
	class GfxVertexArrayObject;
	class GfxShader;
	class GfxTexture;
	class GfxTextureView;
	class GfxPipelineStateObject;
	class GfxInputLayout;
	class GfxSampler;
	//class GfxImmediatePrimitives;

	typedef HandleObjectSlot<GfxBuffer> BufferSlot;
	typedef HandleObjectSlot<GfxVertexArrayObject> VertexArrayObjectSlot;
	typedef HandleObjectSlot<GfxShader> ShaderSlot;
	typedef HandleObjectSlot<GfxTexture> TextureSlot;
	typedef HandleObjectSlot<GfxTextureView> TextureViewSlot;
	typedef HandleObjectSlot<GfxPipelineStateObject> PipelineStateObjectSlot;
	typedef HandleObjectSlot<GfxSampler> SamplerSlot;

	class DAWN_API GfxGDI : public std::enable_shared_from_this<GfxGDI>
	{
	public:
		static GfxGDI* Create();

		virtual bool Init(const AppSettings& InSettings) = 0;

		virtual void Present() = 0;
		virtual void Shutdown();
		virtual void Resize(i32 InWidth, i32 InHeight) = 0;

	public:
		virtual void ActivateTextureSlot(u32 InIndex) = 0;
		virtual void DrawIndexed(const GfxResId& InVertexArrayId) = 0;
		virtual void DrawArray(const GfxResId& VertexArrayId) = 0;
		virtual void DrawInstanced(const GfxResId& VertexArrayId, u32 InAmount) = 0;
		void SetClearColor(const vec4& InColor);
		virtual void SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom) = 0;
		virtual void ClearWithColor(const vec4& InColor) = 0;
		virtual void Clear() = 0;

		virtual void SetPipelineState(const GfxResId& InId) = 0;
		virtual void SetVAO(const GfxResId& InId) = 0;
		virtual void CommitShaderResources(const GfxResId& InPSOId) = 0;
		virtual void BindPipelineShaders() = 0;
		virtual void UpdateConstantBuffer(const GfxResId& InBufferId, void* InData, i32 InSize) = 0;

	public:
		virtual GfxResId CreateBuffer(const GfxBufferDesc& InDesc, const GfxBufferData& InData, GfxBuffer** OutBuffer) = 0;
		virtual GfxResId CreateVertexArrayObject(GfxVertexArrayObject** OutBuffer) = 0;
		virtual GfxResId CreateShader(const GfxShaderDesc& InDesc, const GfxShaderData& InData, GfxShader** OutShader) = 0;
		virtual GfxResId CreateTexture(const GfxTextureDesc& InDesc, const GfxTextureData& InData, GfxTexture** OutTexture) = 0;
		virtual GfxResId CreateTextureView(const GfxTextureViewDesc& InDesc, GfxTextureView** OutTexture) = 0;
		virtual GfxResId CreateSampler(const GfxSamplerDesc& InDesc, GfxSampler** OutSampler) = 0;
		virtual GfxResId CreatePSO(const GfxPipelineStateObjectDesc& InDesc, GfxPipelineStateObject** OutSampler) = 0;

		inline GfxBuffer* GetBuffer(const GfxResId& InId) const
		{
			return BufferPool.Find(InId);
		}

		inline GfxVertexArrayObject* GetVertexArrayObject(const GfxResId& InId) const
		{
			return VertexArrayObjectPool.Find(InId);
		}

		inline GfxShader* GetShader(const GfxResId& InId) const
		{
			return ShaderPool.Find(InId);
		}

		inline GfxTexture* GetTexture(const GfxResId& InId) const
		{
			return TexturePool.Find(InId);
		}

		inline GfxTextureView* GetTextureView(const GfxResId& InId) const
		{
			return TextureViewPool.Find(InId);
		}

		inline GfxPipelineStateObject* GetPipelineState(const GfxResId& InId) const
		{
			return PipelineStatePool.Find(InId);
		}

		inline GfxSampler* GetSampler(const GfxResId& InId) const
		{
			return SamplerPool.Find(InId);
		}

		/*inline GfxImmediatePrimitives* GetPrimitiveHelper()
		{
			return Primitives.get();
		}*/

		void ReturnTextureView(const GfxResId& InId);
		void ReturnBuffer(const GfxResId& InId);
		void ReturnTexture(const GfxResId& InId);
		void ReturnVertexArrayObject(const GfxResId& InId);
		void ReturnShader(const GfxResId& InId);
		void ReturnPipelineState(const GfxResId& InId);
	protected:
		//Unique<GfxImmediatePrimitives> Primitives;
		HandleObjectArray<GfxBuffer> BufferPool;
		HandleObjectArray<GfxVertexArrayObject> VertexArrayObjectPool;
		HandleObjectArray<GfxShader> ShaderPool;
		HandleObjectArray<GfxTexture> TexturePool;
		HandleObjectArray<GfxTextureView> TextureViewPool;
		HandleObjectArray<GfxPipelineStateObject> PipelineStatePool;
		HandleObjectArray<GfxSampler> SamplerPool;

		vec4 ClearColor = vec4(1, 0, 0, 1);
	};

	typedef std::shared_ptr<GfxGDI> GfxGDIPtr;
}