#include <vector>
#include <cstdint>
#include <optional>
#include <iostream>

#define VOLK_IMPLEMENTATION
#include "volk.h"

#include "../Includes/DeviceAllocator.h"

int main()
{
	volkInitialize();

	VkInstanceCreateInfo instance_create_info{};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	#if defined(__APPLE__) && defined(__MACH__)
		instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	#else
		instance_create_info.flags = 0;
	#endif

	VkInstance instance;
	vkCreateInstance(&instance_create_info, nullptr, &instance);
	volkLoadInstance(instance);

	std::uint32_t device_count;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

	VkPhysicalDevice physical = devices.back(); // Pick a random device

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	VkDevice device;
	vkCreateDevice(physical, &device_create_info, nullptr, &device);

	volkLoadDevice(device);

	Ggm::DeviceAllocator allocator;
	allocator.AttachToDevice(device, physical, {
		.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
		.vkAllocateMemory = vkAllocateMemory,
		.vkFreeMemory = vkFreeMemory,
		.vkMapMemory = vkMapMemory,
	});

	auto find_memory_type = [=](std::uint32_t type_filter, VkMemoryPropertyFlags properties) -> std::optional<std::uint32_t> {
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(physical, &mem_properties);

		for(std::uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		return std::nullopt;
	};

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = 4096;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer buffer;
	vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer);

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

	Ggm::MemoryBlock block = allocator.Allocate(mem_requirements.size, mem_requirements.size, *find_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

	std::cout   << "Memory block 1: " << &block << '\n'
				<< "    Memory: " << block.memory << '\n'
				<< "    Offset: " << block.offset << '\n'
				<< "    Size: " << block.size << std::endl;

	Ggm::MemoryBlock block2 = allocator.Allocate(mem_requirements.size, mem_requirements.size, *find_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	std::cout   << "Memory block 2: " << &block2 << '\n'
				<< "    Memory: " << block2.memory << '\n'
				<< "    Offset: " << block2.offset << '\n'
				<< "    Size: " << block2.size << std::endl;

	vkBindBufferMemory(device, buffer, block.memory, block.offset);
	vkDestroyBuffer(device, buffer, nullptr);

	allocator.DetachFromDevice(); // Will free the alloacted block

	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);

	return 0;
}
