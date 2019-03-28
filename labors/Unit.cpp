/*
 * Copyright 2019 Clement Vuchener
 *
 * This file is part of qtlabors plugin for DFHack
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <modules/Translation.h>
#include <modules/Units.h>

#include <df/unit_soul.h>

#include "Unit.h"

#include "utils.h"

using namespace qtlabors;
using namespace DFHack;

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

Unit::Unit(df::unit *unit)
    : id(unit->id)
{
    refresh(unit);
}

void Unit::refresh(df::unit *unit)
{
    assert(unit->id == id);

    name = DF2QT(Translation::TranslateName(Units::getVisibleName(unit), false));
    skills.clear();
    if (auto soul = unit->status.current_soul) {
        skills.reserve(soul->skills.size());
        for (auto skill: soul->skills)
            skills.push_back(*skill);
    }
    static_assert(ARRAY_SIZE(unit->status.labors) == std::tuple_size<decltype(labors)>::value,
                  "Invalid labor array size");
    std::copy_n(unit->status.labors, labors.size(), labors.begin());
}

const df::unit_skill *Unit::findSkill(df::enums::job_skill::job_skill skill) const
{
    using df::enums::job_skill::job_skill;
    auto it = std::lower_bound(skills.begin(), skills.end(), skill,
            [](const df::unit_skill &us, job_skill s) {
                return us.id < s;
            });
    if (it != skills.end() && it->id == skill)
        return &(*it);
    else
        return nullptr;
}
