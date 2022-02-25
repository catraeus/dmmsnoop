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
#include <QtCore/QMetaType>

#include "AppVersion.hpp"
#include "BuildNo.hpp"

#include "QVwMain.hpp"
#include "util/DmmStr.hpp"

QVwMain::QVwMain(QApplication *i_theApp, QObject *i_parent) : QVwDesgn(":/dmmsnoop/QVwMain.ui", i_parent), theApp(i_theApp) {
  QWd_root      = getRootWidget();
  theDrvIf      = new DrvIf();
  theQVwAbout   = new QVwAbout();
  theQVwConfig  = new QVwConfig(theDrvIf);
  theQVwMsg     = new QVwMsg();

  MSU_WinMain         = NULL;

  BuildActions();
  BuildGrid();
  Connect();
  MRU_CheckTxEn = new CbT<QVwMain>();
  MRU_CheckTxEn->SetCallback(this, &QVwMain::ChkTxEn);
  theQVwConfig->MSU_WinMainCheckTxEn = MRU_CheckTxEn;


  TZ = 0;
}
QVwMain::~QVwMain() {}





void  QVwMain::BuildActions        (void      ) {
  QAc_AppAbout       = getChild<QAction>(QWd_root, "QAc_AppAbout"       );
  QAc_MiMsgOutAdd    = getChild<QAction>(QWd_root, "QAc_MiMsgOutAdd"    );
  QAc_MiMsgListClear = getChild<QAction>(QWd_root, "QAc_MiMsgListClear" );
  QAc_AppConfig      = getChild<QAction>(QWd_root, "QAc_AppConfig"      );
  QAc_AppQuit        = getChild<QAction>(QWd_root, "QAc_AppQuit"        );
  return;
}
void  QVwMain::BuildGrid           (void     ) {
  QMd_MiMsgGrid.setColumnCount(MTC_NUM);
  QMd_MiMsgGrid.setRowCount   (0);

  QMd_MiMsgGrid.setHeaderData (MTC_TS,   Qt::Horizontal,  tr("TS"       ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_RAW,  Qt::Horizontal,  tr("Raw"      ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_STA,  Qt::Horizontal,  tr("Stat"     ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_CHAN, Qt::Horizontal,  tr("Ch"       ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_NOTE, Qt::Horizontal,  tr("Note"     ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_CC,   Qt::Horizontal,  tr("CC"       ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_VELO, Qt::Horizontal,  tr("Vel"      ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_BEND, Qt::Horizontal,  tr("Bend"     ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_POS,  Qt::Horizontal,  tr("Pos"      ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_SONG, Qt::Horizontal,  tr("Song"     ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_PROG, Qt::Horizontal,  tr("Prog"     ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_MTC,  Qt::Horizontal,  tr("MTC"      ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_SYS,  Qt::Horizontal,  tr("Sys"      ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_ERR,  Qt::Horizontal,  tr("Error"    ), Qt::DisplayRole);
  QMd_MiMsgGrid.setHeaderData (MTC_PAD,  Qt::Horizontal,  tr(""         ), Qt::DisplayRole);

  QTb_MiMsgGrid = getChild<QTableView>(QWd_root, "QTb_MiMsgGrid");
  QTb_MiMsgGrid->setItemDelegate(&QDg_MiMsgGrid); // The delegate to pick up after the visitors
  QTb_MiMsgGrid->setModel(&QMd_MiMsgGrid);        // The model that actually knows what's what.

// OK, so the TableView had to actually exist first
  QTb_MiMsgGrid->setColumnWidth(MTC_TS,   108);
  QTb_MiMsgGrid->setColumnWidth(MTC_RAW,   72);
  QTb_MiMsgGrid->setColumnWidth(MTC_STA,   88);
  QTb_MiMsgGrid->setColumnWidth(MTC_CHAN,  32);
  QTb_MiMsgGrid->setColumnWidth(MTC_NOTE,  40);
  QTb_MiMsgGrid->setColumnWidth(MTC_CC,    32);
  QTb_MiMsgGrid->setColumnWidth(MTC_VELO,  40);
  QTb_MiMsgGrid->setColumnWidth(MTC_BEND,  72);
  QTb_MiMsgGrid->setColumnWidth(MTC_POS,   64);
  QTb_MiMsgGrid->setColumnWidth(MTC_SONG,  40);
  QTb_MiMsgGrid->setColumnWidth(MTC_PROG,  40);
  QTb_MiMsgGrid->setColumnWidth(MTC_MTC,  120);
  QTb_MiMsgGrid->setColumnWidth(MTC_SYS,  108);
  QTb_MiMsgGrid->setColumnWidth(MTC_ERR,  120);
  QTb_MiMsgGrid->setColumnWidth(MTC_PAD,   64);
  return;
}
void QVwMain::Connect(void) {
  connect(QAc_AppAbout,       SIGNAL(triggered   ()), this,         SLOT  (OnUiAppAbout   ()));
  connect(QAc_MiMsgOutAdd,    SIGNAL(triggered   ()), theQVwMsg,    SLOT  (show           ()));
  connect(QAc_MiMsgListClear, SIGNAL(triggered   ()), this,         SLOT  (OnUiMsgClr     ()));
  connect(QAc_AppConfig,      SIGNAL(triggered   ()), theQVwConfig, SLOT  (show           ()));
  connect(QAc_AppQuit,        SIGNAL(triggered   ()), theApp,       SLOT  (quit           ()));
  connect(this,               SIGNAL(closeRequest()), theApp,       SLOT  (quit           ()));
  return;
}








void QVwMain::SetTimeZero(quint64 i_TZ) {  TZ = i_TZ;  return;}
int  QVwMain::MsgAdd(quint64 i_TS, Midi *i_tMidi, bool i_val) {
  (void)i_val;
  int               count;
  quint64           bla;

  Qt::AlignmentFlag alignment;
    strcpy(i_miRaw,      i_tMidi->theMS->raw);
    strcpy(i_miSta,      i_tMidi->theMS->stat);
    strcpy(i_miChanDesc, i_tMidi->theMS->ch   );
    strcpy(i_miNoteDesc, i_tMidi->theMS->note );
    strcpy(i_miChChDesc, i_tMidi->theMS->cc   );
    strcpy(i_miNoteVel,  i_tMidi->theMS->vel  );
    strcpy(i_miBend,     i_tMidi->theMS->bend );
    strcpy(i_miPos,      i_tMidi->theMS->pos  );
    strcpy(i_miSong,     i_tMidi->theMS->song );
    strcpy(i_miProg,     i_tMidi->theMS->prog );
    strcpy(i_miMtc,      i_tMidi->theMS->mtc  );
    strcpy(i_miSysCmd,   i_tMidi->theMS->sys  );
    strcpy(i_miErr,      i_tMidi->theMS->err  );

  count = QMd_MiMsgGrid.rowCount();
  QMd_MiMsgGrid.insertRow(count); // WARNING There is no check for insertion, insertRow returns a bool

  alignment = Qt::AlignVCenter;
  bla = i_TS - TZ;
  TimeUsToStrSec(bla, i_tMidi->theMS->TS);

  setModelData(count, MTC_TS,    i_tMidi->theMS->TS );  setModelData(count, MTC_TS,   alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_RAW,   i_miRaw            );  setModelData(count, MTC_RAW,  alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_STA,   i_miSta            );  setModelData(count, MTC_STA,  alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_CHAN,  i_miChanDesc       );  setModelData(count, MTC_CHAN, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_NOTE,  i_miNoteDesc       );  setModelData(count, MTC_NOTE, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_CC,    i_miChChDesc       );  setModelData(count, MTC_CC,   alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_VELO,  i_miNoteVel        );  setModelData(count, MTC_VELO, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_BEND,  i_miBend           );  setModelData(count, MTC_BEND, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_POS,   i_miPos            );  setModelData(count, MTC_POS,  alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_SONG,  i_miSong           );  setModelData(count, MTC_SONG, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_PROG,  i_miProg           );  setModelData(count, MTC_PROG, alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_MTC,   i_miMtc            );  setModelData(count, MTC_MTC,  alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_SYS,   i_miSysCmd         );  setModelData(count, MTC_SYS,  alignment,   Qt::TextAlignmentRole);
  setModelData(count, MTC_ERR,   i_miErr            );  setModelData(count, MTC_ERR,  alignment,   Qt::TextAlignmentRole);
  if(!i_tMidi->GetValid())
    setModelData(count, MTC_STA,  QIcon(":/dmmsnoop/images/16x16/error.png"), Qt::DecorationRole);
  QTb_MiMsgGrid->resizeRowToContents(count);
  QTb_MiMsgGrid->scrollToBottom();
    uint32_t b;
    b = 3;
    if(MSU_WinMain != 0)    MSU_WinMain->Execute(&b);
  return count;
}
void QVwMain::OnMiMsgRX(quint64 i_TS, Midi *i_tMidi, bool valid) {
  MsgAdd(i_TS, i_tMidi, valid);
}
void QVwMain::OnMiMsgTX(quint64 i_TS, Midi *i_tMidi, bool valid) {
  int   index;
  const QBrush &brush = qApp->palette().alternateBase();
  index = MsgAdd(i_TS, i_tMidi, valid);
  for(uint i=0; i<MTC_NUM; i++ )
    setModelData(index, i,  brush, Qt::BackgroundRole);
}
void QVwMain::OnUiMsgClr() {
  int count = QMd_MiMsgGrid.rowCount();
  if(count > 0) {
    QMd_MiMsgGrid.removeRows(0, QMd_MiMsgGrid.rowCount()); // WARNING there is no check here for removal, removeRows returns a bool.
  }
}
void QVwMain::OnTxEn(bool enabled) {
  (void) enabled;
}
void QVwMain::setModelData(int row, int column, const QVariant &value, int role) {
  QMd_MiMsgGrid.setData(QMd_MiMsgGrid.index(row, column), value, role);
}
bool QVwMain::ChkTxEn(void *) { // IS the callback after Config changes any Interface port
  bool txEn = (theDrvIf->DrvNumGet() != -1) && (theDrvIf->PortOutNoGet() != -1);
  fprintf(stdout, "TxEn%c\n", txEn ? '+' : '-'); fflush(stdout);
  QAc_MiMsgOutAdd->setEnabled(txEn);
  return true;
}


