#pragma once

#include <atomic>

namespace tr {

template<typename T>
class MPSCQueue {
public:
    explicit MPSCQueue()
        : mHead(reinterpret_cast<buffer_node *>(new buffer_node_aligned))
        , mTail(mHead.load(std::memory_order_relaxed))
    {
        buffer_node *front = mHead.load(std::memory_order_relaxed);
        front->next.store(NULL, std::memory_order_relaxed);
    }

    MPSCQueue(const MPSCQueue &) = delete;
    MPSCQueue &operator=(const MPSCQueue &) = delete;

    MPSCQueue(MPSCQueue &&) = delete;
    MPSCQueue &operator=(MPSCQueue &&) = delete;

    ~MPSCQueue()
    {
        T output;
        while (this->dequeue(output))
            ;
        buffer_node *front = mHead.load(std::memory_order_relaxed);
        delete front;
    }

    bool dequeue(T &output)
    {
        buffer_node *tail = mTail.load(std::memory_order_relaxed);
        buffer_node *next = tail->next.load(std::memory_order_acquire);

        if (next == NULL) {
            return false;
        }

        output = next->data;
        mTail.store(next, std::memory_order_release);
        delete tail;
        return true;
    }

    void enqueue(const T &input)
    {
        buffer_node *node
            = reinterpret_cast<buffer_node *>(new buffer_node_aligned);
        node->data = input;
        node->next.store(NULL, std::memory_order_relaxed);

        buffer_node *prev_head
            = mHead.exchange(node, std::memory_order_acq_rel);
        prev_head->next.store(node, std::memory_order_release);
    }

private:
    struct buffer_node {
        T                          data;
        std::atomic<buffer_node *> next;
    };

    using buffer_node_aligned =
        typename std::aligned_storage<sizeof(buffer_node),
            std::alignment_of<buffer_node>::value>::type;

    std::atomic<buffer_node *> mHead;
    std::atomic<buffer_node *> mTail;
};
}
