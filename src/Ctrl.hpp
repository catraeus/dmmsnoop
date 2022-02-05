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

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "App.hpp"
#include "Midi.hpp"

#include "QVwMain.hpp"
#include "QVwConfig.hpp"
#include "QVwMsg.hpp"
#include "QVwAbout.hpp"
#include "QVwErr.hpp"

#include "util/TrMsg.hpp"

class Ctrl: public QObject {
  Q_OBJECT

public:
  explicit  Ctrl(App &i_theApp, QObject *parent=0);
           ~Ctrl();
  void      run();  // RESEARCH Might be Inherited via QThread::run

private slots:
  void      OnMidiDrvChg();
  void      OnMiMsgTx(const QString &message);
  void      OnMiMsgRx(quint64 timeStamp, const QByteArray &message);

private:
  void      MiMsgDatBytesStr(const QByteArray &message, char *o_dStr, int lastIndex=-1);
  void      MiMsgParse(const QByteArray &i_msg);
  void      QVwErrShow(const QString &message);

  App           &theApp;

  Midi           theMidi;

  QVwMain        theQVwMain;
  QVwConfig      theQVwConfig;
  QVwAbout       theQVwAbout;
  QVwMsg         theQVwMsg;
  QVwErr         theQVwErr;

  int            miMsgLen;
  uint8_t        miStat;
  uint           miStatBase;
  QString        strMiData;
  QString        strMiStat;

  TrMsg         *theTrMsg;

  bool           valid;

static       int lenMidiSpecAry[];

};

#endif
