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

#ifndef QT_LABORS_MAIN_WINDOW_H
#define QT_LABORS_MAIN_WINDOW_H

#include <QMainWindow>

#include <QSortFilterProxyModel>

#include <memory>

#include "UnitModel.h"
#include "ChangesModel.h"

namespace qtlabors
{

class EventProxy;

namespace Ui { class MainWindow; }

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(std::shared_ptr<EventProxy> &&proxy, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_refresh_action_triggered();
    void on_apply_changes_action_triggered();
    void on_clear_changes_action_triggered();
    void on_remove_change_button_clicked();

    void changeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::shared_ptr<EventProxy> event_proxy;
    UnitModel model;
    ChangesModel changes;
    struct gridview;
    std::vector<std::unique_ptr<gridview>> gridviews;
};

}

#endif
