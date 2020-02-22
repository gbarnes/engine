#include "stdafx.h"
#include "GfxVulkanGDI.h"
/*#include "Core/Logging/Log.h"

namespace Dawn
{
	bool GfxVulkanGDI::Init(const AppSettings& InSettings)
	{
		/*VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = std::string(InSettings.Title.begin(), InSettings.Title.end()).c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

	
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.enabledExtensionCount = 0;
		
		VkResult result = vkCreateInstance(&createInfo, NULL, &Instance);
		D_ASSERT(result == VK_SUCCESS, "Couldn't create Vulkan instance!");

		// Query how many devices are present in the system
		uint32_t deviceCount = 0;
		result = vkEnumeratePhysicalDevices(Instance, &deviceCount, NULL);

		D_ASSERT(result == VK_SUCCESS, "Failed to query the number of physical devices present: %d\n");
		D_ASSERT(deviceCount != 0, "Couldn't detect any device present with Vulkan support: %d\n");

		// Get the physical devices
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		result = vkEnumeratePhysicalDevices(Instance, &deviceCount, &physicalDevices[0]);
		D_ASSERT(result == VK_SUCCESS, "Faied to enumerate physical devices present: %d\n");

		VkPhysicalDeviceProperties deviceProperties;
		for (u32 i = 0; i < deviceCount; i++) 
		{
			memset(&deviceProperties, 0, sizeof deviceProperties);
			vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
			DWN_CORE_INFO("Driver Version: {0}", deviceProperties.driverVersion);
			DWN_CORE_INFO("Device Name:    {0}", deviceProperties.deviceName);
			DWN_CORE_INFO("Device Type:    {0}", deviceProperties.deviceType);
			DWN_CORE_INFO("API Version:    {0}.{1}.{2}",
				// See note below regarding this:
				(deviceProperties.apiVersion >> 22) & 0x3FF,
				(deviceProperties.apiVersion >> 12) & 0x3FF,
				(deviceProperties.apiVersion & 0xFFF));
		}
		
		return false;
	}


	void GfxVulkanGDI::Present()
	{

	}

	void GfxVulkanGDI::Shutdown()
	{
		//if(Instance)
			//vkDestroyInstance(Instance, nullptr);
	}

	void GfxVulkanGDI::ActivateTextureSlot(u32 InIndex)
	{

	}

	void GfxVulkanGDI::DrawIndexed(GfxResId VertexArrayId)
	{


	}

	void GfxVulkanGDI::DrawArray(GfxResId VertexArrayId)
	{

	}

	void GfxVulkanGDI::DrawInstanced(GfxResId VertexArrayId, u32 InAmount)
	{

	}

	void GfxVulkanGDI::Clear()
	{

	}
		
	void GfxVulkanGDI::ClearWithColor(const vec4& InColor)
	{

	}

	void GfxVulkanGDI::SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom)
	{

	}

	void GfxVulkanGDI::SetState(const GfxState& InState)
	{

	}

	GfxResId GfxVulkanGDI::CreateVertexBuffer(void* Vertices, u32 Size, GfxVertexBuffer** OutBuffer)
	{
		return INVALID_HANDLE;
	}

	GfxResId GfxVulkanGDI::CreateIndexBuffer(u32* Indices, u32 Size, GfxIndexBuffer** OutBuffer)
	{
		return INVALID_HANDLE;
	}

	GfxResId GfxVulkanGDI::CreateVertexArray(GfxVertexArray** OutBuffer)
	{
		return INVALID_HANDLE;
	}

	GfxResId GfxVulkanGDI::CreateShader(GfxShader** OutShader)
	{
		return INVALID_HANDLE;
	}

	GfxResId GfxVulkanGDI::CreateTexture(const GfxTextureDesc& InDesc, GfxTexture** OutTexture)
	{
		return INVALID_HANDLE;
	}
	
	GfxResId GfxVulkanGDI::CreateRenderBuffer(GfxRenderBuffer** OutTexture)
	{
		return INVALID_HANDLE;
	}
}*/