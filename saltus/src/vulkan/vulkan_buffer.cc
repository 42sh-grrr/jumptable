#include "saltus/vulkan/vulkan_buffer.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanBuffer::VulkanBuffer(
        std::shared_ptr<VulkanDevice> device,
            size_t size, VkBufferUsageFlags usage
    ): device_(device), size_(size)
    {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.size = size;
        create_info.usage = usage;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result =
            vkCreateBuffer(*device, &create_info, nullptr, &buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create buffer");
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vkDestroyBuffer(*device_, buffer_, nullptr);
        if (memory_ != VK_NULL_HANDLE)
            vkFreeMemory(*device_, memory_, nullptr);
    }

    VulkanBuffer::operator VkBuffer() const
    {
        return buffer_;
    }

    VkBuffer VulkanBuffer::buffer() const
    {
        return buffer_;
    }

    VkDeviceMemory VulkanBuffer::memory() const
    {
        return memory_;
    }

    bool VulkanBuffer::is_allocated() const
    {
        return memory_ != VK_NULL_HANDLE;
    }

    void VulkanBuffer::alloc()
    {
        if (memory_ != VK_NULL_HANDLE)
            throw std::runtime_error("Buffer already allocated");

        VkMemoryRequirements mem_reqs;
        vkGetBufferMemoryRequirements(*device_, buffer_, &mem_reqs);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_reqs.size;
        alloc_info.memoryTypeIndex = find_mem_type(
            mem_reqs.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        VkResult result =
            vkAllocateMemory(*device_, &alloc_info, nullptr, &memory_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Buffer allocation failed");

        vkBindBufferMemory(*device_, buffer_, memory_, 0);
    }

    void *VulkanBuffer::map()
    {
        if (memory_ == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot map non allocated buffer");

        void *data = nullptr;
        VkResult result = vkMapMemory(*device_, memory_, 0, VK_WHOLE_SIZE, 0, &data);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Buffer map error");

        return data;
    }

    void VulkanBuffer::unmap()
    {
        if (memory_ == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot unmap non allocated buffer");

        vkUnmapMemory(*device_, memory_);
    }

    uint32_t VulkanBuffer::find_mem_type(
        uint32_t type_filter,
        VkMemoryPropertyFlags properties
    ) {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(device_->physical_device(), &mem_properties);

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if (
                (type_filter & (1<<i)) &&
                (mem_properties.memoryTypes[i].propertyFlags & properties) == properties
            ) {
                return i;
            }
        }

        throw std::runtime_error("Could not find suitable heap type");
    }
}
