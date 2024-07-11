#pragma once

#include <functional>
#include <memory>

#include "saltus/vulkan/config.hh"

namespace saltus::vulkan
{
    /// Used for storing frame resources of whatever types
    /// Can be a weak resource or strong one
    class GenericFrameResource
    {
    public:
        virtual ~GenericFrameResource() = 0;

        virtual void free(uint32_t frame_index) = 0;

    protected:

    private:
    };

    template <class T>
    class WeakFrameResource;

    template <class T>
    class FrameResource: public GenericFrameResource
    {
    public:
        using Factory = std::function<std::unique_ptr<T>(uint32_t)>;

        FrameResource(Factory factory);
        FrameResource(FrameResource&&);
        FrameResource(const FrameResource&);
        FrameResource& operator=(const FrameResource&);
        FrameResource& operator=(FrameResource&&);

        T &get(uint32_t frame_index);
        const T &get(uint32_t frame_index) const;

        void free(uint32_t frame_index) override;
        void clear();

        operator WeakFrameResource<T>();

    protected:
        friend WeakFrameResource<T>;

        struct inner {
            Factory factory;
            std::array<std::unique_ptr<T>, MAX_FRAMES_IN_FLIGHT> values;
        };

    private:
        std::shared_ptr<inner> inner_;
    };

    template <class T>
    class WeakFrameResource: public GenericFrameResource
    {
    public:
        WeakFrameResource(FrameResource<T>& res);
        WeakFrameResource(WeakFrameResource&&);

        WeakFrameResource(const WeakFrameResource&) = delete;
        WeakFrameResource& operator=(const WeakFrameResource&) = delete;
        WeakFrameResource& operator=(WeakFrameResource&&) = delete;

        void free(uint32_t frame_index) override;

    private:
        std::weak_ptr<typename FrameResource<T>::inner> inner_;
    };
}

#include <saltus/vulkan/frame_resource.hxx>
