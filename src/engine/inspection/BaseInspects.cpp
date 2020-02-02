//
// Created by alastair on 20/01/2020.
//

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
