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

#ifndef QT_LABORS_LABOR_COLUMN_H
#define QT_LABORS_LABOR_COLUMN_H

#include "DataDefs.h"
#include "df/unit_labor.h"
#include "df/job_skill.h"

#include "ViewColumn.h"

#include <QStandardItem>

class QSettings;

namespace qtlabors
{

class UnitModel;

class LaborColumn: public ViewColumn
{
public:
    LaborColumn(QSettings &settings, UnitModel *model, const QColor &color);
    ~LaborColumn() override;

    QVariant data(const Unit &unit, int role = Qt::DisplayRole) const override;
    bool setData(const Unit &unit, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const Unit &unit) const override;

private:
    df::enums::unit_labor::unit_labor labor;
    df::enums::job_skill::job_skill skill;
    UnitModel *model;
};

}

#endif
