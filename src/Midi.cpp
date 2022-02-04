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
#include <string.h>
#include <QtCore/QDebug>

#include "Midi.hpp"
#include "error.h"

#if QT_VERSION >= 0x040700
#include <QtCore/QDateTime>
#else
#include <cerrno>
#include <cstring>
#include <sys/time.h>
#endif

Midi:: Midi(QObject *parent) : QObject(parent) {
              RtMidi::Api    theMidiAPI;
              char           tStr[256];
  std::vector<RtMidi::Api>   midiAPIlist;
              int            numAPIs;

  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
//Get available MIDI drivers.
  RtMidi::getCompiledApi(midiAPIlist);
  numAPIs = midiAPIlist.size();
  theMidiAPI = midiAPIlist[0];
  fprintf(stdout, "Setting up MIDI APIs with RtMidi.\n"); fflush(stdout);
  fprintf(stdout, "RtMidi says we have %d of them available.\n", numAPIs); fflush(stdout);

  for(int i=0; i<numAPIs; i++) {
    theMidiAPI = midiAPIlist[i];
    switch (theMidiAPI) {
      case RtMidi::LINUX_ALSA : miDrvNames.append(tr("ALSA Seq"     )); break;
      case RtMidi::MACOSX_CORE: miDrvNames.append(tr("CoreMidi"     )); break;
      case RtMidi::UNIX_JACK  : miDrvNames.append(tr("JACK"         )); break;
      case RtMidi::WINDOWS_MM : miDrvNames.append(tr("Win MM MIDI"  )); break;
      default                 : sprintf(tStr, "Bad MIDI API enum: %d",static_cast<int>(theMidiAPI)) ;
          // Fallthrough on purpose, unknown, RTMIDI_DUMMY and UNSPECIFIED all get out of the for loop
      case RtMidi::RTMIDI_DUMMY:
      case RtMidi::UNSPECIFIED:
        continue;
    }
    miDrvApis.append(theMidiAPI);
  }

  miDrvNo = -1;
  modeIgnActSn = true;
  modeIgnSysEx = true;
  modeIgnMiTim = true;
  miPortInpNum = -1;
  miPortOutNum = -1;
}
Midi::~Midi(               )                   {
  OnDrvChg(-1);
}

int     Midi::MiDrvNumGet          (         ) const { return miDrvNo                  ; }
int     Midi::getDriverCount       (         ) const { return miDrvApis.count()        ; }
QString Midi::getDriverName        (int i_dex) const { return miDrvNames[i_dex]        ; }
bool    Midi::ModeIgnActSnGet      (         ) const { return modeIgnActSn             ; }
bool    Midi::ModeIgnSysExGet      (         ) const { return modeIgnSysEx             ; }
bool    Midi::ModeIgnMiTimGet      (         ) const { return modeIgnMiTim             ; }
int     Midi::getInputPort         (         ) const { return miPortInpNum             ; }
int     Midi::getInputPortCount    (         ) const { return inputPortNames.count()   ; }
QString Midi::getInputPortName     (int i_dex) const { return inputPortNames[i_dex]    ; }
int     Midi::getOutputPort        (         ) const { return miPortOutNum             ; }
int     Midi::getOutputPortCount   (         ) const { return miPortOutNames.count()   ; }
QString Midi::getOutputPortName    (int i_dex) const { return miPortOutNames[i_dex]    ; }

void Midi::DoMiMsgRx(double i_TS, std::vector<uint8_t> *i_msg, void *i_aMidi) {
  Midi *aMidi;

  aMidi = (Midi *)i_aMidi;
  aMidi->DoMiMsgRx(i_TS, *i_msg);
}
quint64 Midi::TimeGet         () const {
  return QDateTime::currentDateTime().toMSecsSinceEpoch();
}
void    Midi::DoMiMsgRx       (double i_TS, const std::vector<uint8_t> &message) {
  QByteArray msg;
  quint64    TS;
  int        msgSize;
  (void)i_TS;

  switch (message[0]) {
    case 0xF0: if(modeIgnSysEx) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // SysEx
    case 0xF1: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // TimeCode Qtr Frm
    case 0xF2: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Song Position Pointer
    case 0xF3: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Song Select
    case 0xF4:                  { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_UNDEF  ); return; } break;  // Undefined
    case 0xF5:                  { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_UNDEF  ); return; } break;  // Undefined
    case 0xF6: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Tune Request
    case 0xF7: if(modeIgnSysEx) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // SysEx End
    case 0xF8: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Time Clock
    case 0xF9:                  { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_UNDEF  ); return; } break;  // Undefined
    case 0xFA: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Song Start
    case 0xFB: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Song Continue
    case 0xFC: if(modeIgnMiTim) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Song Stop
    case 0xFD:                  { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_UNDEF  ); return; } break;  // Undefined
    case 0xFE: if(modeIgnActSn) { qWarning() << theTrMsg->MsgMiMetaGet(TrMsg::DEM_RTM_FLT_FAIL); return; } break;  // Active Sense
    case 0xFF:                                                                                             break;  // Reset
  }
  TS      = TimeGet();
  msgSize = static_cast<int>(message.size());
  for(int i=0; i<msgSize; i++) msg.append(message[i]);
  emit EmMiMsgRx(TS, msg);
}
void    Midi::DoPortAllDel    () {
  OnPortInpChg(-1);
  OnPortOutChg(-1);
  for(int i = inputPortNames.count() - 1; i >= 0; i--) {
    inputPortNames.removeAt(i);
    emit EmPortInpDel(i);
  }
  for(int i = miPortOutNames.count() - 1; i >= 0; i--) {
    miPortOutNames.removeAt(i);
    emit EmPortOutDel(i);
  }
}
quint64 Midi::OnMiMsgTx       (const QByteArray &message) {
  assert(miPortOutNum != -1);
  std::vector<uint8_t> msg;
  for(int i = 0; i < message.count(); i++)
    msg.push_back(static_cast<uint8_t>(message[i]));
  miPortOutInst->sendMessage(&msg);
  return TimeGet();
}

