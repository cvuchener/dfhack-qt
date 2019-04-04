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

#include "LogWindow.h"
#include "ui_LogWindow.h"

#include "Log.h"

using namespace qapplication;

enum FilterMode {
    FixedString,
    Wildcard,
    RegExp,
};

LogWindow::LogWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    ui->window_menu->addAction(ui->message_type_toolbar->toggleViewAction());
    ui->window_menu->addSeparator();
    ui->window_menu->addAction(ui->search_dock->toggleViewAction());

    filter.setSourceModel(Log::model());
    ui->log_view->setModel(&filter);

    ui->show_fatal_action->setChecked(filter.acceptFatal());
    ui->show_critical_action->setChecked(filter.acceptCritical());
    ui->show_warning_action->setChecked(filter.acceptWarning());
    ui->show_info_action->setChecked(filter.acceptInfo());
    ui->show_debug_action->setChecked(filter.acceptDebug());

    connect(ui->show_fatal_action, &QAction::toggled,
            &filter, &LogFilterProxyModel::setAcceptFatal);
    connect(ui->show_critical_action, &QAction::toggled,
            &filter, &LogFilterProxyModel::setAcceptCritical);
    connect(ui->show_warning_action, &QAction::toggled,
            &filter, &LogFilterProxyModel::setAcceptWarning);
    connect(ui->show_info_action, &QAction::toggled,
            &filter, &LogFilterProxyModel::setAcceptInfo);
    connect(ui->show_debug_action, &QAction::toggled,
            &filter, &LogFilterProxyModel::setAcceptDebug);

    ui->filter_column->addItem(tr("Message"), Log::MessageColumn);
    ui->filter_column->addItem(tr("Location"), Log::LocationColumn);

    ui->filter_mode->addItem(tr("Fixed string"), FixedString);
    ui->filter_mode->addItem(tr("Wildcard"), Wildcard);
    ui->filter_mode->addItem(tr("RegExp"), RegExp);

    connect(ui->filter_edit, &QLineEdit::textChanged,
            this, &LogWindow::updateSearchFilter);
    connect(ui->filter_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogWindow::updateSearchFilter);

    on_filter_case_sensitive_stateChanged(ui->filter_case_sensitive->checkState());
}

LogWindow::~LogWindow()
{
}

void LogWindow::on_filter_column_currentIndexChanged(int index)
{
    filter.setFilterKeyColumn(ui->filter_column->currentData().toInt());
}

void LogWindow::on_filter_case_sensitive_stateChanged(int state)
{
    if (state == Qt::Checked)
        filter.setFilterCaseSensitivity(Qt::CaseSensitive);
    else
        filter.setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void LogWindow::updateSearchFilter()
{
    switch (ui->filter_mode->currentData().toInt()) {
    case FixedString:
        filter.setFilterFixedString(ui->filter_edit->text());
        break;
    case Wildcard:
        filter.setFilterWildcard(ui->filter_edit->text());
        break;
    case RegExp:
        filter.setFilterRegExp(ui->filter_edit->text());
        break;
    }
}
