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

    AssetID GetID() const
    {
        return m_Asset;
    }

    bool operator==(const AssetHandle<T>& _other) const
    {
        return GetID() == _other.GetID();
    }
};

template<typename T>
AssetBank<T> AssetHandle<T>::m_AssetBank = AssetBank<T>();
