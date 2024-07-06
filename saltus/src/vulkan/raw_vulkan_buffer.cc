#include "saltus/vulkan/raw_vulkan_buffer.hh"

namespace saltus::vulkan
{
    RawVulkanBuffer::RawVulkanBuffer(
        std::shared_ptr<VulkanDevice> device,
        VkDeviceSize size,
        VkBufferUsageFlags usage_flags
    ): device_(device) {
        VkBufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.size = size;
        create_info.usage = usage_flags;
        uint32_t families[] = {
            device->get_physical_device_family_indices().graphicsFamily.value(),
            device->get_physical_device_family_indices().transferFamily.value(),
        };
        if (families[0] == families[1])
        {
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 1;
            create_info.pQueueFamilyIndices = families;
        }
        else {
            create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = families;
        }

        VkResult result =
            vkCreateBuffer(*device, &create_info, nullptr, &buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create buffer");
    }
    RawVulkanBuffer::~RawVulkanBuffer()
    {
        
    }

    RawVulkanBuffer::operator VkBuffer() const
    {
        return buffer_;
    }
    
    VkBuffer RawVulkanBuffer::buffer_handle() const
    {
        return buffer_;
    }

    void RawVulkanBuffer::alloc(VkMemoryPropertyFlags memory_properties)
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

    void *RawVulkanBuffer::map(VkDeviceSize offset, VkDeviceSize size)
    {
        if (memory_ == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot map non allocated buffer");

        void *data = nullptr;
        VkResult result = vkMapMemory(*device_, memory_, offset, size, 0, &data);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Buffer map error");

        return data;
    }

    void RawVulkanBuffer::unmap()
    {
        if (memory_ == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot unmap non allocated buffer");

        vkUnmapMemory(*device_, memory_);
    }

    uint32_t RawVulkanBuffer::find_mem_type(
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
} // namespace saltus::vulkan
