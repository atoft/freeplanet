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

#include <vector>

#include <src/engine/loader/DynamicLoader.h>
#include <src/tools/globals.h>

template<typename Content, typename Identifier, u32 Count, typename... Args>
class DynamicLoaderCollection
{
private:
    using Loader = DynamicLoader<Content, Identifier, Args...>;

    std::array<Loader, Count> m_Loaders;

public:
    bool RequestLoad(Identifier _identifier, Args... _args)
    {
        if (IsLoadingOrLoaded(_identifier))
        {
            return false;
        }

        for (Loader& loader : m_Loaders)
        {
            if (loader.IsEmpty())
            {
                loader.Load(_identifier, _args...);
                return true;
            }
        }

        return false;
    }

    bool IsLoadingOrLoaded(Identifier _identifier) const
    {
        for (const Loader& loader : m_Loaders)
        {
            if (!loader.IsEmpty() && loader.GetIdentifier() == _identifier)
            {
                return true;
            }
        }

        return false;
    }

    const Content* Get(Identifier _identifier) const
    {
        for (const Loader& loader : m_Loaders)
        {
            if (loader.GetIdentifier() == _identifier && loader.IsLoaded())
            {
                return &loader.Get();
            }
        }

        return nullptr;
    }

    void Clear(Identifier _identifier)
    {
        for (Loader& loader : m_Loaders)
        {
            if (loader.GetIdentifier() == _identifier)
            {
                loader.Clear();
            }
        }
    }

    void TransferLoadedContent(std::vector<Content>& _outContent)
    {
        for (Loader& loader : m_Loaders)
        {
            if (loader.IsLoaded())
            {
                _outContent.push_back(std::move(loader.Get()));

                loader.Clear();
            }
        }
    }

    std::vector<Identifier> GetLoadedIdentifiers() const
    {
        std::vector<Identifier> identifiers;

        for (const Loader& loader : m_Loaders)
        {
            if (loader.IsLoaded())
            {
                identifiers.push_back(loader.GetIdentifier());
            }
        }

        return identifiers;
    }

    std::vector<Identifier> GetLoadingOrLoadedIdentifiers() const
    {
        std::vector<Identifier> identifiers;

        for (const Loader& loader : m_Loaders)
        {
            if (loader.IsLoading() || loader.IsLoaded())
            {
                identifiers.push_back(loader.GetIdentifier());
            }
        }

        return identifiers;
    }

    bool IsAllLoaded() const
    {
        for (const Loader& loader : m_Loaders)
        {
            if (loader.IsLoading())
            {
                return false;
            }
        }

        return true;
    }

    bool IsEmpty() const
    {
        for (const Loader& loader : m_Loaders)
        {
            if (!loader.IsEmpty())
            {
                return false;
            }
        }

        return true;
    }
};
