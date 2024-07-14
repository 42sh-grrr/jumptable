#pragma once

#include <atomic>
#include <mutex>
#include "logger/level.hh"
#include "saltus/resource_pool.hh"

namespace saltus
{
    template <class T>
    ResourcePool<T>::Handle::Handle(
        size_t index,
        ResourcePool<T> *pool,
        T *ptr
    ):
        index_(index),
        pool_(pool),
        ptr_(ptr)
    {
        pool->handle_counter.fetch_add(1, std::memory_order_relaxed);
    }

    template <class T>
    ResourcePool<T>::Handle::~Handle()
    {
        pool_->release(index_);
        pool_->handle_counter.fetch_sub(1, std::memory_order_relaxed);
    }

    template <class T>
    const T *ResourcePool<T>::Handle::get() const
    {
        return ptr_;
    }

    template <class T>
    T *ResourcePool<T>::Handle::get()
    {
        return ptr_;
    }

    template <class T>
    const T *ResourcePool<T>::Handle::operator->() const
    {
        return ptr_;
    }

    template <class T>
    T *ResourcePool<T>::Handle::operator->()
    {
        return ptr_;
    }

    template <class T>
    const T &ResourcePool<T>::Handle::operator*() const
    {
        return *ptr_;
    }

    template <class T>
    T &ResourcePool<T>::Handle::operator*()
    {
        return *ptr_;
    }

    template <class T>
    ResourcePool<T>::ResourcePool(Factory factory):
        factory_(factory)
    { }

    template <class T>
    ResourcePool<T>::~ResourcePool() noexcept(false)
    {
        if (handle_counter.load(std::memory_order_relaxed) > 0)
        {
            logger::error() << "A resource pool was destroyed with still active handles, use after free is guarenteed\n";
            // There is no way correct usage can be happening
            // a throw is required
            throw ResourcePoolInUseError();
        }
    }

    template <class T>
    ResourcePool<T>::Handle ResourcePool<T>::get()
    {
        std::lock_guard lock(resources_mutex_);
        if (resources_.size() == 0 || free_resources_.size() == 0)
        {
            size_t index = resources_.size();
            resources_.push_back(factory_());
            return Handle(index, this, resources_.at(index).get());
        }
        size_t available = free_resources_.front();
        free_resources_.pop();
        return Handle(available, this, resources_.at(available).get());
    }

    template <class T>
    std::vector<std::unique_ptr<T>> ResourcePool<T>::drain()
    {
        std::lock_guard lock(resources_mutex_);
        if (handle_counter.load(std::memory_order_relaxed) != 0)
            throw ResourcePoolInUseError();

        std::vector<std::unique_ptr<T>> out = std::move(resources_);
        resources_ = std::vector<std::unique_ptr<T>>{};
        return out;
    }

    template <class T>
    void ResourcePool<T>::release(size_t index)
    {
        std::lock_guard lock(resources_mutex_);
        free_resources_.push(index);
    }
} // namespace saltus
