//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
#include "stdafx.h"
#include "RTUVKGraphicsDevice.h"
#include "VkUtil.h"

#define APP_SHORT_NAME "RTU_GAME"

void RTUVKGraphicsDevice::Init()
{
	uint32_t extension_count = 0;
	ThrowIfFailed(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL));
	VkBool32 surfaceExtFound = 0;
	VkBool32 platformSurfaceExtFound = 0;

	const char* extension_names[64] = {};
	uint32_t enabled_extension_count = 0;
	if (extension_count > 0) {
		std::vector<VkExtensionProperties>instance_extensions(extension_count);
		ThrowIfFailed(vkEnumerateInstanceExtensionProperties(NULL, &extension_count, instance_extensions.data()));
		for (uint32_t i = 0; i < extension_count; i++) {
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				surfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}


			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
			if (!strcmp(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				extension_names[enabled_extension_count++] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
			}
			assert(enabled_extension_count < 64);
		}
	}

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = APP_SHORT_NAME;
	app_info.applicationVersion = 1;
	app_info.pEngineName = APP_SHORT_NAME;
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = NULL;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = enabled_extension_count;
	inst_info.ppEnabledExtensionNames = extension_names;
	inst_info.enabledLayerCount = 0;
	inst_info.ppEnabledLayerNames = NULL;

	ThrowIfFailed(vkCreateInstance(&inst_info, NULL, &m_instance));
	std::vector<VkPhysicalDevice> devices;
	uint32_t gpu_count = 0;
	ThrowIfFailed(vkEnumeratePhysicalDevices(m_instance, &gpu_count, nullptr));
	devices.resize(gpu_count);
	ThrowIfFailed(vkEnumeratePhysicalDevices(m_instance, &gpu_count, devices.data()));

	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &m_family_count, NULL);
	std::vector <VkQueueFamilyProperties> queueProps(m_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &m_family_count, queueProps.data());
	assert(m_family_count);

	VkDeviceQueueCreateInfo queue_info = {};

	bool found = false;
	for (unsigned int i = 0; i < m_family_count; i++) {
		if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queue_info.queueFamilyIndex = i;
			found = true;
			break;
		}
	}
	assert(found);

	float queue_priorities[1] = { 0.0 };
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = NULL;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = queue_priorities;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledExtensionNames = NULL;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.pEnabledFeatures = NULL;

	ThrowIfFailed(vkCreateDevice(devices[0], &device_info, NULL, &m_device));

	VkCommandPoolCreateInfo cmd_pool_info = {};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = NULL;
	cmd_pool_info.queueFamilyIndex = queue_info.queueFamilyIndex;
	cmd_pool_info.flags = 0;

	ThrowIfFailed(vkCreateCommandPool(m_device, &cmd_pool_info, NULL, &m_commandPool));
}

void RTUVKGraphicsDevice::CreateWindowSizeDependentResources(int width, int height, DXGI_FORMAT swapChainFormat)
{
}

void RTUVKGraphicsDevice::Present(bool vsync)
{
}

void RTUVKGraphicsDevice::BeginCommandAllocator()
{
}

void RTUVKGraphicsDevice::WaitForGpu()
{
}

void RTUVKGraphicsDevice::Next()
{
}

void RTUVKGraphicsDevice::SetupSwapChain(HWND hwnd, HINSTANCE hinstance, int width, int height, DXGI_FORMAT swapChainFormat)
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = NULL;
	createInfo.hinstance = hinstance;
	createInfo.hwnd = hwnd;
	ThrowIfFailed(vkCreateWin32SurfaceKHR(m_instance, &createInfo, NULL, &m_surface));
	std::vector<VkBool32> supportsPresent();

}

void RTUVKGraphicsDevice::InitRootSignaturePass1(IRTURootSignature* rootSignature)
{
}

void RTUVKGraphicsDevice::InitPipelineState(RTUPipelineStateLoader* data, IRTUPipelineState* pipelineState)
{
}

void RTUVKGraphicsDevice::InitBuffer(IRTUCBuffer* buffer, uint32_t size)
{
}

void RTUVKGraphicsDevice::InitBuffer(IRTUSBuffer* buffer, uint32_t size)
{
}

void RTUVKGraphicsDevice::InitRenderTexture2D(IRTURenderTexture2D* texture, int width, int height)
{
}

void RTUVKGraphicsDevice::RemoveMesh(IRTUMesh* mesh)
{
}

void RTUVKGraphicsDevice::Uninit()
{
}

RTUVKGraphicsDevice::~RTUVKGraphicsDevice()
{
	vkDestroyCommandPool(m_device, m_commandPool, NULL);
	vkDestroyDevice(m_device, NULL);
	vkDestroyInstance(m_instance, nullptr);
}
