#include "saltus/vulkan/vulkan_buffer.hh"

#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanBuffer::VulkanBuffer(
        std::shared_ptr<VulkanDevice> device,
        BufferCreateInfo info
    ): Buffer(info), device_(device)
    {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.size = info.size;
        create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        uint32_t families[] = {
            device->get_physical_device_family_indices().graphicsFamily.value(),
            device->get_physical_device_family_indices().transferFamily.value(),
        };
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = families;

        if (info.usages.uniform)
            create_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (info.usages.index)
            create_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (info.usages.vertex)
            create_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        if (create_info.usage == 0)
            throw std::runtime_error("Cannot create a vulkan buffer with 0 usages");

        VkResult result =
            vkCreateBuffer(*device, &create_info, nullptr, &buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create buffer");

        alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (info.data)
            assign(info.data);
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

    void VulkanBuffer::assign(const uint8_t *data)
    {
        void *bdata = map();
        memcpy(bdata, data, size());
        unmap();
    }

    void VulkanBuffer::alloc(VkMemoryPropertyFlags memory_properties)
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
            memory_properties
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
