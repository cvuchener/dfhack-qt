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

#include "DataDefs.h"
#include <df/unit.h>

#include <Core.h>

#include "ChangesModel.h"
#include "UnitModel.h"

#include <QtDebug>

using namespace qtlabors;

struct ChangesModel::unit_changes_comp
{
    bool operator()(const unit_changes_t &lhs, const unit_changes_t &rhs) const {
        return lhs.unit_id < rhs.unit_id;
    }
    bool operator()(const unit_changes_t &changes, int unit_id) const {
        return changes.unit_id < unit_id;
    }
};

struct labor_comp
{
    using unit_labor = df::enums::unit_labor::unit_labor;
    bool operator()(const std::pair<unit_labor, int> &p, unit_labor labor) {
        return p.first < labor;
    }
};

ChangesModel::ChangesModel(const UnitModel *units, QObject *parent)
    : QAbstractItemModel(parent)
    , units(units)
{
    connect(units, &UnitModel::unitRefreshed, this, &ChangesModel::refreshUnit);
}

ChangesModel::~ChangesModel()
{
}

bool ChangesModel::empty() const
{
    return std::all_of(unit_changes.begin(), unit_changes.end(),
                       [](const unit_changes_t &unit){
                           return unit.labors.empty();
                       });
}

std::pair<bool, bool> ChangesModel::unitLabor(int unit_id, unit_labor labor) const
{
    auto unit_it = std::lower_bound(unit_changes.begin(), unit_changes.end(),
                                    unit_id, unit_changes_comp{});
    if (unit_it == unit_changes.end() || unit_it->unit_id != unit_id) // unit not changed
        return {false, false};
    auto labor_it = std::lower_bound(unit_it->labors.begin(), unit_it->labors.end(),
                                     labor, labor_comp{});
    if (labor_it == unit_it->labors.end() || labor_it->first != labor) // labor not changed
        return {false, false};
    else
        return {true, labor_it->second};
}

static constexpr quintptr NoParent = -1;

QModelIndex ChangesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return createIndex(row, column, parent.row());
    else
        return createIndex(row, column, NoParent);
}

QModelIndex ChangesModel::parent(const QModelIndex &index) const
{
    if (index.internalId() == NoParent)
        return QModelIndex();
    else
        return createIndex(index.internalId(), 0, NoParent);
}

int ChangesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return unit_changes.size();
    else if (parent.internalId() == NoParent)
        return unit_changes[parent.row()].labors.size();
    else
        return 0;
}

int ChangesModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ChangesModel::data(const QModelIndex &index, int role) const
{
    if (index.internalId() == NoParent) {
        // Unit item
        int id = unit_changes[index.row()].unit_id;
        auto unit = Unit::findById(units->units().begin(), units->units().end(), id);
        switch (role) {
        case Qt::DisplayRole:
            if (unit != units->units().end())
                return unit->name;
            else
                return tr("Unknown unit %1").arg(id);
        default:
            return QVariant();
        }
    }
    else {
        // Unit changes item
        const auto &labor = unit_changes[index.internalId()].labors[index.row()];
        auto labor_caption = df::enum_traits<unit_labor>::attrs(labor.first).caption;
        switch (role) {
        case Qt::DisplayRole:
            return labor.second
                ? QString("Activate %1").arg(labor_caption)
                : QString("Deactivate %1").arg(labor_caption);
        default:
            return QVariant();
        }
    }
}

bool ChangesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // TODO: emit unitLaborChanged
    if (parent.isValid()) {
        auto &changes = unit_changes[parent.row()];
        if (row+count > changes.labors.size())
                return false;
        beginRemoveRows(parent, row, row+count);
        changes.labors.erase(changes.labors.begin()+row,
                             changes.labors.begin()+row+count);
        endRemoveRows();
        cleanupUnitChanges(unit_changes.begin() + parent.row());
        return true;
    }
    else {
        if (row+count > unit_changes.size())
                return false;
        beginRemoveRows(parent, row, row+count);
        unit_changes.erase(unit_changes.begin()+row,
                           unit_changes.begin()+row+count);
        endRemoveRows();
        return true;
    }
}

