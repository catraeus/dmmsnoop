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

// Class definition

        Midi::Midi(QObject *parent) : QObject(parent) {
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
      case RtMidi::LINUX_ALSA : driverNames.append(tr("ALSA Seq"     )); break;
      case RtMidi::MACOSX_CORE: driverNames.append(tr("CoreMidi"     )); break;
      case RtMidi::UNIX_JACK  : driverNames.append(tr("JACK"         )); break;
      case RtMidi::WINDOWS_MM : driverNames.append(tr("Win MM MIDI"  )); break;
      default                 : sprintf(tStr, "Bad MIDI API enum: %d",static_cast<int>(theMidiAPI)) ;
          // Fallthrough on purpose, unknown, RTMIDI_DUMMY and UNSPECIFIED all get out of the for loop
      case RtMidi::RTMIDI_DUMMY:
      case RtMidi::UNSPECIFIED:
        continue;
    }
    driverAPIs.append(theMidiAPI);
  }

  driver = -1;
  modeIgnActSn = true;
  modeIgnSysEx = true;
  modeIgnMiTim = true;
  inputPort = -1;
  outputPort = -1;
}
        Midi::~Midi                (         )       {        setDriver(-1)                ; }

int     Midi::getDriver            (         ) const { return driver                       ; }
int     Midi::getDriverCount       (         ) const { return driverAPIs.count()           ; }
QString Midi::getDriverName        (int i_dex) const { return driverNames[i_dex]           ; }
bool    Midi::ModeIgnActSnGet      (         ) const { return modeIgnActSn ; }
bool    Midi::ModeIgnSysExGet      (         ) const { return modeIgnSysEx ; }
bool    Midi::ModeIgnMiTimGet      (         ) const { return modeIgnMiTim ; }
int     Midi::getInputPort         (         ) const { return inputPort                    ; }
int     Midi::getInputPortCount    (         ) const { return inputPortNames.count()       ; }
QString Midi::getInputPortName     (int i_dex) const { return inputPortNames[i_dex]        ; }
int     Midi::getOutputPort        (         ) const { return outputPort                   ; }
int     Midi::getOutputPortCount   (         ) const { return outputPortNames.count()      ; }
QString Midi::getOutputPortName    (int i_dex) const { return outputPortNames[i_dex]       ; }

void Midi::handleMidiInput(double timeStamp, std::vector<uint8_t> *message, void *engine) {
    static_cast<Midi *>(engine)->handleMidiInput(timeStamp, *message);
}
quint64 Midi::TimeGet() const {
  return QDateTime::currentDateTime().toMSecsSinceEpoch();
}
void Midi::handleMidiInput(double i_TS, const std::vector<uint8_t> &message) {
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
void Midi::removePorts() {
    setInputPort(-1);
    setOutputPort(-1);
    for(int i = inputPortNames.count() - 1; i >= 0; i--) {
        inputPortNames.removeAt(i);
        emit EmPortInpDel(i);
    }
    for(int i = outputPortNames.count() - 1; i >= 0; i--) {
        outputPortNames.removeAt(i);
        emit outputPortRemoved(i);
    }
}

quint64 Midi::OnMiMsgTx(const QByteArray &message) {
    assert(outputPort != -1);
    std::vector<uint8_t> msg;
    for(int i = 0; i < message.count(); i++) {
        msg.push_back(static_cast<uint8_t>(message[i]));
    }
        theRtMidiOut->sendMessage(&msg);
    return TimeGet();
}

void Midi::setDriver(int i_dex) {
    assert((i_dex >= -1) && (i_dex < driverAPIs.count()));
    if(driver != i_dex) {

        // Close the currently open MIDI driver.
        if(driver != -1) {
            removePorts();
            delete input;
            delete theRtMidiOut;
            driver = -1;
            emit EmDrvChange(-1);
        }

        // Open the new driver.
        if(i_dex != -1) {
            RtMidi::Api api = driverAPIs[i_dex];
                input = new RtMidiIn(api, "dmmsnoop");
                QScopedPointer<RtMidiIn> inputPtr(input);
                theRtMidiOut = new RtMidiOut(api, "dmmsnoop");
                QScopedPointer<RtMidiOut> outputPtr(theRtMidiOut);
                input->setCallback(handleMidiInput, this);

                // Add ports.
                    unsigned int count = input->getPortCount();
                    QString name;
                    for(unsigned int i = 0; i < count; i++) {
                        name = QString::fromStdString(input->getPortName(i));
                        inputPortNames.append(name);
                        emit EmPortInpAdd(i, name);
                    }
                    count = theRtMidiOut->getPortCount();
                    for(unsigned int i = 0; i < count; i++) {
                        name = QString::fromStdString(theRtMidiOut->getPortName(i));
                        outputPortNames.append(name);
                        emit outputPortAdded(i, name);
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
                        outputPortNames.append(name);
                        emit outputPortAdded(outputPortNames.count() - 1, name);
                        virtualPortsAdded = true;
                        break;
                    default:
                        virtualPortsAdded = false;
                    }
                inputPtr.take();
                outputPtr.take();
            driver = i_dex;
            emit EmDrvChange(i_dex);
        }
    }
}

void Midi::OnModeIgnActSnChg(bool i_ign) { if(modeIgnActSn != i_ign) { modeIgnActSn = i_ign; updateEventFilter(); emit EmModeIgnActSnChg(i_ign); }}
void Midi::OnModeIgnSysExChg(bool i_ign) { if(modeIgnSysEx != i_ign) { modeIgnSysEx = i_ign; updateEventFilter(); emit EmModeIgnSysExChg(i_ign); }}
void Midi::OnModeIgnMiTimChg(bool i_ign) { if(modeIgnMiTim != i_ign) { modeIgnMiTim = i_ign; updateEventFilter(); emit EmModeIgnMiTimChg(i_ign); }}
void Midi::setInputPort(int index) {
    assert((index >= -1) && (index < inputPortNames.count()));
    if(inputPort != index) {

        // Close the currently open input port.
        if(inputPort != -1) {
                input->closePort();
            inputPort = -1;
            emit EmPortInpChg(-1);
        }

        // Open the new input port.
        if(index != -1) {
                if(virtualPortsAdded &&
                    (index == (inputPortNames.count() - 1))) {
                    input->openVirtualPort("MIDI Input");
                } else {
                    input->openPort(index, "MIDI Input");
                }
            inputPort = index;
            updateEventFilter();
            emit EmPortInpChg(index);
        }
    }
}

void Midi::setOutputPort(int index) {
    assert((index >= -1) && (index < outputPortNames.count()));
    if(outputPort != index) {

        // Close the currently open output port.
        if(outputPort != -1) {
                theRtMidiOut->closePort();
            outputPort = -1;
            emit outputPortChanged(-1);
        }

        // Open the new output port.
        if(index != -1) {
                if(virtualPortsAdded &&
                    (index == (outputPortNames.count() - 1))) {
                    theRtMidiOut->openVirtualPort("MIDI Output");
                } else {
                    theRtMidiOut->openPort(index, "MIDI Output");
                }
            outputPort = index;
            emit outputPortChanged(index);
        }
    }
}

void Midi::updateEventFilter() {
    if(inputPort != -1) {
        input->ignoreTypes(modeIgnSysEx, modeIgnMiTim,
                           modeIgnActSn);
    }
}
