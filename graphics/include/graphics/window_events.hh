#pragma once

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
} // namespace graphics

