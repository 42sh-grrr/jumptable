#include <iostream>
#include <matrix/matrix.hh>
#include <graphics/graphics.hh>
#include <graphics/window.hh>
#include <memory>
#include <unistd.h>
#include "graphics/window_events.hh"

int main()
{
    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    graphics::Window window = graphics::WindowBuilder()
        .title("bite")
        .build();

    for (;;)
    {
        std::unique_ptr<graphics::WindowEvent> event = window.wait_event();

        if (auto *expose = dynamic_cast<graphics::WindowEventExpose*>(&*event))
        {
            std::cout << "Expose <pos " << expose->x << ", " << expose->y
                      << "> <size " << expose->width << "x" << expose->height << ">\n";
        }
        if (auto *keypress = dynamic_cast<graphics::WindowEventKeyPress*>(&*event))
        {
            // ESC
            if (keypress->keycode == 9)
                break;
        }
    }

    sleep(1);

    return 0;
}
