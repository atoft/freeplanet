//
// Created by alastair on 14/10/18.
//


#pragma once

#include <src/engine/AssetHandle.h>

template<typename T>
class AssetLoader
{
public:
    void HandleLoadRequests()
    {
        AssetHandle<T>::GetAssetBank().HandleLoadRequests();
    }

    void Shutdown()
    {
        AssetHandle<T>::GetAssetBank().Shutdown();
    }
};
