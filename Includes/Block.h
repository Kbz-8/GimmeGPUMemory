#ifndef GGM_BLOCK_H
#define GGM_BLOCK_H

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>
#include <algorithm>

namespace Ggm
{
	class MemoryBlock
	{
		friend class MemoryChunk;

		public:
			MemoryBlock() = default;

			[[nodiscard]] inline bool operator==(const MemoryBlock& rhs) const noexcept
			{
				return  memory == rhs.memory &&
						offset == rhs.offset &&
						size == rhs.size &&
						free == rhs.free &&
						map == rhs.map;
			}

			inline void Swap(MemoryBlock& rhs) noexcept
			{
				std::swap(memory, rhs.memory);
				std::swap(offset, rhs.offset);
				std::swap(size, rhs.size);
				std::swap(map, rhs.map);
				std::swap(free, rhs.free);
			}

			~MemoryBlock() = default;

		public:
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkDeviceSize offset = 0;
			VkDeviceSize size = 0;
			void* map = nullptr; // Useless if it's a GPU allocation
		
		private:
			bool free = false;
	};
}

#endif
