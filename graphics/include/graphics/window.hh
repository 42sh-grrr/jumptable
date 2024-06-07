#pragma once

#include "window_events.hh"

#include <memory>

namespace graphics
{
    class Window;

    class WindowBuilder {
    public:
        WindowBuilder() = default;

        WindowBuilder &title(const char *title);
        WindowBuilder &size(int width, int height);
        WindowBuilder &min_size(int min_width, int min_height);

        Window build();
        
    private:
        const char *title_ = "Unnamed";
        int width_ = 100;
        int height_ = 100;

        int min_width_ = 100;
        int min_height_ = 100;
    };

    class Window
    {
    public:
        ~Window();

        std::unique_ptr<WindowEvent> poll_event();
        std::unique_ptr<WindowEvent> wait_event();

    protected:
        struct WindowData;
        Window(std::unique_ptr<WindowData> data);

        friend WindowBuilder;

    private:
        std::unique_ptr<WindowData> data_;
    };
} // namespace graphics
