#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_instance_group.hh"
#include "saltus/vertex_attribute.hh"
#include "saltus/vulkan/vulkan_material.hh"

namespace saltus::vulkan
{
    static VkFormat attribute_type_to_vulkan_format(VertexAttributeType type)
    {
        switch (type.scalar_type)
        {
        case VertexAttributeDataType::u8:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R8_UINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R8G8_UINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R8G8B8_UINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R8G8B8A8_UINT;
            }
        case VertexAttributeDataType::i8:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R8_SINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R8G8_SINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R8G8B8_SINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R8G8B8A8_SINT;
            }
        case VertexAttributeDataType::u16:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R16_UINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R16G16_UINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R16G16B16_UINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R16G16B16A16_UINT;
            }
        case VertexAttributeDataType::i16:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R16_SINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R16G16_SINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R16G16B16_SINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R16G16B16A16_SINT;
            }
        case VertexAttributeDataType::u32:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R32_UINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R32G32_UINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R32G32B32_UINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R32G32B32A32_UINT;
            }
        case VertexAttributeDataType::i32:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R32_SINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R32G32_SINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R32G32B32_SINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R32G32B32A32_SINT;
            }
        case VertexAttributeDataType::f32:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R32_SFLOAT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R32G32_SFLOAT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
        case VertexAttributeDataType::u64:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R64_UINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R64G64_UINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R64G64B64_UINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R64G64B64A64_UINT;
            }
        case VertexAttributeDataType::i64:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R64_SINT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R64G64_SINT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R64G64B64_SINT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R64G64B64A64_SINT;
            }
        case VertexAttributeDataType::f64:
            switch (type.format)
            {
            case VertexAttributeFormat::Scalar:
                return VK_FORMAT_R64_SFLOAT;
            case VertexAttributeFormat::Vec2:
                return VK_FORMAT_R64G64_SFLOAT;
            case VertexAttributeFormat::Vec3:
                return VK_FORMAT_R64G64B64_SFLOAT;
            case VertexAttributeFormat::Vec4:
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            }
        }

        throw std::runtime_error("Invalid format");
    };

    VulkanInstanceGroup::VulkanInstanceGroup(
        std::shared_ptr<VulkanRenderTarget> render_target,
        InstanceGroupCreateInfo create_info
    ): InstanceGroup(create_info), render_target_(render_target) {
        material_ = std::dynamic_pointer_cast<VulkanMaterial>(create_info.material);
        if (!material_)
            throw std::runtime_error("Can only create vulkan instance group with a vulkan material");
        mesh_ = std::dynamic_pointer_cast<VulkanMesh>(create_info.mesh);
        if (!mesh_)
            throw std::runtime_error("Can only create vulkan instance group with a vulkan mesh");

        for (const auto &bind_group : create_info.bind_groups)
        {
            auto vk_bind_group =
                std::dynamic_pointer_cast<VulkanBindGroup>(bind_group);
            if (!vk_bind_group)
                throw std::runtime_error("Can only create vulkan instance groups with a vulkan bind groups");
            bind_groups_.push_back(vk_bind_group);
            descriptor_sets_.push_back(vk_bind_group->descriptor_set());
        }

        create_pipeline_layout();
        create_pipeline();
    }
    VulkanInstanceGroup::~VulkanInstanceGroup()
    {
        destroy_pipeline();
        destroy_pipeline_layout();
    }

    const std::shared_ptr<VulkanRenderTarget> &VulkanInstanceGroup::render_target() const
    {
        return render_target_;
    }

    const std::shared_ptr<VulkanMaterial> &VulkanInstanceGroup::material() const
    {
        return material_;
    }

    const std::shared_ptr<VulkanMesh> &VulkanInstanceGroup::mesh() const
    {
        return mesh_;
    }

    const std::vector<std::shared_ptr<VulkanBindGroup>> &VulkanInstanceGroup::bind_groups() const
    {
        return bind_groups_;
    }

    VkPipelineLayout VulkanInstanceGroup::pipeline_layout() const
    {
        return pipeline_layout_;
    }

    VkPipeline VulkanInstanceGroup::pipeline() const
    {
        return pipeline_;
    }

    void VulkanInstanceGroup::render(VkCommandBuffer command_buffer) const
    {
        vkCmdBindPipeline(command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_
        );

        vkCmdBindVertexBuffers(
            command_buffer, 0,
            vertex_buffers_.size(),
            vertex_buffers_.data(), vertex_offsets_.data()
        );

        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout_,
            0, descriptor_sets_.size(), descriptor_sets_.data(),
            0, nullptr
        );

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(render_target_->swapchain_extent().width);
        viewport.height = static_cast<float>(render_target_->swapchain_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = render_target_->swapchain_extent();
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        vkCmdDraw(command_buffer, mesh_->vertex_count(), 1, 0, 0);
    }

    void VulkanInstanceGroup::create_pipeline_layout()
    {
        const auto device = render_target_->device();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
        std::ranges::transform(
            material_->bind_group_layouts(),
            std::back_insert_iterator(descriptor_set_layouts),
            [](const auto &l){ return l->layout(); }
        );

        pipelineLayoutInfo.setLayoutCount = descriptor_set_layouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptor_set_layouts.data();

        VkResult result =
            vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, &pipeline_layout_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create pipeline layout");
    }

    void VulkanInstanceGroup::create_pipeline()
    {
        const auto device = render_target_->device();

        auto vert_shader_stage_info = material_->vertex_shader()->stage_create_info();
        auto frag_shader_stage_info = material_->fragment_shader()->stage_create_info();

        VkPipelineShaderStageCreateInfo shader_stages[] = {
            vert_shader_stage_info,
            frag_shader_stage_info
        };

        VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamic_state.dynamicStateCount = sizeof(dynamic_states) / sizeof(*dynamic_states);
        dynamic_state.pDynamicStates = dynamic_states;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        std::vector<VkVertexInputBindingDescription> bindings{};
        std::vector<VkVertexInputAttributeDescription> attrs{};

        vertex_buffers_.clear();
        vertex_offsets_.clear();
        for (size_t mesh_attr_i = 0; mesh_attr_i < mesh_->vertex_attributes().size(); mesh_attr_i++)
        {
            const MaterialVertexAttribute *material_attr = nullptr;
            for (const auto &attr : material_->vertex_attributes())
            {
                if (attr.name == mesh_->vertex_attributes()[mesh_attr_i].name)
                    material_attr = &attr;
            }
            if (!material_attr)
                continue;
            uint32_t binding_i = vertex_buffers_.size();

            VkVertexInputBindingDescription binding{};
            binding.binding = binding_i;
            binding.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
            binding.stride = material_attr->type.size();
            bindings.push_back(binding);

            VkVertexInputAttributeDescription attr{};
            attr.binding = binding_i;
            attr.location = material_attr->location;
            attr.offset = 0;
            attr.format = attribute_type_to_vulkan_format(material_attr->type);
            attrs.push_back(attr);

            vertex_buffers_.push_back(mesh_->vertex_buffers()[mesh_attr_i]->raw_buffer());
            vertex_offsets_.push_back(0);
        }
        vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
        vertexInputInfo.pVertexBindingDescriptions = bindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = attrs.size();
        vertexInputInfo.pVertexAttributeDescriptions = attrs.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        switch (material_->primitive_topology())
        {
        case PritmitiveTopology::TriangleList:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case PritmitiveTopology::TriangleStrip:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        case PritmitiveTopology::LineList:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case PritmitiveTopology::LineStrip:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        VkPipelineViewportStateCreateInfo viewport_state{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        switch (material_->cull_mode())
        {
        case MaterialCullMode::None:
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            break;
        case MaterialCullMode::Back:
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        case MaterialCullMode::Front:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case MaterialCullMode::All:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
            break;
        }
        switch (material_->front_face())
        {
        case MaterialFrontFace::CounterClockwise:
            if (mesh_->flip_faces())
                rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            else
                rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        case MaterialFrontFace::Clockwise:
            if (mesh_->flip_faces())
                rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            else
                rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            break;
        }
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                            | VK_COLOR_COMPONENT_G_BIT
                                            | VK_COLOR_COMPONENT_B_BIT
                                            | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkPipelineRenderingCreateInfoKHR pipeline_create{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
        pipeline_create.colorAttachmentCount    = 1;
        pipeline_create.pColorAttachmentFormats = &render_target_->swapchain_image_format();
        // TODO: Depth buffer
        // pipeline_create.depthAttachmentFormat   = render_target_->swapchain_image_format();
        // pipeline_create.stencilAttachmentFormat = render_target_->swapchain_image_format();

        VkGraphicsPipelineCreateInfo pipeline_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipeline_info.pNext = &pipeline_create;
        pipeline_info.stageCount = sizeof(shader_stages) / sizeof(*shader_stages);
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertexInputInfo;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr;
        pipeline_info.pColorBlendState = &colorBlending;
        pipeline_info.pDynamicState = &dynamic_state;

        pipeline_info.layout = pipeline_layout_;
        pipeline_info.renderPass = nullptr;
        pipeline_info.subpass = 0;

        VkResult result = vkCreateGraphicsPipelines(
            *device, nullptr, 1, &pipeline_info, nullptr, &pipeline_
        );
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create graphics pipeline");
    }

    void VulkanInstanceGroup::destroy_pipeline_layout()
    {
        vkDestroyPipelineLayout(*render_target_->device(), pipeline_layout_, nullptr);
    }

    void VulkanInstanceGroup::destroy_pipeline()
    {
        vkDestroyPipeline(*render_target_->device(), pipeline_, nullptr);
    }
} // namespace saltus::vulkan
