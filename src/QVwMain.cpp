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

    QAc_AppAbout       = getChild<QAction>(QWd_root, "QAc_AppAbout"       );  connect(QAc_AppAbout,       SIGNAL(triggered()), SIGNAL(EmAppAbout           ()));
    QAc_MiMsgOutAdd    = getChild<QAction>(QWd_root, "QAc_MiMsgOutAdd"    );  connect(QAc_MiMsgOutAdd,    SIGNAL(triggered()), SIGNAL(EmMiMsgTXAdd         ()));
    QAc_MiMsgListClear = getChild<QAction>(QWd_root, "QAc_MiMsgListClear" );  connect(QAc_MiMsgListClear, SIGNAL(triggered()), SIGNAL(EmMiMsgTabClr        ()));
    QAc_AppConfig      = getChild<QAction>(QWd_root, "QAc_AppConfig"      );  connect(QAc_AppConfig,      SIGNAL(triggered()), SIGNAL(EmAppConfig     ()));
    QAc_AppQuit        = getChild<QAction>(QWd_root, "QAc_AppQuit"        );  connect(QAc_AppQuit,        SIGNAL(triggered()), SIGNAL(closeRequest         ()));

    QMd_MiMsgGrid.setColumnCount(MTC_NUM);
    QMd_MiMsgGrid.setRowCount   (0);
    QMd_MiMsgGrid.setHeaderData (MTC_TS,   Qt::Horizontal,  tr("TS"       ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_STAT, Qt::Horizontal,  tr("Status"   ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_DATA, Qt::Horizontal,  tr("Data"     ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_CHAN, Qt::Horizontal,  tr("Ch"       ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_NOTE, Qt::Horizontal,  tr("Note"     ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_VELO, Qt::Horizontal,  tr("Vel"      ), Qt::DisplayRole);
    QMd_MiMsgGrid.setHeaderData (MTC_SYS,  Qt::Horizontal,  tr("Sys"      ), Qt::DisplayRole);
    QTb_MiMsgGrid = getChild<QTableView>(QWd_root, "QTb_MiMsgGrid");
    QTb_MiMsgGrid->setItemDelegate(&QDg_MiMsgGrid); // The delegate to pick up after the visitors
    QTb_MiMsgGrid->setModel(&QMd_MiMsgGrid);        // The model that actually knows what's what.
    timeZero = 0;
}
     QVwMain::~QVwMain() {}

void QVwMain::SetTimeZero(qint64 i_timeZero) {  timeZero = i_timeZero;  return;}
int  QVwMain::MsgAdd(quint64 i_TS, const QString &i_miStatDesc, const QString &i_miDataDesc, bool i_val) {
  int               count;
  Qt::AlignmentFlag alignment;
  QString  i_miChanDesc;
  QString  i_miNoteDesc;
  QString  i_miNoteVel;
  QString  i_miSysCmd;

  i_miChanDesc = "CHAN";
  i_miNoteDesc = "NOTE";
  i_miNoteVel  = "VEL";
  i_miSysCmd   = "SYS";

  count = QMd_MiMsgGrid.rowCount();
  QMd_MiMsgGrid.insertRow(count); // WARNING There is no check for insertion, insertRow returns a bool

  alignment = Qt::AlignTop;
  setModelData(count, MTC_TS,    i_TS - timeZero);  setModelData(count, MTC_TS,   alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_STAT,  i_miStatDesc   );  setModelData(count, MTC_STAT, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_DATA,  i_miDataDesc   );  setModelData(count, MTC_DATA, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_CHAN,  i_miChanDesc   );  setModelData(count, MTC_CHAN, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_NOTE,  i_miNoteDesc   );  setModelData(count, MTC_NOTE, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_VELO,  i_miNoteVel    );  setModelData(count, MTC_VELO, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_SYS,   i_miSysCmd     );  setModelData(count, MTC_SYS,  alignment,   Qt::TextAlignmentRole);
  if(!i_val)
    setModelData(count, MTC_STAT,  QIcon(":/dmmsnoop/images/16x16/error.png"), Qt::DecorationRole);
  QTb_MiMsgGrid->resizeRowToContents(count);
  QTb_MiMsgGrid->scrollToBottom();
  return count;
}
void QVwMain::OnMiMsgRX(quint64 i_TS, const QString &i_miMsgStatStr,  const QString &i_miMsgDataStr, bool valid) {
  MsgAdd(i_TS, i_miMsgStatStr, i_miMsgDataStr, valid);
}
void QVwMain::OnMiMsgTX(quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, bool valid) {
  int   index;
  const QBrush &brush = qApp->palette().alternateBase();
  index = MsgAdd(i_TS, i_miMsgStatStr, i_miMsgDataStr, valid);
  setModelData(index, MTC_DATA,  brush, Qt::BackgroundRole);
  setModelData(index, MTC_STAT,  brush, Qt::BackgroundRole);
  setModelData(index, MTC_TS,    brush, Qt::BackgroundRole);
}
void QVwMain::OnMiMsgTabClr() {
  int count = QMd_MiMsgGrid.rowCount();
  if(count > 0) {
    QMd_MiMsgGrid.removeRows(0, QMd_MiMsgGrid.rowCount()); // WARNING there is no check here for removal, removeRows returns a bool.
  }
}
void QVwMain::OnMiMsgTxEn(bool enabled) {
  QAc_MiMsgOutAdd->setEnabled(enabled);
}
void QVwMain::setModelData(int row, int column, const QVariant &value, int role) {
  QMd_MiMsgGrid.setData(QMd_MiMsgGrid.index(row, column), value, role);
}



