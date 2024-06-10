#pragma once

#include <memory>

#include "saltus/window.hh"
#include "saltus/shader.hh"

namespace saltus
{
    class Renderer
    {
    public:
        virtual ~Renderer();

        static std::unique_ptr<Renderer> create(Window &window);

        Window &window();

        virtual void render() = 0;
        virtual void wait_for_idle() = 0;

        virtual std::shared_ptr<Shader> create_shader(ShaderCreateInfo) = 0;

    protected:
        Renderer(Window &window);

        Window &window_;
    };
} // namespace saltus

