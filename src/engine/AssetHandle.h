//
// Created by alastair on 23/09/18.
//

#pragma once

#include "AssetBank.h"

template<typename T>
class AssetHandle
{
private:
    static AssetBank<T> m_AssetBank;
    AssetID m_Asset;

public:
    AssetHandle(AssetID _asset)
    {
        m_Asset = _asset;
        m_AssetBank.OnHandleCreated(_asset);
    }

    AssetHandle()
    {
        m_Asset = ASSETID_INVALID;
        m_AssetBank.OnHandleCreated(ASSETID_INVALID);
    }

    AssetHandle& operator=(const AssetHandle& _other)
    {
        m_AssetBank.OnHandleDestroyed(this->m_Asset);
        m_Asset = _other.m_Asset;
        m_AssetBank.OnHandleCreated(m_Asset);
        return *this;
    }

    AssetHandle(const AssetHandle& _other)
    {
        m_Asset = _other.m_Asset;
        m_AssetBank.OnHandleCreated(m_Asset);
    }

    AssetHandle(AssetHandle&& _other)
    {
        m_Asset = _other.m_Asset;
        m_AssetBank.OnHandleCreated(m_Asset);
    }

    ~AssetHandle()
    {
        m_AssetBank.OnHandleDestroyed(m_Asset);
    }

    T* GetAsset() const
    {
        return m_AssetBank.GetAsset(m_Asset);
    }

    static AssetBank<T>& GetAssetBank()
    {
        return m_AssetBank;
    }
};

template<typename T>
AssetBank<T> AssetHandle<T>::m_AssetBank = AssetBank<T>();
