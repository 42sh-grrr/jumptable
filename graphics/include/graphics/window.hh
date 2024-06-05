#pragma once

#include <memory>

namespace graphics
{
    class Window
    {
    public:
        Window();
        ~Window();

        void run();

    private:
        struct WindowData;
        std::unique_ptr<WindowData> data;
    };
} // namespace graphics
