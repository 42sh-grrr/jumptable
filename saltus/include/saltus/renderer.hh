#pragma once

#include <memory>

#include "saltus/window.hh"

namespace saltus
{
    class Renderer
    {
    public:
        ~Renderer();

        static std::unique_ptr<Renderer> create(Window &window);

        Window &window();

    protected:
        Renderer(Window &window);

        Window &window_;
    };
} // namespace saltus

