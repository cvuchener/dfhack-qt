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

namespace df { class unit; }

namespace qtlabors
{

struct Unit
{
    int id;
    QString name;
    df::unit *ptr;

    inline bool operator<(const Unit &other) const { return id < other.id; }

    template<typename Iterator>
    static Iterator findById(Iterator first, Iterator last, int unit_id)
    {
        struct comp {
            inline bool operator() (const Unit &unit, int unit_id) const {
                return unit.id < unit_id;
            }
            inline bool operator() (int unit_id, const Unit &unit) const {
                return unit_id < unit.id;
            }
        };
        auto it = std::lower_bound(first, last, unit_id, comp{});
        if (it != last && it->id == unit_id)
            return it;
        else
            return last;
    }
};

}

#endif
