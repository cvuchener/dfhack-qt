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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>
#include <QTreeView>

#include "EventProxy.h"
#include "GridView.h"
#include "GridViewDelegate.h"
#include "GridViewHeader.h"

using namespace qtlabors;

static constexpr struct in_place_t {} in_place;

struct MainWindow::gridview
{
    GridView model;
    GridViewDelegate delegate;
    GridViewHeader header;
    QSortFilterProxyModel filter;

    template<typename... Args>
    explicit gridview(in_place_t, Args &&... args): model(std::forward<Args>(args)...) {}
};

MainWindow::MainWindow(std::shared_ptr<EventProxy> &&proxy, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , event_proxy(std::move(proxy))
    , changes(&model)
{
    ui->setupUi(this);

    ui->window_menu->addAction(ui->changes_dock->toggleViewAction());
    ui->window_menu->addSeparator();
    ui->window_menu->addAction(ui->tool_bar->toggleViewAction());

    model.refresh();

    QSettings gridview_settings("default_gridviews.dtg", QSettings::IniFormat);
    int gridview_count = gridview_settings.beginReadArray("gridviews");
    gridviews.reserve(gridview_count);
    for (int i = 0; i < gridview_count; ++i) {
        gridview_settings.setArrayIndex(i);
        gridviews.emplace_back(new gridview(in_place, gridview_settings, &model, &changes));
        auto gv = gridviews.back().get();

        connect(ui->filter_edit, &QLineEdit::textChanged,
                &gv->filter, &QSortFilterProxyModel::setFilterWildcard);

        gv->filter.setSourceModel(&gv->model);
        gv->filter.setFilterKeyColumn(0);
        gv->filter.setFilterCaseSensitivity(Qt::CaseInsensitive);

        auto view = new QTreeView;
        view->setHeader(&gv->header);
        view->setSortingEnabled(true);
        view->setItemDelegate(&gv->delegate);
        view->setModel(&gv->filter);
        view->setMouseTracking(true);
        gv->header.setSectionResizeMode(0, QHeaderView::ResizeToContents);
        ui->gridview_tabs->addTab(view, gridview_settings.value("name").toString());
    }
    gridview_settings.endArray();

    ui->changes_view->setModel(&changes);
    ui->remove_change_button->setEnabled(false);
    connect(ui->changes_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::changeSelectionChanged);

    connect(event_proxy.get(), &EventProxy::mapLoaded,
            this, &MainWindow::on_refresh_action_triggered);
    connect(event_proxy.get(), &EventProxy::gamePaused,
            this, &MainWindow::on_refresh_action_triggered);
    connect(event_proxy.get(), &EventProxy::embarkScreenOpened,
            this, &MainWindow::on_refresh_action_triggered);
    connect(event_proxy.get(), &EventProxy::embarkScreenClosed,
            this, &MainWindow::on_refresh_action_triggered);

}

MainWindow::~MainWindow()
{
}

void MainWindow::on_refresh_action_triggered()
{
    model.refresh();
    switch (model.getMode()) {
    case UnitModel::Mode::None:
        setWindowTitle(tr("Labors — No game loaded"));
        break;
    case UnitModel::Mode::Embark:
        setWindowTitle(tr("Labors — Embark"));
        break;
    case UnitModel::Mode::Fortress:
        setWindowTitle(tr("Labors — Fortress"));
        break;
    }
}

void MainWindow::on_apply_changes_action_triggered()
{
    changes.apply();
    model.refresh();
}

void MainWindow::on_clear_changes_action_triggered()
{
    changes.reset();
    model.refresh();
}

void MainWindow::on_remove_change_button_clicked()
{
    auto selection = ui->changes_view->selectionModel();
    // removing rows invalidate indices, only one can be removed
    auto row = selection->selectedRows().first();
    changes.removeRow(row.row(), row.parent());
}

void MainWindow::changeSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    if (auto selection = qobject_cast<const QItemSelectionModel *>(QObject::sender()))
        ui->remove_change_button->setEnabled(selection->hasSelection());
}
