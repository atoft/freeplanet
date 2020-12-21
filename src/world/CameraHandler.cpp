/* 
 * Copyright 2020 Alastair Toft
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CameraHandler.h"

#include <src/tools/globals.h>
#include <src/profiling/Profiler.h>
#include <src/world/ComponentAccess.h>
#include <src/world/FreelookCameraComponent.h>
#include <src/world/World.h>
#include <src/world/WorldZone.h>

CameraHandler::CameraHandler(World* _world)
    : m_World(_world)
{}

void CameraHandler::Update(TimeMS _delta)
{
    ProfileCurrentFunction();
    
    for (WorldZone& zone : m_World->GetActiveZones())
    {
        for (FreelookCameraComponent& component : zone.GetComponents<FreelookCameraComponent>())
        {
            const glm::vec3 LOCAL_UP = glm::vec3(0.f, 1.f, 0.f);

            // Freelook left and right causes the parent object to rotate,
            // but up and down should only rotate the camera view
            WorldObject* self = component.GetOwnerObject();
            self->Rotate(LOCAL_UP, component.m_AngularVelocity.x);

            component.m_VerticalRotation = glm::clamp(component.m_VerticalRotation + component.m_AngularVelocity.y, -glm::half_pi<f32>(), glm::half_pi<f32>());
        }
    }
}

void CameraHandler::OnMouseInput(u32 _playerIdx, WorldObject* _controlledWorldObject, f32 _x, f32 _y)
{
    if (_controlledWorldObject == nullptr)
    {
        return;
    }

    FreelookCameraComponent* component = ComponentAccess::GetComponent<FreelookCameraComponent>(*_controlledWorldObject);

    if (component == nullptr)
    {
        return;
    }
    
    constexpr f32 sensX = 2.f;
    constexpr f32 sensY = 2.f;
    
    component->m_AngularVelocity = glm::vec3(-1.f, 0.f, 0.f) * _x * sensX - glm::vec3(0.f, 1.f, 0.f) * _y * sensY;
}

