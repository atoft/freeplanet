
#pragma once

#include <unordered_map>
#include <src/tools/globals.h>
#include <src/world/WorldObjectID.h>
#include <src/world/WorldObjectRef.h>

class WorldObjectDirectory
{
public:
    WorldObjectID RegisterWorldObject(WorldObjectRef _worldObjectRef);
    void OnWorldObjectTransferred(WorldObjectID _objectID, WorldObjectRef _newLocation);
    void UnregisterWorldObject(WorldObjectID _objectID);

    WorldObjectRef GetWorldObjectLocation(WorldObjectID _objectID) const;
    u32 GetCount() const { return m_Map.size(); };

private:
    std::unordered_map<WorldObjectID, WorldObjectRef> m_Map;

    WorldObjectID m_NextAvailableID = 1;

};
