#pragma once

#include <cstdint>

namespace saltus
{
    class WindowEvent
    {
    public:
        virtual ~WindowEvent() = default;

        virtual const char *name() const = 0;
    private:
    };

    class WindowExposeEvent: public WindowEvent
    {
    public:
        struct Data {
            int width, height;
        };
        int width, height;

        WindowExposeEvent(Data data);

        const char *name() const override;
    };

    class WindowKeyboardEvent: public WindowEvent
    {
    public:
        struct Data {
            uint8_t keycode;
        };
        uint8_t keycode;

        WindowKeyboardEvent(Data data);
    };

    class WindowKeyPressEvent: public WindowKeyboardEvent
    {
    public:
        WindowKeyPressEvent(Data data);

        const char *name() const override;
    };

    class WindowKeyReleaseEvent: public WindowKeyboardEvent
    {
    public:
        WindowKeyReleaseEvent(Data data);

        const char *name() const override;
    };

    class WindowMouseEvent: public WindowEvent
    {
    public:
        struct Data {
            int x, y;
            int root_x, root_y;
            uint8_t mouse_buttons;
        };
        WindowMouseEvent(Data data);

        int x, y;
        /// "Desktop" position
        int root_x, root_y;
        /// bitfield with one bit for each mouse button indicating wether it is
        /// pressed or not
        uint8_t mouse_buttons;
    };

    class WindowMouseMoveEvent: public WindowMouseEvent
    {
    public:
        WindowMouseMoveEvent(Data data);

        const char *name() const override;
    };

    class WindowMouseButtonPressEvent: public WindowMouseEvent
    {
    public:
        struct Data {
            WindowMouseEvent::Data mouse;
            int pressed_button;
        };
        /// Cautious: Starts at one (not like the pressed_button bitfield)
        int pressed_button;
        
        WindowMouseButtonPressEvent(Data data);

        const char *name() const override;
    };

    class WindowMouseButtonReleaseEvent: public WindowMouseEvent
    {
    public:
        struct Data {
            WindowMouseEvent::Data mouse;
            int released_button;
        };
        /// Cautious: Starts at one (not like the pressed_button bitfield)
        int released_button;
        
        WindowMouseButtonReleaseEvent(Data data);

        const char *name() const override;
    };
} // namespace saltus
