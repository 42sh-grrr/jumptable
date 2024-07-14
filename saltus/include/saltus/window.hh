#pragma once

#include "window_events.hh"

#include <memory>
#include <vulkan/vulkan_core.h>

namespace saltus
{
    class Window;

    struct WindowGeometry
    {
        /// Position
        int x, y;
        /// Size
        int width, height;
    };

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

        WindowGeometry request_geometry() const;

        void hide_mouse() const;
        void show_mouse() const;

        /// Returns true only and only if the mouse is now captured
        bool capture_mouse() const;
        void release_mouse() const;

        void warp_mouse(int new_x, int new_y) const;

        VkSurfaceKHR create_vulkan_surface(VkInstance instance) const;

    protected:
        struct WindowData;
        Window(std::unique_ptr<WindowData> data);

        friend WindowBuilder;

    private:
        std::unique_ptr<WindowData> data_;
    };
} // namespace saltus
