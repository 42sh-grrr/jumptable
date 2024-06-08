#include <matrix/matrix.hh>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <unistd.h>
#include "saltus/renderer.hh"

int main()
{
    saltus::Window window = saltus::WindowBuilder()
        .title("bite")
        .build();

    auto renderer = saltus::Renderer::create(window);

    sleep(1);

    return 0;
}
