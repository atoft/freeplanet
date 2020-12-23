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

#include "BaseInspects.h"

void Inspect(std::string _name, u32& _target, InspectionContext& context)
{
    context.U32(_name, _target);
}

void Inspect(std::string _name, s32& _target, InspectionContext& context)
{
    context.S32(_name, _target);
}

void Inspect(std::string _name, f32& _target, InspectionContext& context)
{
    context.F32(_name, _target);
}

void Inspect(std::string _name, bool& _target, InspectionContext& context)
{
    context.Bool(_name, _target);
}

void Inspect(std::string _name, glm::vec2& _target, InspectionContext& context)
{
    context.Struct(_name, InspectionType::vec2, 0);

    Inspect("x", _target.x, context);
    Inspect("y", _target.y, context);

    context.EndStruct();
}

void Inspect(std::string _name, glm::vec3& _target, InspectionContext& context)
{
    context.Struct(_name, InspectionType::vec3, 0);

    Inspect("x", _target.x, context);
    Inspect("y", _target.y, context);
    Inspect("z", _target.z, context);

    context.EndStruct();
}

void Inspect(std::string _name, glm::ivec2& _target, InspectionContext& context)
{
    context.Struct(_name, InspectionType::ivec2, 0);

    Inspect("x", _target.x, context);
    Inspect("y", _target.y, context);

    context.EndStruct();
}

void Inspect(std::string _name, glm::ivec3& _target, InspectionContext& context)
{
    context.Struct(_name, InspectionType::ivec3, 0);

    Inspect("x", _target.x, context);
    Inspect("y", _target.y, context);
    Inspect("z", _target.z, context);

    context.EndStruct();
}
