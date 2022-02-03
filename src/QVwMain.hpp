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

#ifndef __QVW_MAIN_HPP__
#define __QVW_MAIN_HPP__

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>

#include "QVwDesgn.hpp"
#include "DelgMsgTbl.hpp"

class QVwMain: public QVwDesgn {
    Q_OBJECT

public:

  explicit  QVwMain(QObject *parent=0);
           ~QVwMain();

public slots:

  void OnMiMsgRX(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid);
  void OnMiMsgTX(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid);
  void OnMiMsgTabClr();
  void setMessageSendEnabled(bool enabled);

       signals:

  void aboutRequest();
  void addMessageRequest();
  void EmMiMsgTabClr();
  void configureRequest();

public:
  void SetTimeZero(qint64 i_timeZero);

private:
  enum MessageTableColumn {
    MTC_TIMESTAMP = 0,
    MTC_STATUS    = 1,
    MTC_DATA      = 2,
    MTC_TOTAL     = 3
  };
  qint64 timeZero;

  int  MsgAdd(quint64 timeStamp, const QString &statusDescription, const QString &dataDescription, bool valid);
  void setModelData(int row, int column, const QVariant &value, int role=Qt::DisplayRole);

  QAction              *QAc_About;
  QAction              *QAc_MiMsgOutAdd;
  QAction              *QAc_MiMsgListClear;
  QAction              *QAc_AppConfig;
  QAction              *QAc_AppQuit;

  DelgMsgTbl            tableDelegate;
  QStandardItemModel    QMd_MiMsgGrid;
  QTableView           *QTb_MiMsgGrid;

};

#endif
