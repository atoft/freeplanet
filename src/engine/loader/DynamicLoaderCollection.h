//
// Created by alastair on 26/08/19.
//

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
