#pragma once

#include <memory>

namespace graphics
{
    class Window;

    class WindowBuilder {
    public:
        WindowBuilder() = default;

        WindowBuilder &title(const char *title);
        WindowBuilder &size(int width, int height);

        Window build();
        
    private:
        const char *title_ = "Unnamed";
        int width_ = 100;
        int height_ = 100;

        int minWidth_ = 100;
        int minHeight_ = 100;
    };

    class Window
    {
    public:
        ~Window();

        void run();

    protected:
        struct WindowData;
        Window(std::unique_ptr<WindowData> data);

        friend WindowBuilder;

    private:
        std::unique_ptr<WindowData> data_;
    };
} // namespace graphics
