
#include "WorldObjectDirectory.h"

WorldObjectID WorldObjectDirectory::RegisterWorldObject(WorldObjectRef _worldObjectRef)
{
    assert(m_NextAvailableID < std::numeric_limits<WorldObjectID>::max());

    m_Map[m_NextAvailableID] = _worldObjectRef;

    return m_NextAvailableID++;
}

void WorldObjectDirectory::OnWorldObjectTransferred(WorldObjectID _objectID, WorldObjectRef _newLocation)
{
    auto&& result = m_Map.find(_objectID);
    if(result != m_Map.end())
    {
        result->second = _newLocation;
    }
    else
    {
        LogError("Tried to transfer a WorldObject ID" + std::to_string(_objectID) + " that does not exist.");

        // We may want to handle this gracefully and continue.
        // For now let's assert here to make bugs easier to detect.
        assert(false);
    }
}

void WorldObjectDirectory::UnregisterWorldObject(WorldObjectID _objectID)
{
    u32 eraseCount = m_Map.erase(_objectID);

    if(eraseCount == 0)
    {
        LogError("Tried to erase a WorldObject ID" + std::to_string(_objectID) + " that does not exist.");
    }

    assert(eraseCount == 1);
}

WorldObjectRef WorldObjectDirectory::GetWorldObjectLocation(WorldObjectID _objectID) const
{
    auto&& result = m_Map.find(_objectID);
    if(result != m_Map.end())
    {
        return result->second;
    }
    else
    {
        return WorldObjectRef(glm::ivec3(), REF_INVALID);
    }
}
