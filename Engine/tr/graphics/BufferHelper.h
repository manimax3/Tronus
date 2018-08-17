#pragma once
#include <tr.h>

#include "../util/Exceptions.h"
#include "StaticMesh.h"

#include <any>
#include <queue>

namespace tr::detail {

/**
 * Different Types of GPU Buffers
 */
enum class BufferType { VERTEX, INDEX };

/**
 * A Buffer Representing Some data on the gpu
 */
class Buffer {
public:
    enum Locality { STATIC, STREAM, DYNAMIC };

    explicit Buffer(BufferType type, Locality l)
        : mType(type)
        , mLocality(l)
    {
    }

    enum MapType { WRITE, READ, ReWr };

    /**
     * Create a new buffer
     */
    void Create(size_t element_size, size_t count, void *data = nullptr);

    /**
     * Destroy a buffer.
     * Note the destructor does not destoy a buffer.
     */
    void Destroy();

    /**
     * Update a region of the buffer
     */
    void Update(size_t size, int offset, void *data)
    {
        throw NotImplementedError("Buffer::Update");
    }

    /**
     * Map the buffer to memory
     */
    void *MapBuffer(MapType type)
    {
        throw NotImplementedError("Buffer::MapBuffer");
    }

    BufferType GetType() const { return mType; }
    std::any   GetHandle() { return mHandle; }

    /**
     * Check if the buffer is valid
     */
    operator bool() const;

    /**
     * Bind the buffer to the gpu based on its type
     */
    void Bind();

    /**
     * Unbind the buffer
     */
    void Unbind();

private:
    std::any   mHandle = 0;
    BufferType mType;
    Locality   mLocality;
};

/**
 * Represents a layout of a buffer.
 */
class BufferLayout {
public:
    enum ElementType { FLOAT };

    /**
     * Constructor.
     * @param[in] size The stride from e.g. one vertex to the next
     */
    BufferLayout(size_t size) noexcept
        : mSize(size)
    {
    }

    /**
     * Add new element to the layout.
     * @param index The index of the attribute (used in the shaders)
     * @param components Of how many components does this elements consist of
     * @param type The type of one component.
     * @param[in] offset The offset from the previous elements.
     * @param normalized Wheter to normalize float values.
     */
    void Add(uint        index,
             int         components,
             ElementType type,
             void *      offset,
             bool        normalized = false);

    /**
     * Apply this layout to an buffer.
     * Note its recommended to bind a AttributBufferStore first.
     */
    void ApplyTo(Buffer &buffer);

private:
    // Index components type normalized offset
    using Data            = std::tuple<uint, int, ElementType, bool, void *>;
    size_t          mSize = 0;
    std::list<Data> mAttribPointers;
};

/**
 * Stores the attirbutes of Buffers.
 * -> VAO
 */
class AttributBufferStore {
public:
    // TODO:
    // Disable copyable
    // Special move semantics
    // defautl constructor

    /**
     * Setup the internal state
     */
    void Create();

    /**
     * Destroy the internal gpu state.
     * @param destroy_buffers Wheter to destroy the contained buffers too.
     */
    void Destroy(bool destory_buffers = true);

    /**
     * Bind the internal gpu state.
     */
    void Bind();

    /**
     * Unbind the internal gpu state.
     */
    void Unbind();

    /**
     * Adds a buffer to remember.
     */
    void AddBuffer(const Buffer &b);

    /**
     * Check if this represents a valid gpu state.
     */
    operator bool() const;

private:
    std::any          mHandle;
    std::list<Buffer> mBuffers;
};
}