void ChangesModel::apply()
{
    DFHack::CoreSuspender lock;
    beginResetModel();
    for (const auto &u: unit_changes) {
        auto unit = df::unit::find(u.unit_id);
        if (!unit) {
            qCritical() << "Unit not found" << u.unit_id;
            continue;
        }
        for (const auto &labor: u.labors)
            unit->status.labors[labor.first] = labor.second;
    }
    unit_changes.clear();
    endResetModel();
}

void ChangesModel::reset()
{
    beginResetModel();
    unit_changes.clear();
    endResetModel();
}

void ChangesModel::refreshUnit(int index)
{
    auto unit_id = units->units()[index].id;
    auto it = std::lower_bound(unit_changes.begin(), unit_changes.end(),
                               unit_id, unit_changes_comp{});
    if (it != unit_changes.end() && it->unit_id == unit_id) {
        auto i = ChangesModel::index(std::distance(unit_changes.begin(), it), 0);
        emit dataChanged(i, i);
    }
}

ChangesModel::unit_changes_t::unit_changes_t(int unit_id)
    : unit_id(unit_id)
{
}

void ChangesModel::changeUnitLabor(int unit_id, unit_labor labor, bool activated)
{
    auto unit_it = findOrCreateUnitChanges(unit_id);
    auto labor_it = std::lower_bound(unit_it->labors.begin(), unit_it->labors.end(),
                                     labor, labor_comp{});
    int row = std::distance(unit_it->labors.begin(), labor_it);
    if (labor_it == unit_it->labors.end() || labor_it->first != labor) {
        beginInsertRows(unitModelIndex(unit_it), row, row);
        labor_it = unit_it->labors.emplace(labor_it, labor, activated);
        endInsertRows();
    }
    else {
        labor_it->second = activated;
        auto i = index(row, 0, unitModelIndex(unit_it));
        emit dataChanged(i, i);
    }
    emit unitLaborChanged(unit_id, labor);
}

void ChangesModel::resetUnitLabor(int unit_id, unit_labor labor)
{
    auto unit_it = std::lower_bound(unit_changes.begin(), unit_changes.end(),
                                    unit_id, unit_changes_comp{});
    if (unit_it == unit_changes.end() || unit_it->unit_id != unit_id) // unit not changed
        return;
    auto labor_it = std::lower_bound(unit_it->labors.begin(), unit_it->labors.end(),
                                     std::make_pair(labor, false));
    if (labor_it == unit_it->labors.end() || labor_it->first != labor) // labor not changed
        return;
    int row = std::distance(unit_it->labors.begin(), labor_it);
    beginRemoveRows(unitModelIndex(unit_it), row, row);
    labor_it = unit_it->labors.erase(labor_it);
    endRemoveRows();
    emit unitLaborChanged(unit_id, labor);
    cleanupUnitChanges(unit_it);
}


QModelIndex ChangesModel::unitModelIndex(const_unit_iterator it) const
{
    int row = std::distance(unit_changes.begin(), it);
    return index(row, 0);
}

ChangesModel::unit_iterator ChangesModel::findOrCreateUnitChanges(int unit_id)
{
    auto unit_it = std::lower_bound(unit_changes.begin(), unit_changes.end(),
                                    unit_id, unit_changes_comp{});
    if (unit_it == unit_changes.end() || unit_it->unit_id != unit_id) {
        auto row = std::distance(unit_changes.begin(), unit_it);
        beginInsertRows(QModelIndex(), row, row);
        unit_it = unit_changes.emplace(unit_it, unit_id);
        endInsertRows();
    }
    return unit_it;
}

void ChangesModel::cleanupUnitChanges(unit_iterator it)
{
    // Remove units without changes
    if (it->labors.empty()) {
        int row = std::distance(unit_changes.begin(), it);
        beginRemoveRows(QModelIndex(), row, row);
        unit_changes.erase(it);
        endRemoveRows();
    }
}
