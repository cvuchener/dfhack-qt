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

#include "ViewColumn.h"

#include <QStandardItem>

using namespace qtlabors;

ViewColumn::ViewColumn(const QString &title, const QColor &color, QObject *parent)
    : QObject(parent)
    , title(title)
    , color(color)
{
}

ViewColumn::~ViewColumn()
{
}

QVariant ViewColumn::headerData(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return title;
    case Qt::BackgroundRole:
        return color;
    default:
        return QVariant();
    }
}

QVariant ViewColumn::data(const Unit &, int role) const
{
    switch (role) {
    case Qt::BackgroundColorRole:
        return color;
    default:
        return QVariant();
    }
}

bool ViewColumn::setData(const Unit &, const QVariant &, int)
{
    return false;
}

Qt::ItemFlags ViewColumn::flags(const Unit &) const
{
    return Qt::ItemIsEnabled;
}

