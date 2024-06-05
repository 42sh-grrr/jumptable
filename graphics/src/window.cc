#include "graphics/window.hh"

#include <X11/Xlib.h>
#include <memory>
#include <stdexcept>
#include <unistd.h>

using XWindow = Window;

namespace graphics {
    struct Window::WindowData
    {
        Display *display;
        int screen;

        XWindow xwindow;
    };

    Window::Window()
    {
        data = std::make_unique<WindowData>();

        data->display = XOpenDisplay(nullptr);
        if (!data->display)
            throw new std::logic_error("Could not load display");
        data->screen = DefaultScreen(data->display);
        data->xwindow = XCreateSimpleWindow(
            data->display,
            RootWindow(data->display, data->screen),
            0, 0,
            100, 100,
            1,
            BlackPixel(data->display, data->screen),
            WhitePixel(data->display, data->screen)
        );
        XSelectInput(data->display, data->xwindow, ExposureMask | KeyPressMask);
        XMapWindow(data->display, data->xwindow);
    }

    Window::~Window()
    {
        if (data->display)
            XCloseDisplay(data->display);
    }

    void Window::run()
    {
        XEvent event;
        for (;;)
        {
            XNextEvent(data->display, &event);
            if (event.type == Expose) {
                XDrawString(data->display, data->xwindow, DefaultGC(data->display, data->screen), 10, 50, "bite", 5);
            }
            if (event.type == KeyPress)
                break;
        }
    }
}
