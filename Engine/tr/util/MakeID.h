#pragma once
#include <tr.h>

namespace tr {
class MakeID {
public:
    explicit MakeID(const uint max_id);

    MakeID(const MakeID &) = delete;
    MakeID &operator=(const MakeID &) = delete;

    ~MakeID() { ::free(m_Ranges); }

    bool CreateID(uint &id);
    bool CreateRangeID(uint &id, const uint count);

    inline bool DestroyID(const uint id) { return DestroyRangeID(id, 1); }
    bool        DestroyRangeID(const uint id, const uint count);

    bool IsID(const uint id) const;

    uint GetAvailableIDs() const;
    uint GetLargestContinuousRange() const;

private:
    struct Range {
        uint m_First;
        uint m_Last;
    };

    Range *m_Ranges; // Sorted array of ranges of free IDs
    uint   m_Count; // Number of ranges in list
    uint   m_Capacity; // Total capacity of range list

    void InsertRange(const uint index);
    void DestroyRange(const uint index);
};
}

