#include <iostream>
#include <matrix/matrix.hh>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <saltus/renderer.hh>
#include <unistd.h>

int main()
{
    auto window = saltus::WindowBuilder()
        .title("bite")
        .build();
    auto renderer = saltus::Renderer::create(window);

    for (;;)
    {
        auto event = window.wait_event();
        if (!event)
            continue;

        if (dynamic_cast<saltus::WindowExposeEvent*>(&*event))
        {
            std::cout << "Rendering...\n";
            renderer->render();
            std::cout << "Finished rendering !\n";
            continue;
        }

        if (dynamic_cast<saltus::WindowCloseRequestEvent*>(&*event))
            break;
    }

    renderer->wait_for_idle();

    std::cout << "Bye bye !\n";

    return 0;
}
