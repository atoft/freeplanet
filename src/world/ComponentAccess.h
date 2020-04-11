//
// Created by alastair on 11/04/2020.
//

#pragma once

#include <src/world/WorldZone.h>

class ComponentAccess
{
public:
    template <typename T>
    static const T* GetComponent(const WorldObject& _object)
    {
        return _object.GetWorldZone()->FindComponent<T>(_object.GetComponentRef<T>());
    }

    template <typename T>
    static T* GetComponent(WorldObject& _object)
    {
        return const_cast<T*>(GetComponent<T>(static_cast<const WorldObject&>(_object)));
    }
};