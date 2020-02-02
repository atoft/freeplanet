
#pragma once

#include <unordered_map>
#include <src/tools/globals.h>
#include <src/world/WorldObjectID.h>

class WorldObjectDirectory
{
public:
    WorldObjectID RegisterWorldObject(WorldObjectRef _worldObjectRef);
    void OnWorldObjectTransferred(WorldObjectID _objectID, WorldObjectRef _newLocation);
    void UnregisterWorldObject(WorldObjectID _objectID);

    const WorldObjectRef GetWorldObjectLocation(WorldObjectID _objectID) const;

private:
    std::unordered_map<WorldObjectID, WorldObjectRef> m_Map;

    WorldObjectID m_NextAvailableID = 1;

};

