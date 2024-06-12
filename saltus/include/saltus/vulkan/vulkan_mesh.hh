#pragma once

#include <memory>
#include "saltus/mesh.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanMesh: public Mesh
    {
    public:
        VulkanMesh(std::shared_ptr<VulkanDevice>, MeshCreateInfo);
        ~VulkanMesh();

        const std::shared_ptr<VulkanDevice> &device() const;

    private:
        std::shared_ptr<VulkanDevice> device_;
    };
} // namespace saltus::vulkan
