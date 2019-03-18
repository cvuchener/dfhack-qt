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

#include "GridViewDelegate.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "color.h"

#include <QtDebug>

using namespace qtlabors;

GridViewDelegate::GridViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , current_row(-1)
    , current_column(-1)
{
}

GridViewDelegate::~GridViewDelegate()
{
}

void GridViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        // Default delegate for the first column (names)
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    std::pair<QColor, QColor> colors = {Qt::white, Qt::black};
    if (index.data(Qt::BackgroundRole).canConvert<QColor>())
        colors = make_color_pair(index.data(Qt::BackgroundRole).value<QColor>(), index.column() % 2);

    painter->fillRect(option.rect, colors.first);
    if (index.row() == current_row) {
        painter->save();
        painter->setPen(QPen(option.palette.highlight(), 1));
        auto bg = option.palette.highlight().color();
        bg.setAlphaF(0.25);
        painter->fillRect(option.rect, bg);
        painter->drawLine(option.rect.topLeft(), option.rect.topRight());
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        painter->restore();
    }
    if (index.flags() & Qt::ItemIsUserCheckable) {
        auto checked = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
        if (checked == Qt::Checked) {
            painter->fillRect(option.rect.adjusted(2, 2, -2, -2),
                              colors.second);
        }
        painter->save();
        painter->setPen(checked == Qt::Checked
                          ? colors.first
                          : colors.second);
        painter->setFont(option.font);
        painter->drawText(option.rect,
                          Qt::AlignHCenter | Qt::AlignVCenter,
                          index.data(Qt::DisplayRole).toString());
        painter->restore();
    }
    else {
        painter->save();
        painter->setPen(colors.second);
        painter->setFont(option.font);
        painter->drawText(option.rect,
                          Qt::AlignHCenter | Qt::AlignVCenter,
                          index.data(Qt::DisplayRole).toString());
        painter->restore();
    }
}

bool GridViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    bool enter = false;
    if (event->type() == QEvent::MouseMove) {
        if (current_row != index.row() || current_column != index.column())
            enter = true;
        current_row = index.row();
        current_column = index.column();
    }

    if (index.data(Qt::CheckStateRole).canConvert<bool>()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton && option.rect.contains(me->pos())) {
                auto checked = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
                model->setData(index,
                               checked == Qt::Checked ? Qt::Unchecked : Qt::Checked,
                               Qt::CheckStateRole);
            }
        }
        else if (enter) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            if (me->buttons() & Qt::LeftButton) {
                auto checked = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
                model->setData(index,
                               checked == Qt::Checked ? Qt::Unchecked : Qt::Checked,
                               Qt::CheckStateRole);
            }
        }
        return false;
    }
    else {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
}

