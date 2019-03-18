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

#ifndef QT_LABORS_VIEW_COLUMN_H
#define QT_LABORS_VIEW_COLUMN_H

#include <QColor>
#include <QObject>
#include <QString>

namespace qtlabors
{

struct Unit;

class ViewColumn: public QObject
{
    Q_OBJECT
public:
    explicit ViewColumn(const QString &title, const QColor &color, QObject *parent = nullptr);
    ~ViewColumn() override;

    virtual QVariant headerData(int role = Qt::DisplayRole) const;
    virtual QVariant data(const Unit &unit, int role = Qt::DisplayRole) const;
    virtual bool setData(const Unit &unit, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const Unit &unit) const;

signals:
    void dataChanged(int unit_id);

private:
    QString title;
    QColor color;
};

}

#endif
