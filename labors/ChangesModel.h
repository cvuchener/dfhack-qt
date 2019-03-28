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

#ifndef QT_LABORS_CHANGES_MODEL_H
#define QT_LABORS_CHANGES_MODEL_H

#include "DataDefs.h"
#include "df/unit_labor.h"

#include <QAbstractItemModel>

namespace qtlabors
{

class UnitModel;

class ChangesModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    using unit_labor = df::enums::unit_labor::unit_labor;

    explicit ChangesModel(const UnitModel *units, QObject *parent = nullptr);
    ~ChangesModel() override;

    bool empty() const;

    // returns pair <changed, activated>
    std::pair<bool, bool> unitLabor(int unit_id, unit_labor labor) const;

    // Model implementation
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = {}) override;

public slots:
    void changeUnitLabor(int unit_id, unit_labor labor, bool activated);
    void resetUnitLabor(int unit_id, unit_labor labor);

    void apply();
    void reset();

signals:
    void unitLaborChanged(int unit_id, unit_labor labor);

private slots:
    void refreshUnit(int index);

private:
    const UnitModel *units;
    struct unit_changes_t {
        int unit_id;
        std::vector<std::pair<unit_labor, bool>> labors;

        explicit unit_changes_t(int unit_id);
    };
    struct unit_changes_comp;
    std::vector<unit_changes_t> unit_changes;
    using unit_iterator = decltype(unit_changes)::iterator;
    using const_unit_iterator = decltype(unit_changes)::const_iterator;

    QModelIndex unitModelIndex(const_unit_iterator) const;
    unit_iterator findOrCreateUnitChanges(int unit_id);
    void cleanupUnitChanges(unit_iterator);
};

}

#endif
