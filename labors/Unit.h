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

#ifndef QT_LABORS_UNIT_H
#define QT_LABORS_UNIT_H

#include <DataDefs.h>
#include <df/unit_skill.h>

#include <QString>

#include <array>

namespace df { struct unit; }

namespace qtlabors
{

struct Unit
{
    int id;
    QString name;
    std::vector<df::unit_skill> skills;
    std::array<bool, 94> labors;

    Unit(df::unit *);

    void refresh(df::unit *);

    // nullptr if not found
    const df::unit_skill *findSkill(df::enums::job_skill::job_skill skill) const;

    inline bool operator<(const Unit &other) const { return id < other.id; }

    struct comp {
        inline bool operator() (const Unit &unit, int unit_id) const {
            return unit.id < unit_id;
        }
        inline bool operator() (int unit_id, const Unit &unit) const {
            return unit_id < unit.id;
        }
    };

    template<typename Iterator>
    static Iterator findById(Iterator first, Iterator last, int unit_id)
    {
        auto it = std::lower_bound(first, last, unit_id, comp{});
        if (it != last && it->id == unit_id)
            return it;
        else
            return last;
    }
};

}

#endif
