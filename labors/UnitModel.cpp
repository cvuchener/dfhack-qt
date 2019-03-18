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
#include <modules/Translation.h>
#include <modules/Units.h>
#include <modules/World.h>

#include <df/unit.h>
#include <df/viewscreen_setupdwarfgamest.h>
#include <df/world.h>

#include <Core.h>

#include "UnitModel.h"

#include "utils.h"

using namespace qtlabors;
using namespace DFHack;

UnitModel::UnitModel(QObject *parent)
    : QObject(parent)
    , suspender(new CoreSuspender(std::defer_lock))
{
}

UnitModel::~UnitModel()
{
}

const std::vector<Unit> &UnitModel::units() const
{
    return current_units;
}


void UnitModel::beginDataAccess()
{
    suspender->lock();

    current_units.clear();
    if (auto embark_viewscreen = Gui::getViewscreenByType<df::viewscreen_setupdwarfgamest>()) {
        mode = Mode::Embark;
        for (auto unit: embark_viewscreen->units) {
            current_units.push_back(Unit {
                unit->id,
                DF2QT(Translation::TranslateName(Units::getVisibleName(unit), false)),
                unit
            });
        }
    }
    else if (World::isFortressMode()) {
        mode = Mode::Fortress;
        for (auto unit: df::global::world->units.active) {
            if (!Units::isCitizen(unit))
                continue;
            current_units.push_back(Unit {
                unit->id,
                DF2QT(Translation::TranslateName(Units::getVisibleName(unit), false)),
                unit
            });
        }
    }
    else
        mode = Mode::None;
    std::sort(current_units.begin(), current_units.end());

    emit unitListUpdated();
}

void UnitModel::endDataAccess()
{
    emit dataAccessDisabled();
    suspender->unlock();
}

void UnitModel::clearUnitList()
{
    current_units.clear();
    emit unitListUpdated();
}

void UnitModel::setLabor(int unit_id, df::enums::unit_labor::unit_labor labor, bool active)
{
    if (!suspender->owns_lock()) {
        qCritical("Core not suspended.");
        return;
    }
    auto it = Unit::findById(current_units.begin(), current_units.end(), unit_id);
    if (it == current_units.end()) {
        qCritical("Unit ID %d not found.", unit_id);
        return;
    }
    it->ptr->status.labors[labor] = active;
    emit laborChanged(unit_id, labor, active);
}
