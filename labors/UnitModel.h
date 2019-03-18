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

#ifndef QT_LABORS_UNIT_MODEL_H
#define QT_LABORS_UNIT_MODEL_H

#include "DataDefs.h"
#include "df/unit_labor.h"

#include <QObject>

#include "Unit.h"

namespace DFHack { class CoreSuspender; }

namespace qtlabors
{

class UnitModel: public QObject
{
    Q_OBJECT
public:
    explicit UnitModel(QObject *parent = nullptr);
    ~UnitModel() override;

    const std::vector<Unit> &units() const; // sorted by id

    enum class Mode {
        None,
        Embark,
        Fortress,
    };

    inline Mode getMode() const { return mode; }

public slots:
    void beginDataAccess();
    void endDataAccess();
    void clearUnitList();

    void setLabor(int unit_id, df::enums::unit_labor::unit_labor labor, bool active);

signals:
    void unitListUpdated();
    void dataAccessDisabled();

    void laborChanged(int unit_id, df::enums::unit_labor::unit_labor labor, bool active);

private:
    std::unique_ptr<DFHack::CoreSuspender> suspender;
    std::vector<Unit> current_units;
    Mode mode;
};

}

#endif
