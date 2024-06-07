#include "graphics/window_events.hh"

namespace graphics {
    const char *WindowEventExpose::name() const
    {
        return "expose";
    }

    WindowEventExpose::WindowEventExpose(Data data):
        width(data.width), height(data.height), x(data.x), y(data.y)
    {}
    
    WindowEventKeyboard::WindowEventKeyboard(Data data):
        keycode(data.keycode)
    {}
    
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
}
