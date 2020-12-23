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

#include <atomic>
#include <optional>
#include <thread>

#include <src/tools/globals.h>

template <typename Content, typename Identifier, typename... Args>
class DynamicLoader
{
public:
    enum class State
    {
        Empty,
        Loading,
        Loaded
    };


public:
    DynamicLoader() = default;

private:
    std::atomic<State> m_State = State::Empty;
    std::optional<Content> m_Content = std::nullopt;
    std::thread m_Thread;
    Identifier m_Identifier;

public:
    void Load(Identifier _identifier, Args... args)
    {
        assert(ThreadUtils::tl_ThreadType == ThreadType::Main);
        assert(m_State == State::Empty);

        m_State = State::Loading;
        m_Identifier = _identifier;

        if(m_Thread.joinable())
        {
            m_Thread.join();
        }

        m_Thread = std::thread([=]()
                               {
                                   ThreadUtils::tl_ThreadType = ThreadType::Loader;
                                   m_Content = Content(args...);
                                   m_State = State::Loaded;
                               });
    }

    ~DynamicLoader()
    {
        if(m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    bool IsLoaded() const
    {
        return m_State == State::Loaded;
    }

    bool IsEmpty() const
    {
        return m_State == State::Empty;
    }

    bool IsLoading() const
    {
        return m_State == State::Loading;
    }

    Content& Get()
    {
        assert(IsLoaded());
        return *m_Content;
    }

    const Content& Get() const
    {
        assert(IsLoaded());
        return *m_Content;
    }

    Identifier GetIdentifier() const
    {
        return m_Identifier;
    }

    void Clear()
    {
        assert(m_State != State::Loading);
        m_State = State::Empty;
        m_Content = std::nullopt;
        m_Identifier = Identifier();
    }
};
