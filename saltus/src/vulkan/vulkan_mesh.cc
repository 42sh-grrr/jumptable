#include "saltus/vulkan/vulkan_mesh.hh"

namespace saltus::vulkan
{
    VulkanMesh::VulkanMesh(
        std::shared_ptr<VulkanDevice> device,
        MeshCreateInfo info
    ): Mesh(info)
    { }

    VulkanMesh::~VulkanMesh()
    {
        
    }

    const std::shared_ptr<VulkanDevice> &VulkanMesh::device() const
    {
        return device_;
    }
} // namespace saltus::vulkan

