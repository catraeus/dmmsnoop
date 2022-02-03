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

#include "QVwMain.hpp"
#include "util/util.hpp"

     QVwMain::QVwMain(QObject *i_parent) : QVwDesgn(":/dmmsnoop/QVwMain.ui", i_parent) {
    QWidget *QWd_root  = getRootWidget();

    QAc_About          = getChild<QAction>(QWd_root, "QAc_About"          );  connect(QAc_About,          SIGNAL(triggered()), SIGNAL(aboutRequest         ()));
    QAc_MiMsgOutAdd    = getChild<QAction>(QWd_root, "QAc_MiMsgOutAdd"    );  connect(QAc_MiMsgOutAdd,    SIGNAL(triggered()), SIGNAL(addMessageRequest    ()));
    QAc_MiMsgListClear = getChild<QAction>(QWd_root, "QAc_MiMsgListClear" );  connect(QAc_MiMsgListClear, SIGNAL(triggered()), SIGNAL(EmMiMsgTabClr        ()));
    QAc_AppConfig      = getChild<QAction>(QWd_root, "QAc_AppConfig"      );  connect(QAc_AppConfig,      SIGNAL(triggered()), SIGNAL(configureRequest     ()));
    QAc_AppQuit        = getChild<QAction>(QWd_root, "QAc_AppQuit"        );  connect(QAc_AppQuit,        SIGNAL(triggered()), SIGNAL(closeRequest         ()));

    QMd_MiMsgGrid.setColumnCount(MTC_TOTAL);
    QMd_MiMsgGrid.setRowCount(0);
    QMd_MiMsgGrid.setHeaderData(MTC_DATA,      Qt::Horizontal,  tr("Data"     ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData(MTC_STATUS,    Qt::Horizontal,  tr("Status"   ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData(MTC_TIMESTAMP, Qt::Horizontal,  tr("Timestamp"), Qt::DisplayRole);
    QTb_MiMsgGrid = getChild<QTableView>(QWd_root, "QTb_MiMsgGrid");
    QTb_MiMsgGrid->setItemDelegate(&tableDelegate);
    QTb_MiMsgGrid->setModel(&QMd_MiMsgGrid);
    timeZero = 0;
}
     QVwMain::~QVwMain() {}

void QVwMain::SetTimeZero(qint64 i_timeZero) {  timeZero = i_timeZero;  return;}
int  QVwMain::MsgAdd(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid) {
  int   count    = QMd_MiMsgGrid.rowCount();

  QMd_MiMsgGrid.insertRow(count); // WARNING There is no check for insertion, insertRow returns a bool

  Qt::AlignmentFlag alignment = Qt::AlignTop;
  setModelData(count, MTC_DATA,      dataDescription);
  setModelData(count, MTC_DATA,      alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_STATUS,    statusDescription);
  setModelData(count, MTC_STATUS,    alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_TIMESTAMP, timeStamp - timeZero);
  setModelData(count, MTC_TIMESTAMP, alignment,   Qt::TextAlignmentRole);
  if(!valid)
    setModelData(count, MTC_STATUS,  QIcon(":/dmmsnoop/images/16x16/error.png"), Qt::DecorationRole);
  QTb_MiMsgGrid->resizeRowToContents(count);
  QTb_MiMsgGrid->scrollToBottom();
  return count;
}
void QVwMain::OnMiMsgRX(quint64 timeStamp,  const QString &statusDescription,  const QString &dataDescription, bool valid) {
  MsgAdd(timeStamp, statusDescription, dataDescription, valid);
}
void QVwMain::OnMiMsgTX(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid) {
    int   index = MsgAdd(timeStamp, statusDescription, dataDescription, valid);
    const QBrush &brush = qApp->palette().alternateBase();
    setModelData(index, MTC_DATA,      brush, Qt::BackgroundRole);
    setModelData(index, MTC_STATUS,    brush, Qt::BackgroundRole);
    setModelData(index, MTC_TIMESTAMP, brush, Qt::BackgroundRole);
}
void QVwMain::OnMiMsgTabClr() {
  int count = QMd_MiMsgGrid.rowCount();
  if(count > 0) {
    QMd_MiMsgGrid.removeRows(0, QMd_MiMsgGrid.rowCount()); // WARNING there is no check here for removal, removeRows returns a bool.
  }
}
void QVwMain::setMessageSendEnabled(bool enabled) {
  QAc_MiMsgOutAdd->setEnabled(enabled);
}
void QVwMain::setModelData(int row, int column, const QVariant &value, int role) {
  bool result = QMd_MiMsgGrid.setData(QMd_MiMsgGrid.index(row, column), value, role);
  assert(result);
}
