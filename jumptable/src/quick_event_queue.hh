#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore>
#include <vector>

template<class T>
class QuickEventQueue
{
public:
    QuickEventQueue():
        listeners()
    { }

    QuickEventQueue(const QuickEventQueue&) = delete;
    QuickEventQueue(QuickEventQueue&&) = delete;
    QuickEventQueue& operator=(const QuickEventQueue&) = delete;
    QuickEventQueue& operator=(QuickEventQueue&&) = delete;

    class Listener
    {
    public:
        Listener():
            wait_semaphore(0)
        { }
        Listener(const Listener&) = delete;
        Listener(Listener&&) = delete;
        Listener& operator=(const Listener&) = delete;
        Listener& operator=(Listener&&) = delete;

        size_t in_queue()
        {
            const std::lock_guard<std::mutex> lock(queue_mutex);
            return queue.size();
        }

        T recv()
        {
            for (;;)
            {
                if (std::optional<T> event = poll_recv())
                    return event.get();

                wait_semaphore.acquire();
            }
        }

        std::optional<T> poll_recv()
        {
            const std::lock_guard<std::mutex> lock(queue_mutex);
            if (queue.size() == 0)
                return std::nullopt;
            T b = queue.front();
            queue.pop();
            return b;
        }

    protected:
        friend QuickEventQueue;

        std::queue<T> queue;
        std::mutex queue_mutex;

        std::binary_semaphore wait_semaphore;
    private:
    };

    void send(T event)
    {
        for (const std::weak_ptr<Listener> &listener : listeners)
        {
            std::shared_ptr<Listener> v = listener.lock();
            if (!v)
                continue;
            v->queue_mutex.lock();
            v->queue.push(event);
            v->queue_mutex.unlock();
            v->wait_semaphore.release();
        }
    }

    std::shared_ptr<Listener> subscribe()
    {
        std::shared_ptr<Listener> listener = std::make_shared<Listener>();
        listeners_mutex.lock();
        listeners.push_back(listener);
        listeners_mutex.unlock();
        return listener;
    }

private:
    std::vector<std::weak_ptr<Listener>> listeners;
    std::mutex listeners_mutex;
};
