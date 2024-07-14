#include "saltus/vulkan/vulkan_mesh.hh"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/vulkan_buffer.hh"

namespace saltus::vulkan
{
    VkIndexType mesh_index_format_to_index_type(MeshIndexFormat format)
    {
        switch (format)
        {
        case MeshIndexFormat::UInt16:
            return VK_INDEX_TYPE_UINT16;
        case MeshIndexFormat::UInt32:
            return VK_INDEX_TYPE_UINT32;
        }
        throw std::runtime_error("Invalid index format");
    }

    VulkanMesh::VulkanMesh(
        std::shared_ptr<VulkanDevice> device,
        MeshCreateInfo info
    ): Mesh(info), device_(device),
       index_type_(mesh_index_format_to_index_type(info.index_format))
    {
        for (const auto &attr : info.vertex_attributes)
        {
            const auto &vertex_buff =
                std::dynamic_pointer_cast<VulkanBuffer>(attr.buffer);
            if (!vertex_buff)
                throw std::runtime_error("A vulkan mesh can only work with a vulkan buffer");
            vertex_buffers_.push_back(vertex_buff);
        }

        if (info.index_buffer)
        {
            index_buffer_ =
                std::dynamic_pointer_cast<VulkanBuffer>(info.index_buffer);
            if (!index_buffer_)
                throw std::runtime_error("A vulkan mesh can only work with a vulkan buffer");
        }
    }

    VulkanMesh::~VulkanMesh()
    { }

    const std::shared_ptr<VulkanDevice> &VulkanMesh::device() const
    {
        return device_;
    }

    const std::vector<std::shared_ptr<VulkanBuffer>> &VulkanMesh::vertex_buffers() const
    {
        return vertex_buffers_;
    }

    const VkIndexType &VulkanMesh::index_type() const
    {
        return index_type_;
    }

    const std::shared_ptr<VulkanBuffer> &VulkanMesh::index_buffer() const
    {
        return index_buffer_;
    }
} // namespace saltus::vulkan

