#include "MakeID.h"

tr::MakeID::MakeID(const uint max_id)
{
    // Start with a single range, from 0 to max allowed ID (specified)
    m_Ranges            = static_cast<Range *>(::malloc(sizeof(Range)));
    m_Ranges[0].m_First = 0;
    m_Ranges[0].m_Last  = max_id;
    m_Count             = 1;
    m_Capacity          = 1;
}

bool tr::MakeID::CreateID(uint &id)
{
    if (m_Ranges[0].m_First <= m_Ranges[0].m_Last) {
        id = m_Ranges[0].m_First;

        // If current range is full and there is another one, that will
        // become the new current range
        if (m_Ranges[0].m_First == m_Ranges[0].m_Last && m_Count > 1) {
            DestroyRange(0);
        } else {
            ++m_Ranges[0].m_First;
        }
        return true;
    }

    // No availble ID left
    return false;
}

bool tr::MakeID::CreateRangeID(uint &id, const uint count)
{
    uint i = 0;
    do {
        const uint range_count = 1 + m_Ranges[i].m_Last - m_Ranges[i].m_First;
        if (count <= range_count) {
            id = m_Ranges[i].m_First;

            // If current range is full and there is another one, that will
            // become the new current range
            if (count == range_count && i + 1 < m_Count) {
                DestroyRange(i);
            } else {
                m_Ranges[i].m_First += count;
            }
            return true;
        }
        ++i;
    } while (i < m_Count);

    // No range of free IDs was large enough to create the requested
    // continuous ID sequence
    return false;
}
bool tr::MakeID::DestroyRangeID(const uint id, const uint count)
{
    const uint end_id = id + count;

    // Binary search of the range list
    uint i0 = 0;
    uint i1 = m_Count - 1;

    for (;;) {
        const uint i = (i0 + i1) / 2;

        if (id < m_Ranges[i].m_First) {
            // Before current range, check if neighboring
            if (end_id >= m_Ranges[i].m_First) {
                if (end_id != m_Ranges[i].m_First)
                    return false; // Overlaps a range of free IDs, thus (at
                                  // least partially) invalid IDs

                // Neighbor id, check if neighboring previous range too
                if (i > i0 && id - 1 == m_Ranges[i - 1].m_Last) {
                    // Merge with previous range
                    m_Ranges[i - 1].m_Last = m_Ranges[i].m_Last;
                    DestroyRange(i);
                } else {
                    // Just grow range
                    m_Ranges[i].m_First = id;
                }
                return true;
            } else {
                // Non-neighbor id
                if (i != i0) {
                    // Cull upper half of list
                    i1 = i - 1;
                } else {
                    // Found our position in the list, insert the deleted
                    // range here
                    InsertRange(i);
                    m_Ranges[i].m_First = id;
                    m_Ranges[i].m_Last  = end_id - 1;
                    return true;
                }
            }
        } else if (id > m_Ranges[i].m_Last) {
            // After current range, check if neighboring
            if (id - 1 == m_Ranges[i].m_Last) {
                // Neighbor id, check if neighboring next range too
                if (i < i1 && end_id == m_Ranges[i + 1].m_First) {
                    // Merge with next range
                    m_Ranges[i].m_Last = m_Ranges[i + 1].m_Last;
                    DestroyRange(i + 1);
                } else {
                    // Just grow range
                    m_Ranges[i].m_Last += count;
                }
                return true;
            } else {
                // Non-neighbor id
                if (i != i1) {
                    // Cull bottom half of list
                    i0 = i + 1;
                } else {
                    // Found our position in the list, insert the deleted
                    // range here
                    InsertRange(i + 1);
                    m_Ranges[i + 1].m_First = id;
                    m_Ranges[i + 1].m_Last  = end_id - 1;
                    return true;
                }
            }
        } else {
            // Inside a free block, not a valid ID
            return false;
        }
    }
}
bool tr::MakeID::IsID(const uint id) const
{
    // Binary search of the range list
    uint i0 = 0;
    uint i1 = m_Count - 1;

    for (;;) {
        const uint i = (i0 + i1) / 2;

        if (id < m_Ranges[i].m_First) {
            if (i == i0)
                return true;

            // Cull upper half of list
            i1 = i - 1;
        } else if (id > m_Ranges[i].m_Last) {
            if (i == i1)
                return true;

            // Cull bottom half of list
            i0 = i + 1;
        } else {
            // Inside a free block, not a valid ID
            return false;
        }
    }
}

tr::uint tr::MakeID::GetAvailableIDs() const
{
    uint count = m_Count;
    uint i     = 0;

    do {
        count += m_Ranges[i].m_Last - m_Ranges[i].m_First;
        ++i;
    } while (i < m_Count);

    return count;
}

tr::uint tr::MakeID::GetLargestContinuousRange() const
{
    uint max_count = 0;
    uint i         = 0;

    do {
        uint count = m_Ranges[i].m_Last - m_Ranges[i].m_First + 1;
        if (count > max_count)
            max_count = count;

        ++i;
    } while (i < m_Count);

    return max_count;
}

void tr::MakeID::InsertRange(const uint index)
{
    if (m_Count >= m_Capacity) {
        m_Capacity += m_Capacity;
        m_Ranges = (Range *)realloc(m_Ranges, m_Capacity * sizeof(Range));
    }

    ::memmove(m_Ranges + index + 1, m_Ranges + index,
              (m_Count - index) * sizeof(Range));
    ++m_Count;
}

void tr::MakeID::DestroyRange(const uint index)
{
    --m_Count;
    ::memmove(m_Ranges + index, m_Ranges + index + 1,
              (m_Count - index) * sizeof(Range));
}
