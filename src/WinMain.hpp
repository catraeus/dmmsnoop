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
#include "libs/CaesCallBack.hpp"

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>

#include "QVwAbout.hpp"
#include "QVwConfig.hpp"
#include "QVwMsg.hpp"

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
      MTC_CHAN  = 0x00000003U,
      MTC_NOTE  = 0x00000004U,
      MTC_CC    = 0x00000005U,
      MTC_VELO  = 0x00000006U,
      MTC_BEND  = 0x00000007U,
      MTC_POS   = 0x00000008U,
      MTC_SONG  = 0x00000009U,
      MTC_PROG  = 0x0000000AU,
      MTC_MTC   = 0x0000000BU,
      MTC_SYS   = 0x0000000CU,
      MTC_ERR   = 0x0000000DU,
      MTC_PAD   = 0x0000000EU,
      MTC_NUM   = 0x0000000FU,
      MTC_BIG   = 0xFFFFFFFFU
    };
  public:
                 QVwMain        (QApplication *i_theApp, QObject *parent=0);
                ~QVwMain        (                 );
    QVwMsg      *WinMsgGet      (void) {return theQVwMsg;};
    QVwConfig   *WinConfigGet   (void) {return theQVwConfig;};
    DrvIf       *DrvIfGet       (void) {return theDrvIf;};
  public slots:

    void  OnMiMsgRX      (quint64 i_TS, Midi *i_tMidi, bool i_val);
    void  OnMiMsgTX      (quint64 i_TS, Midi *i_tMidi, bool i_val);
    void  OnUiAppAbout   (void      ) {theQVwAbout->show();};
    void  OnUiMsgClr     (void      );
    void  OnTxEn         (bool i_en );

         signals:
    void  EmMiMsgTabClr  (void      );

  public:
    void  SetTimeZero    (quint64 i_TZ);

  private:
    void  BuildActions   (void);
    void  BuildGrid      (void);
    void  Connect        (void);
    int   MsgAdd         (quint64 i_TS, Midi *i_tMidi, bool i_val);
    void  setModelData   (int     i_R,  int   i_C, const QVariant &value, int role=Qt::DisplayRole);
    bool  ChkTxEn        (void *);
  public:
    CbV               *MSU_WinMain; // This Works.
  private:
    QApplication         *theApp;
    QWidget              *QWd_root;
    qint64                TZ;

    QAction              *QAc_AppAbout;
    QAction              *QAc_MiMsgOutAdd;
    QAction              *QAc_MiMsgListClear;
    QAction              *QAc_AppConfig;
    QAction              *QAc_AppQuit;

    DelgMsgTbl            QDg_MiMsgGrid;
    QStandardItemModel    QMd_MiMsgGrid;
    QTableView           *QTb_MiMsgGrid;

    CbT<QVwMain>  *MRU_CheckTxEn;

    QVwAbout      *theQVwAbout;
    QVwMsg        *theQVwMsg;
    QVwConfig     *theQVwConfig;

    DrvIf         *theDrvIf;

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
