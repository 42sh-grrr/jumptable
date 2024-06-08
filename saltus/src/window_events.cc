#include "saltus/window_events.hh"

namespace saltus
{
    const char *WindowExposeEvent::name() const
    {
        return "expose";
    }

    WindowExposeEvent::WindowExposeEvent(Data data):
        width(data.width), height(data.height)
    { }
    
    WindowKeyboardEvent::WindowKeyboardEvent(Data data):
        keycode(data.keycode)
    { }
    
    WindowKeyPressEvent::WindowKeyPressEvent(Data data): WindowKeyboardEvent(data)
    { }
    
    const char *WindowKeyPressEvent::name() const
    {
        return "keypress";
    }

    WindowKeyReleaseEvent::WindowKeyReleaseEvent(Data data): WindowKeyboardEvent(data)
    { }

    const char *WindowKeyReleaseEvent::name() const
    {
        return "keyrelease";
    }

    WindowMouseEvent::WindowMouseEvent(Data data):
        x(data.x), y(data.y), mouse_buttons(data.mouse_buttons)
    { }

    WindowMouseMoveEvent::WindowMouseMoveEvent(Data data): WindowMouseEvent(data)
    { }

    const char *WindowMouseMoveEvent::name() const
    {
        return "mousemove";
    }

    WindowMouseButtonPressEvent::WindowMouseButtonPressEvent(Data data):
        WindowMouseEvent(data.mouse), pressed_button(data.pressed_button)
    { }

    const char *WindowMouseButtonPressEvent::name() const
    {
        return "mousebuttonpress";
    }

    WindowMouseButtonReleaseEvent::WindowMouseButtonReleaseEvent(Data data):
        WindowMouseEvent(data.mouse), released_button(data.released_button)
    { }

    const char *WindowMouseButtonReleaseEvent::name() const
    {
        return "mousebuttonrelease";
    }
}
