#include "saltus/renderer.hh"

#include "saltus/vulkan/vulkan_renderer.hh"

namespace saltus
{
    std::ostream& operator <<(std::ostream& outs, RendererPresentMode present_mode)
    {
        switch (present_mode)
        {
        case RendererPresentMode::Immediate:
            return outs << "Immediate";
        case RendererPresentMode::Mailbox:
            return outs << "Mailbox";
        case RendererPresentMode::VSync:
            return outs << "VSync";
        }
        return outs;
    }

    Renderer::Renderer(RendererCreateInfo info):
        window_(info.window), target_present_mode_(info.target_present_mode)
    {
        
    }

    Renderer::~Renderer()
    {
        
    }

    std::unique_ptr<Renderer> Renderer::create(RendererCreateInfo info)
    {
        return std::make_unique<vulkan::VulkanRenderer>(info);
    }

    Window &Renderer::window()
    {
        return window_;
    }

    const RendererPresentMode &Renderer::target_present_mode() const
    {
        return target_present_mode_;
    }
} // namespace saltus
