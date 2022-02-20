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

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <QtCore/QDebug>
#include <QtCore/QMetaType>

#include "Ctrl.hpp"

#include "util/DmmTypes.hpp"
#include "util/Error.hpp"
#include "util/DmmStr.hpp"




Ctrl:: Ctrl(App *i_theApp, QObject *i_parent) : QObject(i_parent),  theApp(i_theApp) {

//==================================================================================================
//==== Do the important first things.


  theDrvIf     = new DrvIf();
  theMidi      = new Midi();
  theQVwMain   = new QVwMain();
  theQVwConfig = new QVwConfig(theDrvIf);
  theQVwAbout  = new QVwAbout();
  theQVwMsg    = new QVwMsg();
  theQVwErr    = new QVwErr();
  theTrMsg     = TrMsg::GetInstance();


//==== Setup About
  BuildDrvIf       ();
  BuildWinAbout    ();
  theQVwConfig->Build();
  ConnSigWinConfig ();
  BuildWinMain     ();
  ConnSigWinMain   ();
  ConnSigWinMsg    ();
  ConnSigDrvIf     ();
  ConnSigApp       ();
}
Ctrl::~Ctrl() {
// Disconnect theDrvIf signals handled by the controller before the theDrvIf is deleted.
  disconnect(theDrvIf, SIGNAL(EmMiMsgRx    (quint64, const QByteArray &)), this, SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  disconnect(theDrvIf, SIGNAL(EmDrvChange  (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(theDrvIf, SIGNAL(EmPortInpChg (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(theDrvIf, SIGNAL(EmPortOutChg (int                        )), this, SLOT(OnMidiDrvChg()));
}

void Ctrl::BuildDrvIf      (void) {
  theDrvIf->SetMidi(theMidi);
  return;
}
void Ctrl::BuildWinAbout   (void) {
  theQVwAbout->setMajorVersion(DMMSNOOP_MAJOR_VERSION);
  theQVwAbout->setMinorVersion(DMMSNOOP_MINOR_VERSION);
  theQVwAbout->setRevision(DMMSNOOP_REVISION);
  return;
}
void Ctrl::BuildWinMain    (void) {
  quint64  TS;
  char     tStr[256];
  bool     drvReady;

  TS = GetTS();
  TimeUsToStrSec(TS, tStr);
  fprintf(stdout, "MICROSECONDS:  %s\n", tStr);
  drvReady = theDrvIf->DrvReadyGet();
  theQVwMain->OnMiMsgTxEn(drvReady);
  theQVwMain->SetTimeZero(TS);
  MRU_WinMain = new CbT<Ctrl>();
  MRU_WinMain->SetCallback(this, &Ctrl::OnTestCb);
  theQVwMain->MSU_WinMain = MRU_WinMain;
  return;
}



void Ctrl::ConnSigWinConfig(void) {
  connect(theQVwConfig, SIGNAL(EmMidiDrvChg      (int                        )),  theDrvIf,      SLOT(OnDrvChg(int)));
  connect(theQVwConfig, SIGNAL(EmModeIgnActSnChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnActSnChg(bool)));
  connect(theQVwConfig, SIGNAL(EmModeIgnSysExChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnSysExChg(bool)));
  connect(theQVwConfig, SIGNAL(EmModeIgnMiTimChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnMiTimChg(bool)));
  connect(theQVwConfig, SIGNAL(EmPortInpChg      (int                        )),  theDrvIf,      SLOT(OnPortInpChg(int)));
  connect(theQVwConfig, SIGNAL(EmPortOutChg      (int                        )),  theDrvIf,      SLOT(OnPortOutChg(int)));
  return;
}
void Ctrl::ConnSigWinMain  (void) {
  connect(theQVwMain, SIGNAL(EmAppAbout          (                           )),  theQVwAbout,  SLOT(show()));
  connect(theQVwMain, SIGNAL(EmMiMsgTXAdd        (                           )),  theQVwMsg,    SLOT(show()));
  connect(theQVwMain, SIGNAL(EmMiMsgTabClr       (                           )),  theQVwMain,   SLOT(OnMiMsgTabClr()));
  connect(theQVwMain, SIGNAL(EmAppConfig         (                           )),  theQVwConfig, SLOT(show()));
  connect(theQVwMain, SIGNAL(closeRequest        (                           )),  theApp,       SLOT(quit()));
  return;
}
void Ctrl::ConnSigWinMsg   (void) {
  connect(theQVwMsg, SIGNAL(EmMsgSend            (const QString &            )),  this,          SLOT(OnMiMsgTx(const QString &)));
  return;
}
void Ctrl::ConnSigDrvIf    (void) {
  connect(theDrvIf, SIGNAL(EmMiMsgRx              (quint64, const QByteArray &)),  this,          SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  connect(theDrvIf, SIGNAL(EmDrvChange            (int                        )),  theQVwConfig, SLOT(OnMidiDrvChg(int)));
  connect(theDrvIf, SIGNAL(EmDrvChange            (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmModeIgnActSnChg      (bool                       )),  theQVwConfig, SLOT(OnModeIgnActSnChg(bool)));
  connect(theDrvIf, SIGNAL(EmModeIgnSysExChg      (bool                       )),  theQVwConfig, SLOT(OnModeIgnSysExChg(bool)));
  connect(theDrvIf, SIGNAL(EmModeIgnMiTimChg      (bool                       )),  theQVwConfig, SLOT(OnModeIgnMiTimChg(bool)));
  connect(theDrvIf, SIGNAL(EmPortInpAdd           (int, QString               )),  theQVwConfig, SLOT(OnPortInpAdd(int, QString)));
  connect(theDrvIf, SIGNAL(EmPortInpChg           (int                        )),  theQVwConfig, SLOT(OnPortInpChg(int)));
  connect(theDrvIf, SIGNAL(EmPortInpChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmPortInpDel           (int                        )),  theQVwConfig, SLOT(OnPortInpDel(int)));
  connect(theDrvIf, SIGNAL(EmPortOutAdd           (int, QString               )),  theQVwConfig, SLOT(OnPortOutAdd(int, QString)));
  connect(theDrvIf, SIGNAL(EmPortOutChg           (int                        )),  theQVwConfig, SLOT(OnPortOutChg(int)));
  connect(theDrvIf, SIGNAL(EmPortOutChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmPortOutDel           (int                        )),  theQVwConfig, SLOT(OnPortOutDel(int)));
  return;
}
void Ctrl::ConnSigApp      (void) {
  connect( theApp,  SIGNAL(eventError             (QString                    )),  theQVwErr,    SLOT(setMessage(QString)));
  connect( theApp,  SIGNAL(eventError             (QString                    )),  theQVwErr,    SLOT(show()));
  return;
}









void    Ctrl::QVwErrShow       (const QString &message) {
  theQVwErr->setMessage(message);
  theQVwErr->show();
}
void    Ctrl::MiMsgDatBytesStr (const QByteArray &i_msgbb, char *o_dStr, int i_dexLast) {
  uint        tInt;
  char        tStr[32];

  if(i_dexLast == -1)
    i_dexLast = i_msgbb.count() - 1;
  o_dStr[0] =  '\0';
  for(int i = 1; i <= i_dexLast; i++) {
    tInt = (uint)(i_msgbb[i]);
    ByteToString(tInt, tStr);
    if(i > 1) strcat(o_dStr, " ");
    strcat(o_dStr, tStr);
  }
  sprintf(tStr, " (%d bytes)", i_dexLast);
  strcat(o_dStr, tStr);
  return;
}
void    Ctrl::OnMidiDrvChg     (                      ) {
  theQVwMain->OnMiMsgTxEn((theDrvIf->DrvNumGet() != -1) && (theDrvIf->PortOutNoGet() != -1));
}
void    Ctrl::OnMiMsgTx        (const QString &i_miMsgStr) {
  QStringList lines;
  QStringList bytes;
  uint        byteCount;
  uint        lineCount;
  bool        success;
  QByteArray  miMsgBytes;
  QString     byteStr;
  uint        value;
  quint64     TS;
  uint        miMsgLen;
  uint        miBytes[1024];

  lines = i_miMsgStr.split('\n', QString::SkipEmptyParts);
  lineCount = lines.count();
  if(lineCount == 0) {
    miMsgBytes.clear();
    miMsgLen = 0;
    //========================================================================
    theMidi->Parse(miMsgLen, miBytes);
    TS = GetTS();
    theQVwMain->OnMiMsgTX(TS, theMidi, valid);
  }
  for(uint lineDex=0; lineDex<lineCount; lineDex++) {
    bytes = lines[lineDex].split(' ', QString::SkipEmptyParts);// MAGICK The GUI gives us the i_miMsgStr as text ...Convert the i_miMsgStr to bytes.
    byteCount = bytes.count();
    miMsgBytes.clear();
    for(uint byteDex = 0; byteDex<byteCount; byteDex++) {
      byteStr = bytes[byteDex];
      value = byteStr.toUInt(&success, 16);
      if((! success) || (value > 0xff)) {
        QVwErrShow(tr("'%1' is not a valid hexadecimal MIDI byte").arg(byteStr));
        return;
      }
      miMsgBytes.append(static_cast<char>(static_cast<quint8>(value)));
    }


//========
  miMsgLen = miMsgBytes.count();
  for(uint i=0; i<miMsgLen; i++)
    miBytes[i] = ((uint)miMsgBytes[i] & 0x000000FFU); // That freq king QByteArray is signed and sign extends on coersion!
  theMidi->Parse(miMsgLen, miBytes);

//    MiMsgParse(miMsgBytes);  // Make sure the bytes represent a valid MIDI i_miMsgStr.
    if(valid)
      TS = theDrvIf->OnMiMsgTx(miMsgBytes);
    else
      TS = GetTS();
    theQVwMain->OnMiMsgTX(TS, theMidi, valid);
    usleep(10000);
  }
}
void    Ctrl::OnMiMsgRx        (quint64 i_TS, const QByteArray &i_msg) {
//========
  uint     miBytes[1024];
  uint miMsgLen = i_msg.count();
  for(uint i=0; i<miMsgLen; i++)
    miBytes[i] = ((uint)i_msg[i] & 0x000000FFU); // That freq king QByteArray is signed and sign extends on coersion!
  theMidi->Parse(miMsgLen, miBytes);
//    MiMsgParse(i_msg);
    theQVwMain->OnMiMsgRX(i_TS, theMidi, valid);
}

bool    Ctrl::OnTestCb(void *i_blob) {
  (void) i_blob;
  fprintf(stdout, "Callback\n"); fflush(stdout);
  return true;
}
