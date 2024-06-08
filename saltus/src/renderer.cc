#include "saltus/renderer.hh"

#include "saltus/vulkan_renderer.hh"

namespace saltus
{
    Renderer::Renderer(Window &window): window_(window)
    {
        
    }

    Renderer::~Renderer()
    {
        
    }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::unique_ptr<Renderer> Renderer::create(Window &window)
    {
        return std::make_unique<VulkanRenderer>(window);
    }
    #pragma GCC diagnostic pop
} // namespace saltus
