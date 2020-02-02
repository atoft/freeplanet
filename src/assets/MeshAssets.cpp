//
// Created by alastair on 14/10/18.
//

#include "MeshAssets.h"

std::string Assets::GetMeshAssetName(MeshAssetID _assetID)
{
    assert(_assetID < MeshAsset_Count);

    switch(_assetID)
    {
        case MeshAsset_UnitCube:
            return "Cube";
        case MeshAsset_UnitQuad:
            return "Quad";
        case MeshAsset_UnitUVSphere:
            return "UnitUVSphere";
        case MeshAsset_UnitCylinder:
            return "UnitCylinder";
		case MeshAsset_Arrow:
			return "Arrow";
        default:
            return "INVALID";
    }
    static_assert(MeshAsset_Count == 6);
}
