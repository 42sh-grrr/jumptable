#include <iostream>
#include <matrix/matrix.hh>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <memory>
#include <unistd.h>

int main()
{
    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    saltus::Window window = saltus::WindowBuilder()
        .title("bite")
        .build();

    for (;;)
    {
        std::unique_ptr<saltus::WindowEvent> event = window.wait_event();
        if (!event)
            continue;

        if (auto *expose = dynamic_cast<saltus::WindowEventExpose*>(&*event))
        {
            std::cout << "Expose <size " << expose->width << "x" << expose->height << ">\n";
        }
        if (auto *keypress = dynamic_cast<saltus::WindowEventKeyPress*>(&*event))
        {
            // ESC
            if (keypress->keycode == 9)
                break;
        }
        // if (auto *buttonpress = dynamic_cast<saltus::WindowEventMouseButtonPress*>(&*event))
        // {
        //     std::cout << "Button pressed: " << buttonpress->pressed_button
        //     << ", current buttons: " << static_cast<int>(buttonpress->mouse_buttons) << "\n";
        // }
        if (auto *buttonrelease = dynamic_cast<saltus::WindowEventMouseButtonRelease*>(&*event))
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
