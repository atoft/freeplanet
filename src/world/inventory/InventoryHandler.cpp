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

#include "InventoryHandler.h"

#include <src/world/collision/CollisionHandler.h>
#include <src/world/World.h>

InventoryHandler::InventoryHandler(World *_world)
    : m_World(_world)
{}

void InventoryHandler::RegisterLocalPlayer(u32 _playerIndex)
{
    Inventory& defaultInventory = m_Inventories.emplace_back();

    defaultInventory.m_PlayerID = _playerIndex;
    defaultInventory.m_Slots =
        {
            {TerrainSubstanceType::Topsoil, 100},
            {TerrainSubstanceType::Dirt, 100},
            {TerrainSubstanceType::Rock, 100},
            {TerrainSubstanceType::Sand, 100},
        };

}

void InventoryHandler::OnButtonInput(InputType _inputType)
{
    constexpr u32 PLAYER_IDX = 0;

    if (_inputType == InputType::Interact || _inputType == InputType::InteractAlternate)
    {
        const WorldObjectID id = m_World->GetPlayerHandler()->GetControlledWorldObjectID(PLAYER_IDX);

        if (id == WORLDOBJECTID_INVALID)
        {
            return;
        }

        WorldObject* object = m_World->GetWorldObject(id);
        assert(object != nullptr);

        const FreelookCameraComponent* camera = ComponentAccess::GetComponent<FreelookCameraComponent>(*object);

        const glm::vec3 lookDirection = camera->GetLookDirection();
        std::optional<f32> rayIntersect = m_World->GetCollisionHandler()->DoRaycast(object->GetWorldPosition(), lookDirection);

        if (rayIntersect == std::nullopt)
        {
            return;
        }

        WorldEvent terrainEvent;

        if (_inputType == InputType::Interact)
        {
            terrainEvent.m_Type = WorldEvent::Type::AddTerrain;
            terrainEvent.m_Substance = m_Inventories[PLAYER_IDX].m_Slots[m_Inventories[PLAYER_IDX].m_SelectedIndex].m_Substance;
            m_Inventories[PLAYER_IDX].m_Slots[m_Inventories[PLAYER_IDX].m_SelectedIndex].m_Count--;
        }
        else if (_inputType == InputType::InteractAlternate)
        {
            terrainEvent.m_Type = WorldEvent::Type::RemoveTerrain;
        }

        constexpr f32 TERRAIN_INTERACTION_RADIUS = 2.f;

        terrainEvent.m_Radius = TERRAIN_INTERACTION_RADIUS;
        terrainEvent.m_Source = id;
        terrainEvent.m_TargetPosition = object->GetWorldPosition() + lookDirection * (*rayIntersect);

        m_World->AddWorldEvent(terrainEvent);
        return;
    }

    switch (_inputType)
    {
    case InputType::InventorySlot1:
    {
        ChangeSlot(PLAYER_IDX, 0);
        break;
    }
    case InputType::InventorySlot2:
    {
        ChangeSlot(PLAYER_IDX, 1);
        break;
    }
    case InputType::InventorySlot3:
    {
        ChangeSlot(PLAYER_IDX, 2);
        break;
    }
    case InputType::InventorySlot4:
    {
        ChangeSlot(PLAYER_IDX, 3);
        break;
    }
    default:
        break;
    }
}

void InventoryHandler::ChangeSlot(u32 _playerIndex, u32 _slotIndex)
{
    for (Inventory& inventory : m_Inventories)
    {
        if (inventory.m_PlayerID == _playerIndex)
        {
            if (m_Inventories[_playerIndex].m_Slots.size() > _slotIndex)
            {
                m_Inventories[_playerIndex].m_SelectedIndex = _slotIndex;
            }
            break;
        }
    }
}
