#include "saltus/vulkan/vulkan_mesh.hh"
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanMesh::VulkanMesh(
        std::shared_ptr<VulkanDevice> device,
        MeshCreateInfo info
    ): Mesh(info)
    {
        for (const auto &attr : info.vertex_attributes)
        {
            std::unique_ptr<VulkanBuffer> buffer = std::make_unique<VulkanBuffer>(
                device, attr.data.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            );
            buffer->alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void *data = buffer->map();
            memcpy(data, attr.data.data(), attr.data.size());
            buffer->unmap();

            vertex_buffers_.push_back(std::move(buffer));
        }
    }

    VulkanMesh::~VulkanMesh()
    { }

    const std::shared_ptr<VulkanDevice> &VulkanMesh::device() const
    {
        return device_;
    }

    const std::vector<std::unique_ptr<VulkanBuffer>> &VulkanMesh::vertex_buffers() const
    {
        return vertex_buffers_;
    }
} // namespace saltus::vulkan

