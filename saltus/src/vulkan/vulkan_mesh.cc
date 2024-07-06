#include "saltus/vulkan/vulkan_mesh.hh"
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanMesh::VulkanMesh(
        std::shared_ptr<VulkanDevice> device,
        MeshCreateInfo info
    ): Mesh(info), device_(device)
    {
        for (const auto &attr : info.vertex_attributes)
        {
            const auto &vertex_buff =
                std::dynamic_pointer_cast<VulkanBuffer>(attr.buffer);
            if (!vertex_buff)
                throw std::runtime_error("A vulkan mesh can only work with a vulkan buffer");
            vertex_buffers_.push_back(vertex_buff);
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
} // namespace saltus::vulkan

