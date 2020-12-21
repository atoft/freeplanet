//
// Created by alastair on 14/09/17.
//

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <src/engine/InputTypes.h>
#include <src/graphics/ui/UIDrawInterface.h>
#include <src/tools/globals.h>
#include <src/world/WorldObjectDirectory.h>

class WorldObject;
class World;
class UIDisplay;

class WorldObjectComponent
{
public:
    virtual ~WorldObjectComponent() = default;

    WorldObjectID GetOwner() const { return m_WorldObjectID; };

    WorldObject* GetOwnerObject();
    const WorldObject* GetOwnerObject() const;

protected:
    // TODO remove world pointer from component.
    World* m_World;

    WorldObjectID m_WorldObjectID;
};
