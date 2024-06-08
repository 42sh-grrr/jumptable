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

    protected:
        Renderer();
    };
} // namespace saltus

