/*
 * Copyright 2019 Clement Vuchener
 *
 * This file is part of qapplication plugin for DFHack
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

#include "LogFilterProxyModel.h"

#include "Log.h"

using namespace qapplication;

LogFilterProxyModel::LogFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , accepted_types{false, true, true, true, true}
{
    using accepted_types_size = std::tuple_size<decltype(accepted_types)>;
    static_assert(QtFatalMsg < accepted_types_size::value, "invalid array size");
    static_assert(QtCriticalMsg < accepted_types_size::value, "invalid array size");
    static_assert(QtWarningMsg < accepted_types_size::value, "invalid array size");
    static_assert(QtInfoMsg < accepted_types_size::value, "invalid array size");
    static_assert(QtDebugMsg < accepted_types_size::value, "invalid array size");
}

LogFilterProxyModel::~LogFilterProxyModel()
{
}


bool LogFilterProxyModel::acceptFatal() const
{
    return accepted_types[QtFatalMsg];
}

bool LogFilterProxyModel::acceptCritical() const
{
    return accepted_types[QtCriticalMsg];
}

bool LogFilterProxyModel::acceptWarning() const
{
    return accepted_types[QtWarningMsg];
}

bool LogFilterProxyModel::acceptInfo() const
{
    return accepted_types[QtInfoMsg];
}

bool LogFilterProxyModel::acceptDebug() const
{
    return accepted_types[QtDebugMsg];
}

void LogFilterProxyModel::setAcceptFatal(bool accepted)
{
    accepted_types[QtFatalMsg] = accepted;
    invalidateFilter();
}

void LogFilterProxyModel::setAcceptCritical(bool accepted)
{
    accepted_types[QtCriticalMsg] = accepted;
    invalidateFilter();
}

void LogFilterProxyModel::setAcceptWarning(bool accepted)
{
    accepted_types[QtWarningMsg] = accepted;
    invalidateFilter();
}

void LogFilterProxyModel::setAcceptInfo(bool accepted)
{
    accepted_types[QtInfoMsg] = accepted;
    invalidateFilter();
}

void LogFilterProxyModel::setAcceptDebug(bool accepted)
{
    accepted_types[QtDebugMsg] = accepted;
    invalidateFilter();
}

bool LogFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool ok;
    auto index = sourceModel()->index(source_row, Log::TypeColumn, source_parent);
    auto type = index.data(Qt::UserRole).toInt(&ok);
    if (ok && type >= 0 && type < accepted_types.size() && !accepted_types[type])
        return false;
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

