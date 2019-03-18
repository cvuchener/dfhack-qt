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

#include <Core.h>

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
{
    ui->setupUi(this);

    updateUnitModel();

    QSettings gridview_settings("default_gridviews.dtg", QSettings::IniFormat);
    int gridview_count = gridview_settings.beginReadArray("gridviews");
    gridviews.reserve(gridview_count);
    for (int i = 0; i < gridview_count; ++i) {
        gridview_settings.setArrayIndex(i);
        gridviews.emplace_back(new gridview(in_place, gridview_settings, &model));
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

    connect(event_proxy.get(), &EventProxy::mapLoaded,
            this, &MainWindow::updateUnitModel);
    connect(event_proxy.get(), &EventProxy::mapUnloaded,
            this, &MainWindow::clearUnitModel);
    connect(event_proxy.get(), &EventProxy::embarkScreenOpened,
            this, &MainWindow::updateUnitModel);
    connect(event_proxy.get(), &EventProxy::embarkScreenClosed,
            this, &MainWindow::updateUnitModel);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_suspend_action_triggered()
{
    ui->suspend_action->setEnabled(false);
    model.beginDataAccess();
    ui->resume_action->setEnabled(true);
}

void MainWindow::on_resume_action_triggered()
{
    ui->resume_action->setEnabled(false);
    model.endDataAccess();
    ui->suspend_action->setEnabled(true);
}

void MainWindow::updateUnitModel()
{
    model.beginDataAccess();
    model.endDataAccess();
    switch (model.getMode()) {
    case UnitModel::Mode::None:
        setWindowTitle(tr("Labors — No game loaded"));
        ui->suspend_action->setEnabled(false);
        break;
    case UnitModel::Mode::Embark:
        setWindowTitle(tr("Labors — Embark"));
        ui->suspend_action->setEnabled(true);
        break;
    case UnitModel::Mode::Fortress:
        setWindowTitle(tr("Labors — Fortress"));
        ui->suspend_action->setEnabled(true);
        break;
    }
    ui->resume_action->setEnabled(false);
}

void MainWindow::clearUnitModel()
{
    model.clearUnitList();
    setWindowTitle(tr("Labors — No game loaded"));
    ui->suspend_action->setEnabled(false);
    ui->resume_action->setEnabled(false);
}
