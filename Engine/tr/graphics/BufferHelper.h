#pragma once
#include <tr.h>

#include "../util/Exceptions.h"
#include "GLSLShader.h"
#include "InterfaceTypes.h"

#include <any>
#include <list>

namespace tr::detail {

/**
 * A Buffer Representing Some data on the gpu
 */
class Buffer {
public:
    using Locality = BufferLocality;

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

    /**
     * Returns the size in bytes of the buffer.
     */
    size_t GetBufferSize() const { return mDataSize; }

private:
    uint       mHandle = 0;
    BufferType mType;
    Locality   mLocality;
    size_t     mDataSize;
};

/**
 * Represents a layout of a buffer.
 */
class BufferLayout {
public:
    using ElementType = ShaderElementType;

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
     * @param type The type of one component.
     * @param[in] offset The offset from the previous elements.
     * @param normalized Wheter to normalize float values.
     */
    void
    Add(uint index, ElementType type, void *offset, bool normalized = false);

    /**
     * Apply this layout to an buffer.
     * Note its recommended to bind a AttributBufferStore first.
     */
    void ApplyTo(Buffer &buffer);

    /**
     * Adds the attributes described by this to the interface.
     */
    void AddAttributesToShaderInterface(ShaderInterface &interface) const;

private:
    // Index type normalized offset
    using Data            = std::tuple<uint, ElementType, bool, void *>;
    size_t          mSize = 0;
    std::list<Data> mAttribPointers;
};

/**
 * Stores the attirbutes of Buffers.
 * -> VAO
 */
class AttributBufferStore {
public:
    /**
     * Default constructor
     */
    AttributBufferStore() = default;

    /**
     * Deleted copy constructor
     */
    AttributBufferStore(const AttributBufferStore &) = delete;

    /**
     * Deleted assignment copy
     */
    AttributBufferStore &operator=(const AttributBufferStore &) = delete;

    /**
     * Deleted assignemt move.
     */
    AttributBufferStore &operator=(AttributBufferStore &&other) = delete;

    /**
     * Move constructor
     */
    AttributBufferStore(AttributBufferStore &&other)
        : mHandle(std::move(other.mHandle))
        , mBuffers(std::move(other.mBuffers))
    {
        other.mHandle = 0;
        other.mBuffers.clear();
    }

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
    uint              mHandle = 0;
    std::list<Buffer> mBuffers;
};
}
