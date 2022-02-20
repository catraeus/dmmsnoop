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

#ifndef __DRV_IF_HPP_
#define __DRV_IF_HPP_

#include <stdint.h>

#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QObject>

#include "Midi.hpp"
#include "util/DmmTypes.hpp"
#include "util/TrMsg.hpp"

#include <rtmidi/RtMidi.h>

class DrvIf: public QObject {
    Q_OBJECT
  public:
  public:
    explicit      DrvIf(QObject *parent=0);
                 ~DrvIf(                 );

    void          SetMidi         (Midi *i_theMidi) {theMidi = i_theMidi;}; // IMPORTANT, This MUST be done ASAP after the constructor.
    bool          DrvReadyGet     (void     ) const { return (miDrvNo != -1) && (miPortOutNum != -1);};
    int           DrvNumGet       (         ) const { return miDrvNo                  ; }
    int           DrvCntGet       (         ) const { return miDrvApis.size()         ; }
    std::string   DrvNameGet      (int i_dex) const { return miDrvNames[i_dex]        ; }
    int           PortInpNoGet    (         ) const { return miPortInpNum             ; }
    int           PortInpCntGet   (         ) const { return miPortInpNames.size()    ; }
    std::string   PortInpNameGet  (int i_dex) const { return miPortInpNames[i_dex]    ; }
    int           PortOutNoGet    (         ) const { return miPortOutNum             ; }
    int           PortOutCntGet   (         ) const { return miPortOutNames.size()    ; }
    std::string   PortOutNameGet  (int i_dex) const { return miPortOutNames[i_dex]    ; }
    bool          ModeIgnActSnGet (         ) const { return modeIgnActSn             ; }
    bool          ModeIgnSysExGet (         ) const { return modeIgnSysEx             ; }
    bool          ModeIgnMiTimGet (         ) const { return modeIgnMiTim             ; }

public slots:
  void      OnDrvChg            (int     i_dex);
  void      OnModeIgnActSnChg   (bool    i_ign);
  void      OnModeIgnSysExChg   (bool    i_ign);
  void      OnModeIgnMiTimChg   (bool    i_ign);
  void      OnPortInpChg        (int     i_dex);
  void      OnPortOutChg        (int     i_dex);
  quint64   OnMiMsgTx           (const   QByteArray &i_qbMsg);

signals:
  void      EmDrvChange         (int     i_dex);
  void      EmPortInpAdd        (int     i_dex, const QString &name);
  void      EmPortInpChg        (int     i_dex);
  void      EmPortInpDel        (int     i_dex);
  void      EmPortOutAdd        (int     i_dex, const QString &name);
  void      EmPortOutChg        (int     i_dex);
  void      EmPortOutDel        (int     i_dex);
  void      EmModeIgnActSnChg   (bool    i_ign);
  void      EmModeIgnSysExChg   (bool    i_ign);
  void      EmModeIgnMiTimChg   (bool    i_ign);
  void      EmMiMsgRx           (quint64 i_TS,  const QByteArray &i_qbMsg);

public:
private:
  static void    DoMiMsgRxPrep(double i_TS,       std::vector<uint8_t> *i_vbMsg, void *i_aDrvIf);
         void    DoMiMsgRxPrep(double i_TS, const std::vector<uint8_t> &i_vbMsg);
         void    DoPortAllDel();
         void    DoModeIgnChg();

    TrMsg               *theTrMsg;
    Midi                *theMidi;

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
    char                *miMsgVelo_str;    // iff is 0x8x 0x9x 0xA0 0xB0 0xD0
    int                  miMsgChChNo;     // iff is 0xA0
    char                *miMsgChChStr;    // iff is 0xA0

    int                        miDrvNo;
    std::vector<RtMidi::Api>   miDrvApis;
    std::vector<std::string>   miDrvNames;

    bool                 modeIgnActSn;
    bool                 modeIgnSysEx;
    bool                 modeIgnMiTim;
    RtMidiIn            *miPortInpInst;
    int                  miPortInpNum;
    std::vector<std::string>   miPortInpNames;
    RtMidiOut           *miPortOutInst;
    int                  miPortOutNum;
    std::vector<std::string>   miPortOutNames;
    bool                 hasPortsVirtual;
};

#endif
