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

#include "GridViewHeader.h"

#include <QFontMetrics>
#include <QPainter>

#include "color.h"

#include <QtDebug>

using namespace qtlabors;

GridViewHeader::GridViewHeader(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent)
{
    QFontMetrics fm{QFont{}};
    setDefaultSectionSize(fm.height());
    setSectionResizeMode(QHeaderView::Fixed);
}

void GridViewHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (logicalIndex == 0) {
        QHeaderView::paintSection(painter, rect, logicalIndex);
    }
    else {
        QStyleOptionHeader option;
        initStyleOption(&option);

        painter->save();

        auto colors = make_color_pair(model()->headerData(logicalIndex, Qt::Horizontal, Qt::BackgroundRole).value<QColor>(), logicalIndex % 2);
        painter->setPen(colors.second);

        // Background
        painter->fillRect(rect, colors.first);

        // Sort Indicator
        option.rect = QRect(rect.left(),
                            rect.bottom() - style()->pixelMetric(QStyle::PM_HeaderMarkSize),
                            rect.width(),
                            style()->pixelMetric(QStyle::PM_HeaderMarkSize));
        if (sortIndicatorSection() == logicalIndex) {
            switch (sortIndicatorOrder()) {
            case Qt::AscendingOrder:
                option.sortIndicator = QStyleOptionHeader::SortUp;
                break;
            case Qt::DescendingOrder:
                option.sortIndicator = QStyleOptionHeader::SortDown;
                break;
            }
            style()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &option, painter, this);
        }

        // Title
        painter->rotate(90);
        QRect rotated {
            rect.y(),
            -rect.x()-rect.width(),
            rect.height()
                 - style()->pixelMetric(QStyle::PM_HeaderMarkSize)
                 - style()->pixelMetric(QStyle::PM_HeaderMargin),
            rect.width()};
        auto title = model()->headerData(logicalIndex, Qt::Horizontal).toString();
        painter->drawText(rotated, Qt::AlignRight | Qt::AlignVCenter, title);

        painter->restore();
    }
}

QSize GridViewHeader::sectionSizeFromContents(int logicalIndex) const
{
    if (logicalIndex == 0)
        return QHeaderView::sectionSizeFromContents(logicalIndex).transposed();
    auto title = model()->headerData(logicalIndex, Qt::Horizontal).toString();
    QFontMetrics fm{QFont{}};
    return QSize(fm.height(),
                 fm.width(title) +
                    style()->pixelMetric(QStyle::PM_HeaderMarkSize) +
                    style()->pixelMetric(QStyle::PM_HeaderMargin));

}
