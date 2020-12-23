/*
 * Copyright 2017-2020 Alastair Toft
 *
 * This file is part of freeplanet.
 *
 * freeplanet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeplanet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeplanet. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FreelookCameraComponent.h"

#include <src/tools/MathsHelpers.h>
#include <src/profiling/Profiler.h>
#include <src/world/WorldObject.h>
#include <src/engine/Engine.h>

FreelookCameraComponent::FreelookCameraComponent(World *ownerWorld, WorldObjectID _ownerID)
{
    m_World = ownerWorld;
    m_WorldObjectID = _ownerID;
}

f32 FreelookCameraComponent::GetFarClipDistance() const
{
    return Globals::FREEPLANET_FAR_CLIP_DISTANCE;
}

glm::mat4 FreelookCameraComponent::GetCameraZoneTransform() const
{
    const WorldObject* self = GetOwnerObject();

    if(self == nullptr)
    {
        LogError("Couldn't find camera's owner");
        return glm::mat4(1.f);
    }

    const glm::vec3 LOCAL_RIGHT = glm::vec3(1,0,0);
    return glm::rotate(self->GetZoneTransform(), m_VerticalRotation, LOCAL_RIGHT);
}

glm::vec3 FreelookCameraComponent::GetLookDirection() const
{
    return MathsHelpers::GetForwardVector(GetCameraZoneTransform());
}
