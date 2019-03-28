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

#include <QSettings>

#include "GridView.h"

#include "LaborColumn.h"
#include "SkillColumn.h"
#include "UnitModel.h"

#include <QtDebug>

using namespace qtlabors;

GridView::GridView(QSettings &settings, const UnitModel *m, ChangesModel *c, QObject *parent)
    : QAbstractItemModel(parent)
    , model(m)
    , changes(c)
{
    int set_count = settings.beginReadArray("sets");
    for (int i = 0; i < set_count; ++i) {
        settings.setArrayIndex(i);
        auto color = settings.value("bg_color").value<QColor>();
        int column_count = settings.beginReadArray("columns");
        for (int j = 0; j < column_count; ++j) {
            settings.setArrayIndex(j);
            auto type = settings.value("type").toString();
            ViewColumn *col = nullptr;
            if (type == "LABOR") {
                col = new LaborColumn(settings, changes, color);
            }
            else if (type == "SKILL") {
                col = new SkillColumn(settings, color);
            }
            else {
                qDebug() << "Skip unknown column type" << type;
            }
            if (col) {
                columns.emplace_back(col);
                int col_index = columns.size();
                connect(col, &ViewColumn::dataChanged,
                        [this, col_index] (int unit_id) {
                            auto it = model->findUnit(unit_id);
                            if (it != model->units().end()) {
                                auto index = createIndex(std::distance(model->units().begin(), it), col_index);
                                emit dataChanged(index, index);
                            }
                        });
            }
        }
        settings.endArray();
    }
    settings.endArray();

    connect(model, &UnitModel::unitAboutToBeInserted,
            [this](int row){ beginInsertRows({}, row, row); });
    connect(model, &UnitModel::unitInserted,
            [this](int row){ endInsertRows(); });
    connect(model, &UnitModel::unitAboutToBeRemoved,
            [this](int row){ beginRemoveRows({}, row, row); });
    connect(model, &UnitModel::unitRemoved,
            [this](int row){ endRemoveRows(); });
    connect(model, &UnitModel::unitRefreshed,
            [this](int row){ emit dataChanged(index(row, 0), index(row, columns.size())); });
}

GridView::~GridView()
{
}

QModelIndex GridView::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex GridView::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int GridView::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return model->units().size();
}

int GridView::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return columns.size() + 1;
}

QVariant GridView::data(const QModelIndex &index, int role) const
{
    const auto &unit = model->units()[index.row()];
    if (index.column() == 0) {
        switch (role) {
        case Qt::DisplayRole:
            return unit.name;
        default:
            return QVariant();
        }
    }
    else {
        return columns[index.column()-1]->data(unit, role);
    }
    return QVariant();
}

bool GridView::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto &unit = model->units()[index.row()];
    if (index.column() == 0) {
        return false;
    }
    else {
        return columns[index.column()-1]->setData(unit, value, role);
    }
    return false;
}

Qt::ItemFlags GridView::flags(const QModelIndex &index) const
{
    const auto &unit = model->units()[index.row()];
    if (index.column() == 0) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    }
    else {
        return columns[index.column()-1]->flags(unit);
    }
    return Qt::NoItemFlags;
}

QVariant GridView::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    if (section == 0)
        return QVariant();
    else
        return columns[section-1]->headerData(role);
}

