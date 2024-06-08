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
        if (!event)
            continue;

        if (auto *expose = dynamic_cast<graphics::WindowEventExpose*>(&*event))
        {
            std::cout << "Expose <size " << expose->width << "x" << expose->height << ">\n";
        }
        if (auto *keypress = dynamic_cast<graphics::WindowEventKeyPress*>(&*event))
        {
            // ESC
            if (keypress->keycode == 9)
                break;
        }
        // if (auto *buttonpress = dynamic_cast<graphics::WindowEventMouseButtonPress*>(&*event))
        // {
        //     std::cout << "Button pressed: " << buttonpress->pressed_button
        //     << ", current buttons: " << static_cast<int>(buttonpress->mouse_buttons) << "\n";
        // }
        if (auto *buttonrelease = dynamic_cast<graphics::WindowEventMouseButtonRelease*>(&*event))
        {
            std::cout << "Button release: " << buttonrelease->released_button
            << ", current buttons: " << static_cast<int>(buttonrelease->mouse_buttons)
            << ", val: " << (1 << (buttonrelease->released_button-1))
            << "\n";
        }
    }

    sleep(1);

    return 0;
}
