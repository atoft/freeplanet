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

    virtual void OnButtonInput(InputType _type){};
    virtual void OnMouseInput(float _mouseX, float _mouseY){};

    // Called when the owner WorldObject is removed from the World, i.e. when it is destroyed. Do not do this work in
    // the destructor because it may be called at other times, e.g. when reordering components in the WorldZone.
    virtual void OnRemovedFromWorld(){};

    virtual void Update(TimeMS _delta){};
    virtual void DebugDraw(UIDrawInterface& _interface) const {};

    WorldObjectID GetOwner() const { return m_WorldObjectID; };

    WorldObject* GetOwnerObject();
    const WorldObject* GetOwnerObject() const;

protected:
    // TODO remove world pointer from component.
    World* m_World;

    WorldObjectID m_WorldObjectID;
};
