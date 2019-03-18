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

#include "SkillColumn.h"

#include "df/unit.h"
#include "df/unit_skill.h"
#include "df/unit_soul.h"

#include <QSettings>

#include "UnitModel.h"

#include "utils.h"

using namespace qtlabors;
using namespace df::enums;

using skill_traits = df::enum_traits<job_skill::job_skill>;

SkillColumn::SkillColumn(QSettings &settings, const QColor &color)
    : ViewColumn(settings.value("name").toString(), color)
    , skill(static_cast<skill_traits::enum_type>(settings.value("skill_id").toInt()-1))
{
}

SkillColumn::~SkillColumn()
{
}

QVariant SkillColumn::data(const Unit &unit, int role) const
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
    default:
        return ViewColumn::data(unit, role);
    }
}

