#include "logger/level.hh"
#include "saltus/window_events.hh"
#include "saltus/window.hh"

#include <cstring>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xfixes.h>
#include <memory>
#include <unistd.h>
#include <xcb/xproto.h>
#include <vulkan/vulkan_xcb.h>

namespace saltus
{
    const uint32_t mouse_event_mask =
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION | 
        XCB_EVENT_MASK_BUTTON_MOTION | 
        XCB_EVENT_MASK_BUTTON_1_MOTION | 
        XCB_EVENT_MASK_BUTTON_2_MOTION | 
        XCB_EVENT_MASK_BUTTON_3_MOTION | 
        XCB_EVENT_MASK_BUTTON_4_MOTION | 
    0;
    const uint32_t event_mask =
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |

        mouse_event_mask |

    0;

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

        void set_property(const char *name, const char *value, const char *type)
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

        std::unique_ptr<WindowEvent> window_event_from_xcb_event(xcb_generic_event_t *event)
        {
            if (!event)
                return std::unique_ptr<WindowEvent>();
            switch (event->response_type & ~0x80)
            {
            case XCB_CLIENT_MESSAGE: {
                auto client_message = reinterpret_cast<xcb_client_message_event_t *>(event);
                if (
                    client_message->format == 32 &&
                    client_message->type == intern("WM_PROTOCOLS") &&
                    client_message->data.data32[0] == intern("WM_DELETE_WINDOW")
                )
                    return std::make_unique<WindowCloseRequestEvent>();
           
                return std::unique_ptr<WindowEvent>();
            }
            case XCB_KEY_PRESS: {
                auto keypress = reinterpret_cast<xcb_key_press_event_t *>(event);
                return std::make_unique<WindowKeyPressEvent>(WindowKeyPressEvent::Data{
                    .keycode = keypress->detail,
                });
            }
            case XCB_KEY_RELEASE: {
                auto keyrelease = reinterpret_cast<xcb_key_release_event_t *>(event);
                return std::make_unique<WindowKeyReleaseEvent>(WindowKeyReleaseEvent::Data{
                    .keycode = keyrelease->detail,
                });
            }
            case XCB_MOTION_NOTIFY: {
                auto motion = reinterpret_cast<xcb_motion_notify_event_t *>(event);
                return std::make_unique<WindowMouseMoveEvent>(WindowMouseMoveEvent::Data{
                    .x = motion->event_x,
                    .y = motion->event_y,
                    .root_x = motion->root_x,
                    .root_y = motion->root_y,
                    .mouse_buttons = static_cast<uint8_t>(motion->state >> 8),
                });
            }
            case XCB_BUTTON_PRESS: {
                auto button = reinterpret_cast<xcb_button_press_event_t *>(event);
                return std::make_unique<WindowMouseButtonPressEvent>(WindowMouseButtonPressEvent::Data{
                    .mouse = {
                        .x = button->event_x,
                        .y = button->event_y,
                        .root_x = button->root_x,
                        .root_y = button->root_y,
                        .mouse_buttons = static_cast<uint8_t>(button->state >> 8),
                    },
                    .pressed_button = button->detail,
                });
            }
            case XCB_BUTTON_RELEASE: {
                auto button = reinterpret_cast<xcb_button_release_event_t *>(event);
                return std::make_unique<WindowMouseButtonReleaseEvent>(WindowMouseButtonReleaseEvent::Data{
                    .mouse = {
                        .x = button->event_x,
                        .y = button->event_y,
                        .root_x = button->root_x,
                        .root_y = button->root_y,
                        .mouse_buttons = static_cast<uint8_t>(button->state >> 8),
                    },
                    .released_button = button->detail,
                });
            }
            case XCB_EXPOSE: {
                auto expose = reinterpret_cast<xcb_expose_event_t *>(event);
                return std::make_unique<WindowExposeEvent>(WindowExposeEvent::Data{
                    .width = expose->width,
                    .height = expose->height,
                });
            }
            default:
                logger::debug() << "Unknown XCB event: " << (event->response_type & ~0x80) << "\n";
                return std::unique_ptr<WindowEvent>();
            }
        }
    };

    WindowBuilder &WindowBuilder::title(const char *title)
    {
        title_ = title;
        return *this;
    }

    WindowBuilder &WindowBuilder::size(int width, int height)
    {
        width_ = width;
        height_ = height;
        return *this;
    }

    WindowBuilder &WindowBuilder::min_size(int min_width, int min_height)
    {
        min_width_ = min_width;
        min_height_ = min_height;
        return *this;
    }

    Window WindowBuilder::build()
    {
        std::unique_ptr<Window::WindowData> data = std::make_unique<Window::WindowData>();

        data->connection = xcb_connect(nullptr, nullptr);

        xcb_xfixes_query_version(data->connection, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);

        const xcb_setup_t *setup = xcb_get_setup(data->connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        data->screen = iter.data;

        data->window_id = xcb_generate_id(data->connection);

        uint32_t mask = XCB_CW_EVENT_MASK;
        uint32_t value_list[] = { event_mask };
        xcb_create_window(
            data->connection,              /* connection */
            XCB_COPY_FROM_PARENT,          /* depth */
            data->window_id,               /* window id           */
            data->screen->root,            /* parent window       */
            0, 0,                          /* x, y                */
            width_, height_,               /* width, height       */
            10,                            /* border_width        */
            XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
            data->screen->root_visual,     /* visual              */
            mask,
            value_list
        );

        xcb_icccm_set_wm_name(data->connection, data->window_id,
            XCB_ATOM_STRING,
            // 8 because everyone uses 8, not sure why
            8,
            strlen(title_), title_
        );
        xcb_size_hints_t hints;
        hints.flags = XCB_ICCCM_SIZE_HINT_P_SIZE |
                 XCB_ICCCM_SIZE_HINT_P_MIN_SIZE;
        hints.width = width_;
        hints.height = height_;
        hints.min_width = min_width_;
        hints.min_height = min_height_;

        xcb_icccm_set_wm_normal_hints_checked(
            data->connection, data->window_id, &hints
        );

        xcb_atom_t protocols[] = {
            data->intern("WM_DELETE_WINDOW")
        };
        xcb_icccm_set_wm_protocols(
            data->connection, data->window_id, data->intern("WM_PROTOCOLS"),
            sizeof(protocols) / sizeof(*protocols), protocols
        );

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

    std::unique_ptr<WindowEvent> Window::poll_event()
    {
        return data_->window_event_from_xcb_event(xcb_poll_for_event(data_->connection));
    }

    std::unique_ptr<WindowEvent> Window::wait_event()
    {
        return data_->window_event_from_xcb_event(xcb_wait_for_event(data_->connection));
    }

    WindowGeometry Window::request_geometry() const
    {
        auto cookie = xcb_get_geometry(data_->connection, data_->window_id);
        auto reply = xcb_get_geometry_reply(data_->connection, cookie, nullptr);
        if (!reply)
            throw std::runtime_error("Could not request error");
        return {
            .x = reply->x,
            .y = reply->y,
            .width = reply->width,
            .height = reply->height,
        };
    }

    void Window::hide_mouse() const
    {
        const int width = 1;
        const int height = 1;

        // Create a pixmap (1x1 transparent pixel)
        xcb_pixmap_t pixmap = xcb_generate_id(data_->connection);
        xcb_create_pixmap(data_->connection, 1, pixmap, data_->screen->root, width, height);

        // Create a graphic context
        xcb_gcontext_t gc = xcb_generate_id(data_->connection);
        uint32_t mask = XCB_GC_FOREGROUND;
        uint32_t value_list = 0;  // Transparent color
        xcb_create_gc(data_->connection, gc, pixmap, mask, &value_list);

        // Draw a transparent pixel
        xcb_rectangle_t rect = {0, 0, width, height};
        xcb_poly_fill_rectangle(data_->connection, pixmap, gc, 1, &rect);

        // Create the cursor
        xcb_cursor_t cursor = xcb_generate_id(data_->connection);
        xcb_create_cursor(data_->connection, cursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);

        // Change the cursor for the root window
        xcb_change_window_attributes(data_->connection, data_->screen->root, XCB_CW_CURSOR, &cursor);

        xcb_free_cursor(data_->connection, cursor);
        xcb_free_pixmap(data_->connection, pixmap);
        xcb_free_gc(data_->connection, gc);
    }

    void Window::show_mouse() const
    {
        auto c = XCB_NONE;
        xcb_change_window_attributes(data_->connection, data_->screen->root, XCB_CW_CURSOR, &c);
    }

    bool Window::capture_mouse() const
    {
        auto cookie = xcb_grab_pointer(
            data_->connection, 1, data_->window_id, 0,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
            data_->window_id,
            XCB_NONE,
            XCB_CURRENT_TIME
        );
        auto reply = xcb_grab_pointer_reply(data_->connection, cookie, nullptr);
        if (!reply)
            throw std::runtime_error("Could not capture mouse");
        switch (reply->status)
        {
        case XCB_GRAB_STATUS_SUCCESS:
            logger::trace() << "Mouse grab success\n";
            return true;
        case XCB_GRAB_STATUS_ALREADY_GRABBED:
            logger::trace() << "Mouse grab error: Already grabbed\n";
            break;
        case XCB_GRAB_STATUS_INVALID_TIME:
            logger::trace() << "Mouse grab error: Invalid time\n";
            break;
        case XCB_GRAB_STATUS_NOT_VIEWABLE:
            logger::trace() << "Mouse grab error: Not viewable\n";
            break;
        case XCB_GRAB_STATUS_FROZEN:
            logger::trace() << "Mouse grab error: Frozen\n";
            break;
        default:
            break;
        }
        return false;
    }

    void Window::release_mouse() const
    {
        xcb_ungrab_pointer(data_->connection, XCB_CURRENT_TIME);
    }

    void Window::warp_mouse(int new_x, int new_y) const {
        xcb_warp_pointer_checked(
             data_->connection,
             XCB_NONE,
             data_->window_id,
             0, 0,
             0, 0,
             new_x, new_y
        );
    }
    
    VkSurfaceKHR Window::create_vulkan_surface(VkInstance instance) const
    {
        VkXcbSurfaceCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        create_info.connection = data_->connection;
        create_info.window = data_->window_id;

        VkSurfaceKHR surface;
        VkResult result = vkCreateXcbSurfaceKHR(instance, &create_info, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create surface");
        }

        return surface;
    }
}
