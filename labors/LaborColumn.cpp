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

#include "UnitModel.h"

#include "utils.h"

using namespace qtlabors;
using namespace df::enums;

using labor_traits = df::enum_traits<unit_labor::unit_labor>;
using skill_traits = df::enum_traits<job_skill::job_skill>;

LaborColumn::LaborColumn(QSettings &settings, UnitModel *model, const QColor &color)
    : ViewColumn(settings.value("name").toString(), color)
    , labor(static_cast<labor_traits::enum_type>(settings.value("labor_id").toInt()))
    , skill(job_skill::NONE)
    , model(model)
{
    for (skill_traits::base_type i = 0; i <= skill_traits::last_item_value; ++i) {
        auto s = static_cast<job_skill::job_skill>(i);
        if (skill_traits::attrs(s).labor == labor) {
            skill = s;
            break;
        }
    }

    connect(model, &UnitModel::laborChanged,
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
    switch (role) {
    case Qt::DisplayRole:
        if (skill != job_skill::NONE) {
            if (auto soul = unit.ptr->status.current_soul) {
                auto s = binsearch_in_vector(soul->skills, &df::unit_skill::id, skill);
                return QString("%1").arg(s ? s->rating : 0);
            }
        }
        return QVariant();
    case Qt::CheckStateRole:
        return unit.ptr->status.labors[labor] ? Qt::Checked : Qt::Unchecked;
    default:
        return ViewColumn::data(unit, role);
    }
}

bool LaborColumn::setData(const Unit &unit, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && value.canConvert<Qt::CheckState>()) {
        model->setLabor(unit.id, labor, value.value<Qt::CheckState>() == Qt::Checked);
        return true;
    }
    else
        return false;
}

Qt::ItemFlags LaborColumn::flags(const Unit &unit) const
{
    return ViewColumn::flags(unit) | Qt::ItemIsUserCheckable;
}

