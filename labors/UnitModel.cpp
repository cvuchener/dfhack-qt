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

#include <modules/Gui.h>
#include <modules/Units.h>
#include <modules/World.h>

#include <df/unit.h>
#include <df/viewscreen_setupdwarfgamest.h>
#include <df/world.h>

#include <Core.h>

#include "UnitModel.h"

#include <QtDebug>

using namespace qtlabors;
using namespace DFHack;

UnitModel::UnitModel(QObject *parent)
    : QObject(parent)
{
}

UnitModel::~UnitModel()
{
}

std::vector<Unit>::const_iterator UnitModel::findUnit(int id) const
{
    auto it = std::lower_bound(current_units.begin(), current_units.end(), id, Unit::comp{});
    if (it == current_units.end() || it->id != id)
        return current_units.end();
    else
        return it;
}

void UnitModel::refresh()
{
    CoreSuspender lock;

    // Retrieve new unit list
    std::vector<df::unit *> new_units;
    if (auto embark_viewscreen = Gui::getViewscreenByType<df::viewscreen_setupdwarfgamest>()) {
        mode = Mode::Embark;
        new_units = embark_viewscreen->units;
    }
    else if (World::isFortressMode()) {
        mode = Mode::Fortress;
        for (auto unit: df::global::world->units.active) {
            if (!Units::isCitizen(unit))
                continue;
            new_units.push_back(unit);
        }
    }
    else {
        mode = Mode::None;
    }
    std::sort(new_units.begin(), new_units.end(),
              [](const df::unit *lhs, const df::unit *rhs){return lhs->id < rhs->id;});

    // Merge unit vectors
    auto old_it = current_units.begin();
    for (const auto &new_unit: new_units) {
        while (old_it != current_units.end() && old_it->id < new_unit->id) {
            int index = std::distance(current_units.begin(), old_it);
            emit unitAboutToBeRemoved(index);
            qDebug() << "remove" << index << old_it->id;
            old_it = current_units.erase(old_it);
            emit unitRemoved(index);
        }
        if (old_it != current_units.end() && old_it->id == new_unit->id) {
            int index = std::distance(current_units.begin(), old_it);
            qDebug() << "refresh" << index << old_it->id;
            old_it->refresh(new_unit);
            emit unitRefreshed(index);
            ++old_it;
        }
        else {
            int index = std::distance(current_units.begin(), old_it);
            emit unitAboutToBeInserted(index);
            qDebug() << "insert" << index << new_unit->id;
            old_it = current_units.emplace(old_it, new_unit);
            ++old_it;
            emit unitInserted(index);
        }
    }
    while (old_it != current_units.end()) {
        int index = std::distance(current_units.begin(), old_it);
        emit unitAboutToBeRemoved(index);
        qDebug() << "remove" << index << old_it->id;
        old_it = current_units.erase(old_it);
        emit unitRemoved(index);
    }
}

