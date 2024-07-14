#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace saltus
{
    class ResourcePoolInUseError: public std::runtime_error
    {
    public:
        ResourcePoolInUseError();

    private:
    };

    template <class T>
    class ResourcePool
    {
    public:
        using Factory = std::function<std::unique_ptr<T> ()>;

        class Handle
        {
        public:
            ~Handle();

            Handle(const Handle&) = delete;
            Handle(Handle&&) = delete;
            Handle& operator=(const Handle&) = delete;
            Handle& operator=(Handle&&) = delete;

            const T *get() const;
            T *get();

            const T *operator->() const;
            T *operator->();
            const T &operator*() const;
            T &operator*();

        protected:
            friend ResourcePool<T>;

            Handle(size_t, ResourcePool<T> *, T *);

        private:
            size_t index_;
            ResourcePool<T> *pool_;
            T *ptr_;
        };

        ResourcePool(Factory factory);
        ~ResourcePool() noexcept(false);

        ResourcePool(const ResourcePool&) = delete;
        ResourcePool(ResourcePool&&) = delete;
        ResourcePool& operator=(const ResourcePool&) = delete;
        ResourcePool& operator=(ResourcePool&&) = delete;

        /// Get an exclusive handle to an instance of the resource
        /// Automatically retired when Handle is dropped
        Handle get();

        /// Throws if any handle still exists
        /// Removes all resources constructed for the resource pool leaving it empty
        std::vector<std::unique_ptr<T>> drain();

    protected:
        friend Handle;

        void release(size_t index);

    private:
        Factory factory_;

        std::mutex resources_mutex_;
        std::vector<std::unique_ptr<T>> resources_;
        std::queue<size_t> free_resources_;

        std::atomic_int handle_counter;
    };

    template <class T>
    using ResourceHandle = ResourcePool<T>::Handle;
} // namespace saltus

#include <saltus/resource_pool.hxx>
