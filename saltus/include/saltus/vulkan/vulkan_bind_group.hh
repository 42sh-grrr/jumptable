#pragma once

#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/fwd.hh"
#include "saltus/bind_group.hh"

namespace saltus::vulkan
{
    class VulkanBindGroup: public BindGroup
    {
    public:
        VulkanBindGroup(std::shared_ptr<VulkanDevice> device, BindGroupCreateInfo info);
        ~VulkanBindGroup();

        const std::shared_ptr<VulkanDevice> &device() const;
        const std::shared_ptr<VulkanBindGroupLayout> &bind_group_layout() const;

        const VkDescriptorSet &descriptor_set() const;

        void set_binding(
            uint32_t binding_id,
            const std::shared_ptr<Buffer> &buffer,
            uint32_t array_index = 0,
            uint64_t offset = 0,
            std::optional<uint64_t> size = std::nullopt
        ) override;

        void set_binding(
            uint32_t binding_id,
            const std::shared_ptr<Texture> &texture,
            uint32_t array_index = 0
        ) override;

    private:
        std::shared_ptr<VulkanDevice> device_;
        std::shared_ptr<VulkanBindGroupLayout> bind_group_layout_;

        /// Used to keep alive resources used by bindings
        std::unordered_map<uint32_t, std::shared_ptr<void>> binds_;

        VkDescriptorPool descriptor_pool_;
        VkDescriptorSet descriptor_set_;
    };
} // namespace saltus

