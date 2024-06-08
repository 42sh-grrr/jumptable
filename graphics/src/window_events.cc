#include "graphics/window_events.hh"

namespace graphics {
    const char *WindowEventExpose::name() const
    {
        return "expose";
    }

    WindowEventExpose::WindowEventExpose(Data data):
        width(data.width), height(data.height)
    { }
    
    WindowEventKeyboard::WindowEventKeyboard(Data data):
        keycode(data.keycode)
    { }
    
    WindowEventKeyPress::WindowEventKeyPress(Data data): WindowEventKeyboard(data)
    { }
    
    const char *WindowEventKeyPress::name() const
    {
        return "keypress";
    }

    WindowEventKeyRelease::WindowEventKeyRelease(Data data): WindowEventKeyboard(data)
    { }

    const char *WindowEventKeyRelease::name() const
    {
        return "keyrelease";
    }

    WindowEventMouse::WindowEventMouse(Data data):
        x(data.x), y(data.y), mouse_buttons(data.mouse_buttons)
    { }

    WindowEventMouseMove::WindowEventMouseMove(Data data): WindowEventMouse(data)
    { }

    const char *WindowEventMouseMove::name() const
    {
        return "mousemove";
    }

    WindowEventMouseButtonPress::WindowEventMouseButtonPress(Data data):
        WindowEventMouse(data.mouse), pressed_button(data.pressed_button)
    { }

    const char *WindowEventMouseButtonPress::name() const
    {
        return "mousebuttonpress";
    }

    WindowEventMouseButtonRelease::WindowEventMouseButtonRelease(Data data):
        WindowEventMouse(data.mouse), released_button(data.released_button)
    { }

    const char *WindowEventMouseButtonRelease::name() const
    {
        return "mousebuttonrelease";
    }
}
