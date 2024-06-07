#pragma once

#include <cstdint>

namespace graphics
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
            int x, y;
        };
        int width, height;
        int x, y;

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
} // namespace graphics

