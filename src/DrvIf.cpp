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

#include <string.h>
#include <QtCore/QDebug>
#include <QtCore/QMetaType>

#include <cerrno>
#include <cstring>
#include <sys/time.h>

#include "DrvIf.hpp"
#include "util/DmmStr.hpp"
#include "util/Error.hpp"


DrvIf:: DrvIf(QObject *parent) : QObject(parent) {
              RtMidi::Api    theMidiAPI;
              char           tStr[256];
  std::vector<RtMidi::Api>   midiAPIlist;
              int            numAPIs;

  theMidi = NULL;
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
      case RtMidi::LINUX_ALSA : miDrvNames.push_back("ALSA Seq"     ); break;
      case RtMidi::MACOSX_CORE: miDrvNames.push_back("CoreMidi"     ); break;
      case RtMidi::UNIX_JACK  : miDrvNames.push_back("JACK"         ); break;
      case RtMidi::WINDOWS_MM : miDrvNames.push_back("Win MM MIDI"  ); break;
      default                 : sprintf(tStr, "Bad MIDI API enum: %d",static_cast<int>(theMidiAPI)) ;
          // Fallthrough on purpose, unknown, RTMIDI_DUMMY and UNSPECIFIED all get out of the for loop
      case RtMidi::RTMIDI_DUMMY:
      case RtMidi::UNSPECIFIED:
        continue;
    }
    miDrvApis.push_back(theMidiAPI);
  }

  miBuffSize = 0;

  miDrvNo = -1;
  modeIgnActSn = false;
  modeIgnSysEx = false;
  modeIgnMiTim = false;
  miPortInpNum = -1;
  miPortOutNum = -1;
}
DrvIf::~DrvIf(               )                   {
  OnDrvChg(-1);
}


void    DrvIf::DoMiMsgRxPrep       (double i_TS,       std::vector<uint8_t> *i_vbMsg, void *i_aDrvIf) {
  DrvIf *aDrvIf;

  aDrvIf = (DrvIf *)i_aDrvIf;
  aDrvIf->DoMiMsgRxPrep(i_TS, *i_vbMsg);
}
void    DrvIf::DoMiMsgRxPrep       (double i_TS, const std::vector<uint8_t> &i_vbMsg) {
  QByteArray msg;
  quint64     TS;
  int        msgSize;
  (void)i_TS;

  miBuffSize = (uint)(i_vbMsg.size());
  for(uint i=0; i<miBuffSize; i++)
    miBuffer[i] = i_vbMsg[i];
  if(miBuffSize == 0)
    fprintf(stdout, "OOPS - Message was zero length\n");
  miMsgStatMajNo  = miBuffer[0];
  miMsgStatMajNo >>= 4;
  miMsgStatMajNo  &= 0x00000007U;
  miMsgStatMajStr = theTrMsg->MsgMiStatGet((TrMsg::eStatType)miMsgStatMajNo);

  TS      = GetTS();
  msgSize = static_cast<int>(i_vbMsg.size());
  for(int i=0; i<msgSize; i++) msg.append(i_vbMsg[i]);
  emit EmMiMsgRx(TS, msg);
}
void    DrvIf::DoPortAllDel    () {
  OnPortInpChg(-1);
  OnPortOutChg(-1);
  for(int i = miPortInpNames.size() - 1; i >= 0; i--) {
    miPortInpNames.erase(miPortInpNames.begin() + i);
    emit EmPortInpDel(i);
  }
  for(int i = miPortOutNames.size() - 1; i >= 0; i--) {
    miPortOutNames.erase(miPortOutNames.begin() + i);
    emit EmPortOutDel(i);
  }
}
quint64 DrvIf::OnMiMsgTx       (const QByteArray &i_qbMsg) {
  assert(miPortOutNum != -1);
  std::vector<uint8_t> msg;
  for(int i = 0; i < i_qbMsg.count(); i++)
    msg.push_back(static_cast<uint8_t>(i_qbMsg[i]));
  miPortOutInst->sendMessage(&msg);
  return GetTS();
}

void DrvIf::OnDrvChg    (int i_dex) {
  RtMidi::Api api;
  uint        count;
  std::string name;

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
      miPortInpInst->setCallback(DoMiMsgRxPrep, this);
      // Add ports.
      count = miPortInpInst->getPortCount();
      for(uint i = 0; i < count; i++) {
        name = miPortInpInst->getPortName(i);
        miPortInpNames.push_back(name);
        emit EmPortInpAdd(i, QString::fromStdString(name));
      }
      count = miPortOutInst->getPortCount();
      for(uint i = 0; i < count; i++) {
        name = miPortOutInst->getPortName(i);
        miPortOutNames.push_back(name);
        emit EmPortOutAdd(i, QString::fromStdString(name));
      }
      // Add a virtual port to drivers that support virtual ports.
      switch (api) {
        case RtMidi::LINUX_ALSA:
        case RtMidi::MACOSX_CORE:
        case RtMidi::UNIX_JACK:
          name = "[virtual input]";
          miPortInpNames.push_back(name);
          emit EmPortInpAdd(miPortInpNames.size() - 1, QString::fromStdString(name));
          name = "[virtual output]";
          miPortOutNames.push_back(name);
          emit EmPortOutAdd(miPortOutNames.size() - 1, QString::fromStdString(name));
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
void DrvIf::OnPortInpChg(int i_dex) {

  if(miPortInpNum != i_dex) {
    if(miPortInpNum != -1) {  // Close the currently open input port.
      miPortInpInst->closePort();
      miPortInpNum = -1;
      emit EmPortInpChg(-1);
    }
    if(i_dex != -1) {   // Open the new input port.
      if(hasPortsVirtual && (i_dex == ((int)(miPortInpNames.size()) - 1)))     miPortInpInst->openVirtualPort("MIDI Input");
      else                                                                     miPortInpInst->openPort(i_dex, "MIDI Input");
      miPortInpNum = i_dex;
      DoModeIgnChg();
      emit EmPortInpChg(i_dex);
      miPortInpInst->ignoreTypes(modeIgnSysEx, modeIgnMiTim, modeIgnActSn);
    }
  }
}
void DrvIf::OnPortOutChg(int i_dex) {
  if(miPortOutNum != i_dex) {
    if(miPortOutNum != -1) {  // Close the currently open output port.
      miPortOutInst->closePort();
      miPortOutNum = -1;
      emit EmPortOutChg(-1);
    }
    if(i_dex != -1) {  // Open the new output port.
      if(hasPortsVirtual && (i_dex == ((int)(miPortOutNames.size()) - 1)))  miPortOutInst->openVirtualPort("MIDI Output");
      else                                                                  miPortOutInst->openPort(i_dex, "MIDI Output");
      miPortOutNum = i_dex;
      emit EmPortOutChg(i_dex);
      miPortInpInst->ignoreTypes(modeIgnSysEx, modeIgnMiTim, modeIgnActSn);
    }
  }
}
void DrvIf::OnModeIgnActSnChg(bool i_ign) {
  modeIgnActSn = i_ign;
  DoModeIgnChg();
}
void DrvIf::OnModeIgnSysExChg(bool i_ign) {
  modeIgnSysEx = i_ign;
  DoModeIgnChg();
}
void DrvIf::OnModeIgnMiTimChg(bool i_ign) {
  modeIgnMiTim = i_ign;
  DoModeIgnChg();
}
void DrvIf::DoModeIgnChg     (void      ) {
  if(miPortInpNum != -1   )
    miPortInpInst->ignoreTypes(modeIgnSysEx, modeIgnMiTim, modeIgnActSn);
}
