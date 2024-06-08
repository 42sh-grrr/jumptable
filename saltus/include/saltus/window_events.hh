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

    class WindowEventExpose: public WindowEvent
    {
    public:
        struct Data {
            int width, height;
        };
        int width, height;

        WindowEventExpose(Data data);

        const char *name() const override;
    };

    class WindowEventKeyboard: public WindowEvent
    {
    public:
        struct Data {
            uint8_t keycode;
        };
        uint8_t keycode;

        WindowEventKeyboard(Data data);
    };

    class WindowEventKeyPress: public WindowEventKeyboard
    {
    public:
        WindowEventKeyPress(Data data);

        const char *name() const override;
    };

    class WindowEventKeyRelease: public WindowEventKeyboard
    {
    public:
        WindowEventKeyRelease(Data data);

        const char *name() const override;
    };

    class WindowEventMouse: public WindowEvent
    {
    public:
        struct Data {
            int x, y;
            int root_x, root_y;
            uint8_t mouse_buttons;
        };
        WindowEventMouse(Data data);

        int x, y;
        /// "Desktop" position
        int root_x, root_y;
        /// bitfield with one bit for each mouse button indicating wether it is
        /// pressed or not
        uint8_t mouse_buttons;
    };

    class WindowEventMouseMove: public WindowEventMouse
    {
    public:
        WindowEventMouseMove(Data data);

        const char *name() const override;
    };

    class WindowEventMouseButtonPress: public WindowEventMouse
    {
    public:
        struct Data {
            WindowEventMouse::Data mouse;
            int pressed_button;
        };
        /// Cautious: Starts at one (not like the pressed_button bitfield)
        int pressed_button;
        
        WindowEventMouseButtonPress(Data data);

        const char *name() const override;
    };

    class WindowEventMouseButtonRelease: public WindowEventMouse
    {
    public:
        struct Data {
            WindowEventMouse::Data mouse;
            int released_button;
        };
        /// Cautious: Starts at one (not like the pressed_button bitfield)
        int released_button;
        
        WindowEventMouseButtonRelease(Data data);

        const char *name() const override;
    };
} // namespace graphics

