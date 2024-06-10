#include "saltus/renderer.hh"

#include "saltus/vulkan/vulkan_renderer.hh"

namespace saltus
{
    Renderer::Renderer(Window &window): window_(window)
    {
        
    }

    Renderer::~Renderer()
    {
        
    }

    std::unique_ptr<Renderer> Renderer::create(Window &window)
    {
        return std::make_unique<vulkan::VulkanRenderer>(window);
    }
} // namespace saltus
