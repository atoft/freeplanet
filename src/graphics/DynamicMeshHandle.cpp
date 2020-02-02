//
// Created by alastair on 28/09/19.
//

#include "DynamicMeshHandle.h"

#include <src/engine/Engine.h>

void DynamicMeshHandle::RequestMeshUpdate(const RawMesh& _mesh)
{
    m_PreviousDynamicMeshId = m_DynamicMeshId;
    m_DynamicMeshId = DYNAMICMESHID_INVALID;
    m_RawMesh = _mesh;
}

void DynamicMeshHandle::RequestDestruction()
{
    Engine::GetInstance().AddEvent(EngineEvent(EngineEvent::Type::RenderOnDynamicMeshDestroyed, static_cast<u64>(m_DynamicMeshId)));
    Engine::GetInstance().AddEvent(EngineEvent(EngineEvent::Type::RenderOnDynamicMeshDestroyed, static_cast<u64>(m_PreviousDynamicMeshId)));
}
