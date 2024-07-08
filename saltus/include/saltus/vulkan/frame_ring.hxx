#pragma once

#include "saltus/vulkan/frame_resource.hh"
#include "saltus/vulkan/frame_ring.hh"

#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    template <class T>
    FrameResource<T> FrameRing::allocate_resource(typename FrameResource<T>::Factory factory)
    {
        FrameResource<T> resource{ factory };

        resources_.push_back(std::make_unique<FrameResource<T>>(resource));

        return resource;
    }
} // namespace saltus::vulkan


