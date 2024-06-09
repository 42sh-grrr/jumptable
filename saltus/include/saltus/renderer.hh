#pragma once

#include <memory>

#include "saltus/window.hh"

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

    protected:
        Renderer(Window &window);

        Window &window_;
    };
} // namespace saltus

