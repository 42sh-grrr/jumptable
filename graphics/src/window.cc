#include "graphics/window.hh"

#include <X11/Xlib.h>
#include <iostream>
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
        XSetWindowAttributes attrs = { };
        data->xwindow = XCreateWindow(
            data->display,
            RootWindow(data->display, data->screen),
            // x y
            0, 0,
            // width height
            100, 100,
            // border width
            0,
            // depth
            CopyFromParent,
            // class
            InputOutput,
            // visual
            CopyFromParent,
            // value mask (which attributes are set)
            0,
            // attributes
            &attrs
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
                continue;
            }
            if (event.type == KeyPress && event.xkey.keycode == 9) {
                break;
            }
        }
    }
}
