//
// Created by alastair on 14/10/18.
//

#pragma once

#include <src/tools/globals.h>

using MeshAssetID = u32;
enum MeshAsset
{
    MeshAsset_Invalid = 0,
    MeshAsset_UnitCube,
    MeshAsset_UnitQuad,
    MeshAsset_UnitUVSphere,
    MeshAsset_UnitCylinder,
    MeshAsset_Arrow,

    MeshAsset_Tree,

    MeshAsset_Count
};

namespace Assets
{
    std::string GetMeshAssetName(MeshAssetID _assetID);
}
