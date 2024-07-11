#pragma once

#include <stdexcept>
#include "saltus/vulkan/frame_resource.hh"

namespace saltus::vulkan
{
    // Meline:
    // ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt
    // ytttttttttttttttttttttttttttttttttttttttttttttttttttbigredrfetgtytductfgdfdttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt

    inline GenericFrameResource::~GenericFrameResource() = default;
    
    template <class T>
    FrameResource<T>::FrameResource(Factory factory):
        inner_(std::make_shared<inner>())
    {
        inner_->factory = factory;
    }

    template <class T>
    FrameResource<T>::FrameResource(FrameResource&& other):
        inner_(other.inner_)
    { }

    template <class T>
    FrameResource<T>::FrameResource(const FrameResource& other):
        inner_(other.inner_)
    { }

    template <class T>
    FrameResource<T>& FrameResource<T>::operator=(const FrameResource& other)
    {
        inner_ = other.inner_;
        return *this;
    }

    template <class T>
    FrameResource<T>& FrameResource<T>::operator=(FrameResource&& other)
    {
        inner_ = other.inner_;
        return *this;
    }

    template <class T>
    T &FrameResource<T>::get(uint32_t frame_index)
    {
        if (frame_index >= MAX_FRAMES_IN_FLIGHT)
            throw std::range_error("Frame index is out of range");
        if (!inner_->values[frame_index])
            inner_->values[frame_index] = inner_->factory(frame_index);
        return *inner_->values[frame_index];
    }

    template <class T>
    const T &FrameResource<T>::get(uint32_t frame_index) const
    {
        if (frame_index >= MAX_FRAMES_IN_FLIGHT)
            throw std::range_error("Frame index is out of range");
        if (!inner_->values[frame_index])
            inner_->values[frame_index] = inner_->factory(frame_index);
        return *inner_->values[frame_index];
    }

    template <class T>
    void FrameResource<T>::free(uint32_t frame_index)
    {
        inner_->values[frame_index].reset();
    }

    template <class T>
    void FrameResource<T>::clear()
    {
        for (auto &fi : inner_->values)
            fi.reset();
    }

    template <class T>
    FrameResource<T>::operator WeakFrameResource<T>()
    {
        return WeakFrameResource<T>(*this);
    }

    template <class T>
    WeakFrameResource<T>::WeakFrameResource(FrameResource<T>& res):
        inner_(res.inner_)
    { }

    template <class T>
    WeakFrameResource<T>::WeakFrameResource(WeakFrameResource&& other):
        inner_(other.inner_)
    { }

    template <class T>
    void WeakFrameResource<T>::free(uint32_t frame_index)
    {
        inner_->values[frame_index].reset();
    }
}

