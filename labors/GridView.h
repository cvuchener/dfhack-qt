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

#ifndef QT_LABORS_GRID_VIEW_H
#define QT_LABORS_GRID_VIEW_H

#include <QAbstractItemModel>

#include <memory>

#include "Unit.h"

class QSettings;

namespace qtlabors
{

class UnitModel;
class ViewColumn;

class GridView: public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GridView(QSettings &settings, UnitModel *model, QObject *parent = nullptr);
    ~GridView() override;

    // Model implementation
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void updateUnits();
    void disableData();

private:
    UnitModel *model;
    bool enabled;
    std::vector<Unit> units;
    std::vector<std::unique_ptr<ViewColumn>> columns;
};

}

#endif
