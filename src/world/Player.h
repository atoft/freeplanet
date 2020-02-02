//
// Created by alastair on 23/11/18.
//

#pragma once

#include <src/world/WorldObjectDirectory.h>

class Player
{
    // TODO pass inputs through a player

public:
    WorldObjectID GetControlledWorldObjectID() const { return m_ControlledWorldObject; };

    void AttachWorldObject(WorldObjectID _id) { m_ControlledWorldObject = _id; };

public:
    bool m_IsLocal = true;

private:
    WorldObjectID m_ControlledWorldObject = WORLDOBJECTID_INVALID;
};
