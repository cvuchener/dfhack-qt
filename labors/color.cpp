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

#include "color.h"

std::pair<QColor, QColor> make_color_pair(const QColor &color, bool odd_column)
{
    std::pair<QColor, QColor> ret;
    if (odd_column)
        ret.first = color.lighter(105);
    else
        ret.first = color.darker(105);
    if (color.lightnessF() > 0.2140) // gamma 2.2
        ret.second = Qt::black;
    else
        ret.second = Qt::white;
    return ret;
}
