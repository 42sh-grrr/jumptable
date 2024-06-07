#include "graphics/window.hh"

#include <xcb/xcb.h>
#include <memory>
#include <unistd.h>
#include <xcb/xproto.h>

namespace graphics {
    struct Window::WindowData
    {
        xcb_connection_t *connection;
        xcb_window_t window_id;
    };

    Window::Window(): data(std::make_unique<WindowData>())
    {
        data->connection = xcb_connect(nullptr, nullptr);

        const xcb_setup_t *setup = xcb_get_setup(data->connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        xcb_screen_t *screen = iter.data;

        data->window_id = xcb_generate_id(data->connection);
        xcb_create_window(
            data->connection,              /* connection */
            XCB_COPY_FROM_PARENT,          /* depth */
            data->window_id,               /* window id           */
            screen->root,                  /* parent window       */
            0, 0,                          /* x, y                */
            150, 150,                      /* width, height       */
            10,                            /* border_width        */
            XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
            screen->root_visual,           /* visual              */
            0, NULL                        /* masks, not used yet */
        );
        xcb_map_window(data->connection, data->window_id);
        xcb_flush(data->connection);
    }

    Window::~Window()
    {
        if (data->connection)
            xcb_disconnect(data->connection);
    }

    void Window::run()
    {
    }
}
