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

#ifndef QT_QAPPLICATION_LOG_FILTER_PROXY_MODEL_H
#define QT_QAPPLICATION_LOG_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>

#include <array>

namespace qapplication
{

class LogFilterProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool fatal READ acceptFatal WRITE setAcceptFatal)
    Q_PROPERTY(bool critical READ acceptCritical WRITE setAcceptCritical)
    Q_PROPERTY(bool warning READ acceptWarning WRITE setAcceptWarning)
    Q_PROPERTY(bool info READ acceptInfo WRITE setAcceptInfo)
    Q_PROPERTY(bool debug READ acceptDebug WRITE setAcceptDebug)
public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);
    ~LogFilterProxyModel() override;

    bool acceptFatal() const;
    bool acceptCritical() const;
    bool acceptWarning() const;
    bool acceptInfo() const;
    bool acceptDebug() const;

public slots:
    void setAcceptFatal(bool);
    void setAcceptCritical(bool);
    void setAcceptWarning(bool);
    void setAcceptInfo(bool);
    void setAcceptDebug(bool);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    std::array<bool, 5> accepted_types;
};

}

#endif

