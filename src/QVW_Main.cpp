/*
 * dmmsnoop - MIDI monitor and prober
 * Copyright (C) 2012 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#include <cassert>

#include <QtWidgets/QApplication>

#include "QVW_Main.hpp"
#include "util.h"

     QVwMain::QVwMain(QObject *parent)
: DesignerView(":/dmmsnoop/QVW_Main.ui", parent)
{
    QWidget *widget = getRootWidget();

    aboutAction     = getChild<QAction>(widget, "aboutAction"    );    connect(aboutAction,     SIGNAL(triggered()), SIGNAL(aboutRequest         ()));
    addAction       = getChild<QAction>(widget, "addAction"      );    connect(addAction,       SIGNAL(triggered()), SIGNAL(addMessageRequest    ()));
    QA_ClearMsgMon  = getChild<QAction>(widget, "QA_ClearMsgMon" );    connect(QA_ClearMsgMon,  SIGNAL(triggered()), SIGNAL(clearMessagesRequest ()));
    configureAction = getChild<QAction>(widget, "configureAction");    connect(configureAction, SIGNAL(triggered()), SIGNAL(configureRequest     ()));
    quitAction      = getChild<QAction>(widget, "quitAction"     );    connect(quitAction,      SIGNAL(triggered()), SIGNAL(closeRequest         ()));

    tableModel.setColumnCount(MESSAGETABLECOLUMN_TOTAL);
    tableModel.setRowCount(0);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_DATA,      Qt::Horizontal,  tr("Data"     ), Qt::DisplayRole);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_STATUS,    Qt::Horizontal,  tr("Status"   ), Qt::DisplayRole);
    tableModel.setHeaderData(MESSAGETABLECOLUMN_TIMESTAMP, Qt::Horizontal,  tr("Timestamp"), Qt::DisplayRole);
    tableView = getChild<QTableView>(widget, "QTV_MidiMsg");
    tableView->setItemDelegate(&tableDelegate);
    tableView->setModel(&tableModel);
    timeZero = 0;
}

QVwMain::~QVwMain() {}

void QVwMain::SetTimeZero(qint64 i_timeZero) {
  timeZero = i_timeZero;
  return;
}


int QVwMain::MsgAdd(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid) {
  int   count    = tableModel.rowCount();
  
  tableModel.insertRow(count); // WARNING There is no check for insertion, insertRow returns a bool

  Qt::AlignmentFlag alignment = Qt::AlignTop;
  setModelData(count, MESSAGETABLECOLUMN_DATA,      dataDescription);
  setModelData(count, MESSAGETABLECOLUMN_DATA,      alignment,   Qt::TextAlignmentRole);
  setModelData(count, MESSAGETABLECOLUMN_STATUS,    statusDescription);
  setModelData(count, MESSAGETABLECOLUMN_STATUS,    alignment,   Qt::TextAlignmentRole);
  setModelData(count, MESSAGETABLECOLUMN_TIMESTAMP, timeStamp - timeZero);
  setModelData(count, MESSAGETABLECOLUMN_TIMESTAMP, alignment,   Qt::TextAlignmentRole);
  if(!valid)
    setModelData(count, MESSAGETABLECOLUMN_STATUS,  QIcon(":/dmmsnoop/images/16x16/error.png"), Qt::DecorationRole);
  tableView->resizeRowToContents(count);
  tableView->scrollToBottom();
  return count;
}

void QVwMain::addReceivedMessage(quint64 timeStamp,  const QString &statusDescription,  const QString &dataDescription, bool valid) {
  MsgAdd(timeStamp, statusDescription, dataDescription, valid);
}

void QVwMain::MsgAddTX(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid) {
    int   index = MsgAdd(timeStamp, statusDescription, dataDescription, valid);
    const QBrush &brush = qApp->palette().alternateBase();
    setModelData(index, MESSAGETABLECOLUMN_DATA,      brush, Qt::BackgroundRole);
    setModelData(index, MESSAGETABLECOLUMN_STATUS,    brush, Qt::BackgroundRole);
    setModelData(index, MESSAGETABLECOLUMN_TIMESTAMP, brush, Qt::BackgroundRole);
}

void QVwMain::clearMessages() {
  int count = tableModel.rowCount();
  if(count > 0) {
    tableModel.removeRows(0, tableModel.rowCount()); // WARNING there is no check here for removal, removeRows returns a bool.
  }
}

void QVwMain::setMessageSendEnabled(bool enabled) {
  addAction->setEnabled(enabled);
}

void QVwMain::setModelData(int row, int column, const QVariant &value, int role) {
  bool result = tableModel.setData(tableModel.index(row, column), value, role);
  assert(result);
}

