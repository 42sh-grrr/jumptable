#pragma once

#include <memory>

namespace graphics
{
    class Window
    {
    public:
        Window(const char *title, int width = 100, int height = 100);
        ~Window();

        void run();

    private:
        struct WindowData;
        std::unique_ptr<WindowData> data;
    };
} // namespace graphics
