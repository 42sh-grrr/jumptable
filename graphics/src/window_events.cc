#include "graphics/window_events.hh"

namespace graphics {
    const char *WindowEventExpose::name() const {
        return "expose";
    }

    WindowEventExpose::WindowEventExpose(Data data):
        width(data.width), height(data.height), x(data.x), y(data.y)
    {}
}
