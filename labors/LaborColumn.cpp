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

#include "LaborColumn.h"

#include "df/unit.h"
#include "df/unit_skill.h"
#include "df/unit_soul.h"

#include <QSettings>

#include "ChangesModel.h"
#include "Unit.h"

#include "utils.h"
#include "globals.h"

#include <QtDebug>

using namespace qtlabors;
using namespace df::enums;

using labor_traits = df::enum_traits<unit_labor::unit_labor>;
using skill_traits = df::enum_traits<job_skill::job_skill>;

LaborColumn::LaborColumn(QSettings &settings, ChangesModel *changes, const QColor &color)
    : ViewColumn(settings.value("name").toString(), color)
    , labor(static_cast<labor_traits::enum_type>(settings.value("labor_id").toInt()))
    , skill(job_skill::NONE)
    , changes(changes)
{
    for (skill_traits::base_type i = 0; i <= skill_traits::last_item_value; ++i) {
        auto s = static_cast<job_skill::job_skill>(i);
        if (skill_traits::attrs(s).labor == labor) {
            skill = s;
            break;
        }
    }

    connect(changes, &ChangesModel::unitLaborChanged,
            [this] (int unit_id, unit_labor::unit_labor labor) {
                if (labor == this->labor)
                    emit dataChanged(unit_id);
            });
}

LaborColumn::~LaborColumn()
{
}

QVariant LaborColumn::data(const Unit &unit, int role) const
{
    auto changed = changes->unitLabor(unit.id, labor);
    bool activated = changed.first ? changed.second : unit.labors[labor];
    switch (role) {
    case Qt::DisplayRole:
        if (skill != job_skill::NONE) {
            if (auto s = unit.findSkill(skill))
                return s->rating;
            else
                return 0;
        }
        return QVariant();
    case Qt::CheckStateRole:
        return activated ? Qt::Checked : Qt::Unchecked;
    case ChangedRole:
        return changed.first;
    default:
        return ViewColumn::data(unit, role);
    }
}

bool LaborColumn::setData(const Unit &unit, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && value.canConvert<Qt::CheckState>()) {
        auto activated = value.value<Qt::CheckState>() == Qt::Checked;
        if (activated != unit.labors[labor])
            changes->changeUnitLabor(unit.id, labor, activated);
        else
            changes->resetUnitLabor(unit.id, labor);
        return true;
    }
    else
        return false;
}

Qt::ItemFlags LaborColumn::flags(const Unit &unit) const
{
    return ViewColumn::flags(unit) | Qt::ItemIsUserCheckable;
}

