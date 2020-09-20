//
// Created by alastair on 20/09/18.
//

#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <mutex>

#include <src/tools/globals.h>
#include <src/tools/STL.h>
#include <iostream>

using AssetID = u32;
constexpr AssetID ASSETID_INVALID = 0;

struct AssetValue
{
    u32 m_Index;
    u32 m_RefCount;
};

template <typename T>
class AssetBank
{
    template<typename HandleType>
    friend class AssetHandle;

    template<typename HandleType>
    friend class AssetLoader;

private:
    static constexpr u32 INVALID_INDEX = std::numeric_limits<u32>::max();

    // For now we enforce the max capacity to avoid dynamic reallocations.
    // In future we should use allocate a fixed size array and add extra
    // arrays if we reach capacity
    static constexpr u32 ASSETBANK_CAPACITY = 32;

    // Could split this into access to map and access to data if needed
    mutable std::mutex m_AccessMutex;

    std::vector<AssetID> m_AssetsToCreate;
    std::vector<AssetID> m_AssetsToDestroy;

    std::map<AssetID, AssetValue> m_Map;
    std::vector<T> m_Data;

public:
    AssetBank()
    {
        m_Data.reserve(ASSETBANK_CAPACITY);
    }

private:
    void OnHandleCreated(AssetID _asset)
    {
        if(_asset == ASSETID_INVALID)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_AccessMutex);

        auto&& result = m_Map.find(_asset);
        if(result != m_Map.end())
        {
            result->second.m_RefCount++;

            // Cancel any pending requests to destroy this asset.
            // TODO Need to evaluate whether it's more performant just to generate the destroy list during HandleLoadRequests.
            STL::Remove(m_AssetsToDestroy, _asset);
        }
        else
        {
            assert(m_Data.size() < ASSETBANK_CAPACITY);
            assert(std::find(m_AssetsToDestroy.begin(), m_AssetsToDestroy.end(), _asset) == m_AssetsToDestroy.end());

            m_AssetsToCreate.push_back(_asset);
            m_Map[_asset].m_RefCount = 1;
            m_Map[_asset].m_Index = INVALID_INDEX;
        }
    }

    void OnHandleDestroyed(AssetID _asset)
    {
        if(_asset == ASSETID_INVALID)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_AccessMutex);

        auto&& result = m_Map.find(_asset);
        assert(result != m_Map.end());

        u32& refCount = result->second.m_RefCount;
        assert(refCount > 0);
        refCount--;

        if(refCount == 0)
        {
            m_AssetsToDestroy.push_back(_asset);

            // Cancel any pending requests to create this asset.
            STL::Remove(m_AssetsToCreate, _asset);
        }
    }

    T* GetAsset(AssetID _asset)
    {
        if(_asset == ASSETID_INVALID)
        {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_AccessMutex);

        auto&& result = m_Map.find(_asset);
        assert(result != m_Map.end());

        if(result->second.m_Index == INVALID_INDEX)
        {
            return nullptr;
        }
        assert(result->second.m_RefCount > 0);
        assert(result->second.m_Index < m_Data.size());

        return &(m_Data[result->second.m_Index]);
    }

    const T* GetAsset(AssetID _asset) const
    {
        if(_asset == 0)
        {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_AccessMutex);

        auto&& result = m_Map.find(_asset);
        assert(result != m_Map.end());

        if(result->second.m_Index == INVALID_INDEX)
        {
            return nullptr;
        }
        assert(result->second.m_RefCount > 0);
        assert(result->second.m_Index < m_Data.size());

        return &(m_Data[result->second.m_Index]);
    }

    void HandleLoadRequests()
    {
        std::lock_guard<std::mutex> lock(m_AccessMutex);
        for(AssetID id : m_AssetsToDestroy)
        {
            auto&& result = m_Map.find(id);
            assert(result != m_Map.end());

            assert(result->second.m_RefCount == 0);

            u32 previousIndex = result->second.m_Index;
            m_Map.erase(id);

            if(previousIndex == m_Data.size() -1)
            {
                m_Data.back().ReleaseResources();
                m_Data.pop_back();
            }
            else
            {
                // Note that we use an explicit ReleaseResources instead of using the destructor
                // to avoid destructor weirdness, such as getting a destructor call from swapping
                // the elements in this case.

                m_Data[previousIndex].ReleaseResources();
                m_Data[previousIndex] = m_Data[m_Data.size() -1];
                m_Data.pop_back();

                for(auto& assetValue : m_Map)
                {
                    if(assetValue.second.m_Index == m_Data.size())
                    {
                        assetValue.second.m_Index = previousIndex;
                        break;
                    }
                }
            }
        }
        for(AssetID id : m_AssetsToCreate)
        {
            LogMessage("Creating " + std::string(typeid(T).name()) + " AssetID" + std::to_string(id));

            assert(m_Data.size() < ASSETBANK_CAPACITY);
            m_Data.emplace_back();
            m_Data.back().AcquireResources(id);

            m_Map[id].m_Index = m_Data.size() - 1;
        }

        m_AssetsToDestroy.clear();
        m_AssetsToCreate.clear();
    }

    // Attempt to fix a crash on quit caused by non-trivial types inside the asset.
    // They seem to get destructor calls on the main thread even after everything should have
    // been freed on the Render thread.
    void Shutdown()
    {
        for (T& asset : m_Data)
        {
            asset.ReleaseResources();
        }

        m_Data.clear();
        m_Data.shrink_to_fit();
        m_AssetsToDestroy.clear();
        m_AssetsToCreate.clear();
        m_Map.clear();
    }
};