void Midi::OnDrvChg    (int i_dex) {
  RtMidi::Api api;
  uint        count;
  QString name;

  if(miDrvNo != i_dex) { // Then we aren't doing nothing new.
    if(miDrvNo != -1) {// Close the currently open MIDI driver.
      DoPortAllDel();
      delete miPortInpInst;
      delete miPortOutInst;
      miDrvNo = -1;
      emit EmDrvChange(-1);
    }
    if(i_dex != -1) {  // Open the new driver.
      api = miDrvApis[i_dex];
      miPortInpInst = new RtMidiIn (api, "dmmsnoop");      QScopedPointer<RtMidiIn> inputPtr(miPortInpInst);
      miPortOutInst = new RtMidiOut(api, "dmmsnoop");      QScopedPointer<RtMidiOut> outputPtr(miPortOutInst);
      miPortInpInst->setCallback(DoMiMsgRx, this);
      // Add ports.
      count = miPortInpInst->getPortCount();
      for(uint i = 0; i < count; i++) {
        name = QString::fromStdString(miPortInpInst->getPortName(i));
        inputPortNames.append(name);
        emit EmPortInpAdd(i, name);
      }
      count = miPortOutInst->getPortCount();
      for(uint i = 0; i < count; i++) {
        name = QString::fromStdString(miPortOutInst->getPortName(i));
        miPortOutNames.append(name);
        emit EmPortOutAdd(i, name);
      }
      // Add a virtual port to drivers that support virtual ports.
      switch (api) {
        case RtMidi::LINUX_ALSA:
        case RtMidi::MACOSX_CORE:
        case RtMidi::UNIX_JACK:
          name = tr("[virtual input]");
          inputPortNames.append(name);
          emit EmPortInpAdd(inputPortNames.count() - 1, name);
          name = tr("[virtual output]");
          miPortOutNames.append(name);
          emit EmPortOutAdd(miPortOutNames.count() - 1, name);
          hasPortsVirtual = true;
          break;
        default:
          hasPortsVirtual = false;
      }
      inputPtr.take();
      outputPtr.take();
      miDrvNo = i_dex;
      emit EmDrvChange(i_dex);
    } // dex != -1
  } // miDrvNo != i_dex
}
void Midi::OnPortInpChg(int i_dex) {

  if(miPortInpNum != i_dex) {
    if(miPortInpNum != -1) {  // Close the currently open input port.
      miPortInpInst->closePort();
      miPortInpNum = -1;
      emit EmPortInpChg(-1);
    }
    if(i_dex != -1) {   // Open the new input port.
      if(hasPortsVirtual && (i_dex == (inputPortNames.count() - 1)))     miPortInpInst->openVirtualPort("MIDI Input");
      else                                                               miPortInpInst->openPort(i_dex, "MIDI Input");
      miPortInpNum = i_dex;
      DoModeIgnChg();
      emit EmPortInpChg(i_dex);
    }
  }
}
void Midi::OnPortOutChg(int i_dex) {
  if(miPortOutNum != i_dex) {
    if(miPortOutNum != -1) {  // Close the currently open output port.
      miPortOutInst->closePort();
      miPortOutNum = -1;
      emit EmPortOutChg(-1);
    }
    if(i_dex != -1) {  // Open the new output port.
      if(hasPortsVirtual && (i_dex == (miPortOutNames.count() - 1)))  miPortOutInst->openVirtualPort("MIDI Output");
      else                                                            miPortOutInst->openPort(i_dex, "MIDI Output");
      miPortOutNum = i_dex;
      emit EmPortOutChg(i_dex);
    }
  }
}
void Midi::OnModeIgnActSnChg(bool i_ign) { if(modeIgnActSn != i_ign) { modeIgnActSn = i_ign; DoModeIgnChg(); emit EmModeIgnActSnChg(i_ign); }}
void Midi::OnModeIgnSysExChg(bool i_ign) { if(modeIgnSysEx != i_ign) { modeIgnSysEx = i_ign; DoModeIgnChg(); emit EmModeIgnSysExChg(i_ign); }}
void Midi::OnModeIgnMiTimChg(bool i_ign) { if(modeIgnMiTim != i_ign) { modeIgnMiTim = i_ign; DoModeIgnChg(); emit EmModeIgnMiTimChg(i_ign); }}
void Midi::DoModeIgnChg     (          ) { if(miPortInpNum != -1   ) { miPortInpInst->ignoreTypes(modeIgnSysEx, modeIgnMiTim, modeIgnActSn);}}
