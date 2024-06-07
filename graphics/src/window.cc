#include "graphics/window.hh"

#include <cstring>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <memory>
#include <unistd.h>

namespace graphics {
    struct Window::WindowData
    {
        xcb_connection_t *connection;
        xcb_screen_t *screen;
        xcb_window_t window_id;

        xcb_atom_t intern(const char *name)
        {
            auto cookie = xcb_intern_atom(connection, 0, strlen(name), name);
            auto reply = xcb_intern_atom_reply(connection, cookie, nullptr);
            auto atom = reply->atom;
            free(reply);
            return atom;
        }

        void setProperty(const char *name, const char *value, const char *type)
        {
            xcb_change_property(
                connection,
                XCB_PROP_MODE_REPLACE,
                window_id,
                intern(name),
                intern(type),
                8,
                strlen(value),
                value
            );
        }
    };

    WindowBuilder &WindowBuilder::title(const char *title) {
        title_ = title;
        return *this;
    }

    WindowBuilder &WindowBuilder::size(int width, int height) {
        width_ = width;
        height_ = height;
        return *this;
    }

    Window WindowBuilder::build() {
        std::unique_ptr<Window::WindowData> data = std::make_unique<Window::WindowData>();

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
            width_, height_,                 /* width, height       */
            10,                            /* border_width        */
            XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
            data->screen->root_visual,     /* visual              */
            0, NULL                        /* masks, not used yet */
        );

        data->setProperty("WM_NAME", title_, "STRING");
        data->setProperty("_NET_WM_NAME", title_, "UTF8_STRING");

        xcb_map_window(data->connection, data->window_id);
        xcb_flush(data->connection);

        return Window(std::move(data));
    }

    Window::Window(std::unique_ptr<WindowData> data): data_(std::move(data))
    { }

    Window::~Window()
    {
        if (data_->connection)
            xcb_disconnect(data_->connection);
    }

    void Window::run()
    {
    }
}
