#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "saltus/mesh.hh"
#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    VkIndexType mesh_index_format_to_index_type(MeshIndexFormat format);

    class VulkanMesh: public Mesh
    {
    public:
        VulkanMesh(std::shared_ptr<VulkanDevice>, MeshCreateInfo);
        ~VulkanMesh();

        const std::shared_ptr<VulkanDevice> &device() const;

        const std::vector<std::shared_ptr<VulkanBuffer>> &vertex_buffers() const;

        const VkIndexType &index_type() const;
        const std::shared_ptr<VulkanBuffer> &index_buffer() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        std::vector<std::shared_ptr<VulkanBuffer>> vertex_buffers_;

        VkIndexType index_type_;
        std::shared_ptr<VulkanBuffer> index_buffer_;
    };
} // namespace saltus::vulkan
