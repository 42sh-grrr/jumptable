#include "graphics/window.hh"

#include <cstring>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <memory>
#include <unistd.h>
#include <xcb/xproto.h>

namespace graphics {
    struct Window::WindowData
    {
        xcb_connection_t *connection;
        xcb_screen_t *screen;
        xcb_window_t window_id;
    };

    Window::Window(
        const char *title, int width, int height
    ): data(std::make_unique<WindowData>())
    {
        data->connection = xcb_connect(nullptr, nullptr);

        const xcb_setup_t *setup = xcb_get_setup(data->connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        data->screen = iter.data;

        data->window_id = xcb_generate_id(data->connection);
        xcb_create_window(
            data->connection,              /* connection */
            XCB_COPY_FROM_PARENT,          /* depth */
            data->window_id,               /* window id           */
            data->screen->root,            /* parent window       */
            0, 0,                          /* x, y                */
            width, height,                 /* width, height       */
            10,                            /* border_width        */
            XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
            data->screen->root_visual,     /* visual              */
            0, NULL                        /* masks, not used yet */
        );

        xcb_change_property(
            data->connection,
            XCB_PROP_MODE_REPLACE,
            data->window_id,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8,
            strlen(title),
            title
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
