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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <stdint.h>

#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QObject>

#include "util/TrMsg.hpp"

#include <rtmidi/RtMidi.h>

class Midi: public QObject {
    Q_OBJECT

public:
  explicit  Midi(QObject *parent=0);
           ~Midi();
  int       MiDrvNumGet() const;
  int       getDriverCount() const;
  QString   getDriverName(int index) const;
  bool      ModeIgnActSnGet() const;
  bool      ModeIgnSysExGet() const;
  bool      ModeIgnMiTimGet() const;
  int       getInputPort() const;
  int       getInputPortCount() const;
  QString   getInputPortName(int index) const;
  int       getOutputPort() const;
  int       getOutputPortCount() const;
  QString   getOutputPortName(int index) const;

public slots:
  void      OnDrvChg(int index);
  void      OnModeIgnActSnChg(bool ignore);
  void      OnModeIgnSysExChg(bool ignore);
  void      OnModeIgnMiTimChg(bool ignore);
  void      OnPortInpChg(int index);
  void      OnPortOutChg(int index);
  quint64   OnMiMsgTx(const QByteArray &message);

signals:
  void      EmDrvChange(int index);
  void      EmModeIgnActSnChg(bool ignore);
  void      EmModeIgnSysExChg(bool ignore);
  void      EmModeIgnMiTimChg(bool ignore);
  void      EmPortInpAdd(int index, const QString &name);
  void      EmPortInpChg(int index);
  void      EmPortInpDel(int index);
  void      EmPortOutAdd(int index, const QString &name);
  void      EmPortOutChg(int index);
  void      EmPortOutDel(int index);
  void      EmMiMsgRx(quint64 timeStamp, const QByteArray &message);

public:
         quint64 TimeGet() const;
private:
  static void    DoMiMsgInpHandle(double timeStamp,       std::vector<uint8_t> *message, void *engine);
         void    DoMiMsgInpHandle(double timeStamp, const std::vector<uint8_t> &message);
         void    DoPortAllDel();
         void    DoModeIgnChg();

    TrMsg               *theTrMsg;
    int                  miDrvNo;
    QList<RtMidi::Api>   miDrvApis;
    QStringList          miDrvNames;
    bool                 modeIgnActSn;
    bool                 modeIgnSysEx;
    bool                 modeIgnMiTim;
    RtMidiIn            *miPortInpInst;
    int                  miPortInpNum;
    QStringList          inputPortNames;
    RtMidiOut           *miPortOutInst;
    int                  miPortOutNum;
    QStringList          miPortOutNames;
    bool                 hasPortsVirtual;
};

#endif
