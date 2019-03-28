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

#include <QObject>

#include "Unit.h"

namespace qtlabors
{

class UnitModel: public QObject
{
    Q_OBJECT
public:
    explicit UnitModel(QObject *parent = nullptr);
    ~UnitModel() override;

    inline const std::vector<Unit> &units() const { return current_units; }; // sorted by id
    std::vector<Unit>::const_iterator findUnit(int id) const; // end if not found

    enum class Mode {
        None,
        Embark,
        Fortress,
    };
    inline Mode getMode() const { return mode; }

public slots:
    void refresh();

signals:
    void unitAboutToBeInserted(int index);
    void unitInserted(int index);
    void unitAboutToBeRemoved(int index);
    void unitRemoved(int index);
    void unitRefreshed(int index);

private:
    std::vector<Unit> current_units;
    Mode mode;
};

}

#endif
