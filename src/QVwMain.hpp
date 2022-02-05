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

#include <util/TrMsg.hpp>

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>

#include "QVwDesgn.hpp"
#include "DelgMsgTbl.hpp"

class QVwMain: public QVwDesgn {
  Q_OBJECT
  private:
    enum eMsgTabCols {
      MTC_TS    = 0x00000000U,
      MTC_STAT  = 0x00000001U,
      MTC_DATA  = 0x00000002U,
      MTC_CHAN  = 0x00000003U,
      MTC_NOTE  = 0x00000004U,
      MTC_VELO  = 0x00000005U,
      MTC_SYS   = 0x00000006U,
      MTC_PAD   = 0x00000007U,
      MTC_NUM   = 0x00000008U,
      MTC_BIG   = 0xFFFFFFFFU
    };
  public:
          QVwMain        (QObject *parent=0);
         ~QVwMain        (                 );

  public slots:

    void  OnMiMsgRX      (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, bool i_val);
    void  OnMiMsgTX      (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, bool i_val);
    void  OnMiMsgTabClr  (void      );
    void  OnMiMsgTxEn    (bool i_en );

         signals:
    void  EmAppAbout     (void      );
    void  EmMiMsgTXAdd   (void      );
    void  EmMiMsgTabClr  (void      );
    void  EmAppConfig    (void      );

  public:
    void  SetTimeZero    (qint64 i_timeZero);

  private:
    int   MsgAdd         (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, bool i_val);
    void  setModelData   (int     i_R,  int   i_C, const QVariant &value, int role=Qt::DisplayRole);

  public:
  private:
    qint64                timeZero;

    QAction              *QAc_AppAbout;
    QAction              *QAc_MiMsgOutAdd;
    QAction              *QAc_MiMsgListClear;
    QAction              *QAc_AppConfig;
    QAction              *QAc_AppQuit;

    DelgMsgTbl            QDg_MiMsgGrid;
    QStandardItemModel    QMd_MiMsgGrid;
    QTableView           *QTb_MiMsgGrid;
};

#endif
