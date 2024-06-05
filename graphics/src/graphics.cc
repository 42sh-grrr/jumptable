#include "graphics/graphics.hh"

#include <iostream>

namespace graphics
{
    Graphics::Graphics()
    {
        std::cout << "Hi !\n";
    }

    Graphics::~Graphics() 
    {
        std::cout << "Bie !\n";
    }
} // namespace graphics
