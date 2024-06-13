#pragma once

#include <memory>
#include "saltus/mesh.hh"
#include "saltus/vulkan/vulkan_buffer.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanMesh: public Mesh
    {
    public:
        VulkanMesh(std::shared_ptr<VulkanDevice>, MeshCreateInfo);
        ~VulkanMesh();

        const std::shared_ptr<VulkanDevice> &device() const;

        const std::vector<std::shared_ptr<VulkanBuffer>> &vertex_buffers() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        std::vector<std::shared_ptr<VulkanBuffer>> vertex_buffers_;
    };
} // namespace saltus::vulkan
