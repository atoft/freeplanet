//
// Created by alastair on 22/05/2020.
//

#pragma once

#include <src/engine/inspection/InspectionContext.h>

template <typename ElementType>
void Inspect(std::string _name, std::vector<ElementType>& _element, InspectionContext& _context)
{
    _context.Vector(_name, _element);
}
