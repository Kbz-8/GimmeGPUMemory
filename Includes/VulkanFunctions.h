#ifndef GGM_VULKAN_FUNCTIONS_H
#define GGM_VULKAN_FUNCTIONS_H

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

namespace Ggm
{
	struct VulkanFunctions
	{
		PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
		PFN_vkAllocateMemory vkAllocateMemory;
		PFN_vkFreeMemory vkFreeMemory;
		PFN_vkMapMemory vkMapMemory;
	};
}

#endif
