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
         quint64 TimeGet     () const;
private:
  static void    DoMiMsgRx(double timeStamp,       std::vector<uint8_t> *message, void *engine);
         void    DoMiMsgRx(double timeStamp, const std::vector<uint8_t> &message);
         void    DoPortAllDel();
         void    DoModeIgnChg();

    TrMsg               *theTrMsg;

    uint8_t              miBuffer[256];
    uint                 miBuffSize;
    uint                 miBuf[256];

    int                  miMsgStatMajNo;  // Basic Commands  IMPORTANT This is LSB removed and right-shifted by 8
    char                *miMsgStatMajStr; // Basic Commands  IMPORTANT This is back up to 80, 90 etc.
    int                  miMsgStatMinNo;  // Channel for lower numbers, System Command above 0xF0
    char                *miMsgStatMinStr; // Channel for lower numbers, System Command above 0xF0
    int                  miMsgNoteNo;     // iff is 0x8x 0x9x 0xA0
    char                *miMsgNoteStr;    // iff is 0x8x 0x9x 0xA0
    int                  miMsgVeloNo;     // iff is 0x8x 0x9x 0xA0 0xB0 0xD0
    char                *miMsgVeloStr;    // iff is 0x8x 0x9x 0xA0 0xB0 0xD0
    int                  miMsgChChNo;     // iff is 0xA0
    char                *miMsgChChStr;    // iff is 0xA0

    int                  miDrvNo;
    QList<RtMidi::Api>   miDrvApis;
    QStringList          miDrvNames;

    bool                 modeIgnActSn;
    bool                 modeIgnSysEx;
    bool                 modeIgnMiTim;
    RtMidiIn            *miPortInpInst;
    int                  miPortInpNum;
    QStringList          miPortInpNames;
    RtMidiOut           *miPortOutInst;
    int                  miPortOutNum;
    QStringList          miPortOutNames;
    bool                 hasPortsVirtual;
};

#endif
