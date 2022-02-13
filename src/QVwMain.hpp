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

#include "util/DmmTypes.hpp"
#include "util/TrMsg.hpp"

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>

#include "QVwDesgn.hpp"
#include "DelgMsgTbl.hpp"

#include "Midi.hpp"
#include "DrvIf.hpp"

class QVwMain: public QVwDesgn {
  Q_OBJECT
  private:
    enum eMsgTabCols {
      MTC_TS    = 0x00000000U,
      MTC_RAW   = 0x00000001U,
      MTC_STA   = 0x00000002U,
      MTC_STAT  = 0x00000003U,
      MTC_DATA  = 0x00000004U,
      MTC_CHAN  = 0x00000005U,
      MTC_NOTE  = 0x00000006U,
      MTC_CC    = 0x00000007U,
      MTC_VELO  = 0x00000008U,
      MTC_BEND  = 0x00000009U,
      MTC_POS   = 0x0000000AU,
      MTC_SONG  = 0x0000000BU,
      MTC_PROG  = 0x0000000CU,
      MTC_MTC   = 0x0000000DU,
      MTC_SYS   = 0x0000000EU,
      MTC_ERR   = 0x0000000FU,
      MTC_PAD   = 0x00000010U,
      MTC_NUM   = 0x00000011U,
      MTC_BIG   = 0xFFFFFFFFU
    };
  public:
          QVwMain        (QObject *parent=0);
         ~QVwMain        (                 );

  public slots:

    void  OnMiMsgRX      (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, Midi *i_tMidi, bool i_val);
    void  OnMiMsgTX      (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, Midi *i_tMidi, bool i_val);
    void  OnMiMsgTabClr  (void      );
    void  OnMiMsgTxEn    (bool i_en );

         signals:
    void  EmAppAbout     (void      );
    void  EmMiMsgTXAdd   (void      );
    void  EmMiMsgTabClr  (void      );
    void  EmAppConfig    (void      );

  public:
    void  SetTimeZero    (quint64 i_TZ);

  private:
    int   MsgAdd         (quint64 i_TS, const QString &i_miMsgStatStr, const QString &i_miMsgDataStr, Midi *i_tMidi, bool i_val);
    void  setModelData   (int     i_R,  int   i_C, const QVariant &value, int role=Qt::DisplayRole);

  public:
  private:
    qint64                TZ;

    QAction              *QAc_AppAbout;
    QAction              *QAc_MiMsgOutAdd;
    QAction              *QAc_MiMsgListClear;
    QAction              *QAc_AppConfig;
    QAction              *QAc_AppQuit;

    DelgMsgTbl            QDg_MiMsgGrid;
    QStandardItemModel    QMd_MiMsgGrid;
    QTableView           *QTb_MiMsgGrid;

  char  i_miRaw     [64];
  char  i_miSta     [64];
  char  i_miChanDesc[ 4];
  char  i_miNoteDesc[ 4];
  char  i_miChChDesc[ 4];
  char  i_miNoteVel [ 4];
  char  i_miBend    [16];
  char  i_miPos     [16];
  char  i_miSong    [ 4];
  char  i_miProg    [ 4];
  char  i_miMtc     [32];
  char  i_miSysCmd  [32];
  char  i_miErr     [32];

};

#endif
