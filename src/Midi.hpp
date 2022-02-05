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

#include "util/DmmTypes.hpp"
#include "util/TrMsg.hpp"

#include <rtmidi/RtMidi.h>

class Midi: public QObject {
    Q_OBJECT
public:
  struct sMsgSpec { // all are null-terminated ASCII c strings.  Whatever the calc is, I'll take it to a next 2^k
    char TS   [16] ; // hh:mm:ss.sss or ssssss.sss (one day is 86400 sec so I figure 999999.999 sec should cover most sessions.)
    char len  [ 4] ; // If we get a SysEx infinitely long blob, I'll just truncate and fake it.
    char raw  [16] ; // ALL normal MIDI messages are 1, 2 or 3 bytes.  This will put out nn nnn nnn as hex dec dec
    char stat [32] ; // If I can't make a reasonable human message in 32 characters, then something is wrong.
    char ch   [ 4] ; // There are at most 16 channels possible printed as decimal
    char vel  [ 4] ; // for notes, controls and pressures.  These are 0 to 127 and will be printed decimal.
    char cc   [ 4] ; // There are only 128 channels possible.  Print decimal.  FIXME But, there are LSB/MSB channel pairs to figure.
    char prog [ 4] ; // 128 programs possible.
    char bend [ 4] ; // Is a Speshul uint16_t
    char sys  [32] ; // Same sentiment as stat above
  };
public:
  explicit  Midi(QObject *parent=0);
           ~Midi();
  int       MiDrvNumGet       () const;
  int       getDriverCount    () const;
  QString   getDriverName     (int i_dex) const;
  bool      ModeIgnActSnGet   () const;
  bool      ModeIgnSysExGet   () const;
  bool      ModeIgnMiTimGet   () const;
  int       getInputPort      () const;
  int       getInputPortCount () const;
  QString   getInputPortName  (int i_dex) const;
  int       getOutputPort     (           ) const;
  int       getOutputPortCount() const;
  QString   getOutputPortName (int i_dex) const;

public slots:
  void      OnDrvChg          (int i_dex);
  void      OnModeIgnActSnChg (bool i_ign);
  void      OnModeIgnSysExChg (bool i_ign);
  void      OnModeIgnMiTimChg (bool i_ign);
  void      OnPortInpChg      (int i_dex);
  void      OnPortOutChg      (int i_dex);
  quint64   OnMiMsgTx         (const QByteArray &i_qbMsg);

signals:
  void      EmDrvChange(int i_dex);
  void      EmModeIgnActSnChg(bool i_ign);
  void      EmModeIgnSysExChg(bool i_ign);
  void      EmModeIgnMiTimChg(bool i_ign);
  void      EmPortInpAdd(int i_dex, const QString &name);
  void      EmPortInpChg(int i_dex);
  void      EmPortInpDel(int i_dex);
  void      EmPortOutAdd(int i_dex, const QString &name);
  void      EmPortOutChg(int i_dex);
  void      EmPortOutDel(int i_dex);
  void      EmMiMsgRx(quint64 i_TS, const QByteArray &i_qbMsg);

public:
private:
  static void    DoMiMsgRx(double i_TS,       std::vector<uint8_t> *i_vbMsg, void *engine);
         void    DoMiMsgRx(double i_TS, const std::vector<uint8_t> &i_vbMsg);
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
