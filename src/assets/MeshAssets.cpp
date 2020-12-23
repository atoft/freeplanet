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
        case MeshAsset_Branch_01:
            return "branches/Branch_01";
        case MeshAsset_Branch_Narrow_01:
            return "branches/Branch_Narrow_01";
        case MeshAsset_Campfire:
            return "Campfire";
        default:
            return "INVALID";
    }
    static_assert(MeshAsset_Count == 9);
}
