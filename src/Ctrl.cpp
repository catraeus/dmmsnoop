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
#include <stdio.h>

#include <QtCore/QDebug>

#include "Ctrl.hpp"
#include "util/Error.hpp"
#include "util/util.hpp"

// MAGICK  0xF0   0 -  SysEx with infinite length terminated by 0xF7
// MAGICK  0xF4  -1 -  Undefined
// MAGICK  0xF5  -1 -  Undefined
// MAGICK  0xF7  -1 -  End of SysEx
// MAGICK  0xFD  -1 -  Undefined
// IMPORTANT The "lengths" below also have meta-information about validity.  0 and -1 meaning special handling.
// IMPORTANT The length (that are truly length) are the length including the Midi Status first byte.
// TODO.  Make a first nibble mask to see what to do with the second nibble and lengths.
    int Ctrl::lenMidiSpecAry[] = {
/*       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/*0x80*/ 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // Note Off
/*0x90*/ 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // Note On
/*0xA0*/ 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // Polyphonic Key Pressure (Aftertouch, super rare)
/*0xB0*/ 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // Control Change (CC), knobs are being turned, spun, slid, pushed, banged, whatever
/*0xC0*/ 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // Program Change - new patches being spec'd
/*0xD0*/ 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // Channel Pressure - whole voice aftertouch, most often used.
/*0xE0*/ 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // Pitch Bend - So special it has its own command, and is a (close to) uint16_t range.
/*0xF0*/ 0,  2,  3,  2, -1, -1,  1, -1,  1,  1,  1,  1,  1, -1,  1,  1  // Aren't we Speecial?  The SysEx and beyond controls
};

// Class definition

        Ctrl::Ctrl(App &i_theApp, QObject *i_parent) : QObject(i_parent), theApp(i_theApp) {

//==================================================================================================
//==== Gain access to a message string machine
  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);

//==================================================================================================
//==== Setup About
    theQVwAbout.setMajorVersion(DMMSNOOP_MAJOR_VERSION);
    theQVwAbout.setMinorVersion(DMMSNOOP_MINOR_VERSION);
    theQVwAbout.setRevision(DMMSNOOP_REVISION);
    connect(&theQVwAbout, SIGNAL(closeRequest()), &theQVwAbout, SLOT(hide()));

//==================================================================================================
//==== Setup Config
    int driverCount = theMidi.getDriverCount();
    if(! driverCount) {
        throw Error(tr("no MIDI drivers found"));
    }
    for(int i = 0; i < driverCount; i++) {
        theQVwConfig.addDriver(i, theMidi.getDriverName(i));
    }
    int driver = theMidi.getDriver();
    int outputPort = theMidi.getOutputPort();
    theQVwConfig.setDriver                        (driver);
    theQVwConfig.setInputPort(theMidi.getInputPort ());
    theQVwConfig.setIgnoreActiveSensingEvents     (theMidi.getIgnoreActiveSensingEvents());
    theQVwConfig.setIgnoreSystemExclusiveEvents   (theMidi.getIgnoreSystemExclusiveEvents());
    theQVwConfig.setIgnoreTimeEvents              (theMidi.getIgnoreTimeEvents());
    theQVwConfig.setOutputPort                    (outputPort);
    connect(&theQVwConfig, SIGNAL(closeRequest()),                                   &theQVwConfig, SLOT(hide()));
    connect(&theQVwConfig, SIGNAL(driverChangeRequest(int)),                         &theMidi, SLOT(setDriver(int)));
    connect(&theQVwConfig, SIGNAL(ignoreActiveSensingEventsChangeRequest(bool)),     &theMidi, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&theQVwConfig, SIGNAL(ignoreSystemExclusiveEventsChangeRequest(bool)),   &theMidi, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&theQVwConfig, SIGNAL(ignoreTimeEventsChangeRequest(bool)),              &theMidi, SLOT(setIgnoreTimeEvents(bool)));
    connect(&theQVwConfig, SIGNAL(inputPortChangeRequest(int)),                      &theMidi, SLOT(setInputPort(int)));
    connect(&theQVwConfig, SIGNAL(outputPortChangeRequest(int)),                     &theMidi, SLOT(setOutputPort(int)));

//==================================================================================================
//==== Setup Error
    connect(&theQVwErr, SIGNAL(closeRequest()),                              &theQVwErr,     SLOT(hide()));

//==================================================================================================
//==== Setup Main
    theQVwMain.setMessageSendEnabled((driver != -1) && (outputPort != -1));
    theQVwMain.SetTimeZero(theMidi.TimeGet());
    connect(&theQVwMain, SIGNAL(aboutRequest()),                               &theQVwAbout,     SLOT(show()));
    connect(&theQVwMain, SIGNAL(addMessageRequest()),                          &theQVwMsg,   SLOT(show()));
    connect(&theQVwMain, SIGNAL(clearMessagesRequest()),                       &theQVwMain,      SLOT(clearMessages()));
    connect(&theQVwMain, SIGNAL(configureRequest()),                           &theQVwConfig, SLOT(show()));
    connect(&theQVwMain, SIGNAL(closeRequest()),                               &theApp,   SLOT(quit()));

//==================================================================================================
//==== Setup Message Send
    connect(&theQVwMsg, SIGNAL(closeRequest()),                            &theQVwMsg,   SLOT(hide()));
    connect(&theQVwMsg, SIGNAL(sendRequest(const QString &)),              &theQVwMsg,   SLOT(hide()));
    connect(&theQVwMsg, SIGNAL(sendRequest(const QString &)),                              SLOT(OnMiMsgTx(const QString &)));

//==================================================================================================
//==== Setup Midi worker  FIXME, there are multiple "overloads and multiple inheritances" of these.  Does order matter ! ? ! ?
    connect(&theMidi, SIGNAL(EmMiMsgRx(quint64, const QByteArray &)),          this,         SLOT(OnMiMsgRx(quint64, const QByteArray &)));
    connect(&theMidi, SIGNAL(EmDrvChange(int)),                               &theQVwConfig, SLOT(setDriver(int)));
    connect(&theMidi, SIGNAL(EmDrvChange(int)),                                this,         SLOT(OnDrvChange()));
    connect(&theMidi, SIGNAL(ignoreActiveSensingEventsChanged(bool)),         &theQVwConfig, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&theMidi, SIGNAL(ignoreSystemExclusiveEventsChanged(bool)),       &theQVwConfig, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&theMidi, SIGNAL(ignoreTimeEventsChanged(bool)),                  &theQVwConfig, SLOT(setIgnoreTimeEvents(bool)));
    connect(&theMidi, SIGNAL(inputPortAdded(int, QString)),                   &theQVwConfig, SLOT(addInputPort(int, QString)));
    connect(&theMidi, SIGNAL(inputPortChanged(int)),                          &theQVwConfig, SLOT(setInputPort(int)));
    connect(&theMidi, SIGNAL(inputPortChanged(int)),                           this,         SLOT(OnDrvChange()));
    connect(&theMidi, SIGNAL(inputPortRemoved(int)),                          &theQVwConfig, SLOT(removeInputPort(int)));
    connect(&theMidi, SIGNAL(outputPortAdded(int, QString)),                  &theQVwConfig, SLOT(addOutputPort(int, QString)));
    connect(&theMidi, SIGNAL(outputPortChanged(int)),                         &theQVwConfig, SLOT(setOutputPort(int)));
    connect(&theMidi, SIGNAL(outputPortChanged(int)),                          this,         SLOT(OnDrvChange()));
    connect(&theMidi, SIGNAL(outputPortRemoved(int)),                         &theQVwConfig, SLOT(removeOutputPort(int)));

//==================================================================================================
//==== Setup theApp
    connect(&theApp, SIGNAL(eventError(QString)),                &theQVwErr, SLOT(setMessage(QString)));
    connect(&theApp, SIGNAL(eventError(QString)),                &theQVwErr, SLOT(show()));
}
        Ctrl::~Ctrl() {
// Disconnect theMidi signals handled by the controller before the theMidi is deleted.
  delete  theTrMsg;
  disconnect(&theMidi, SIGNAL(EmMiMsgRx(quint64, const QByteArray &)),                   this, SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  disconnect(&theMidi, SIGNAL(EmDrvChange(int)),                   this, SLOT(OnDrvChange()));
  disconnect(&theMidi, SIGNAL(inputPortChanged(int)),                   this, SLOT(OnDrvChange()));
  disconnect(&theMidi, SIGNAL(outputPortChanged(int)),                   this, SLOT(OnDrvChange()));
}

void    Ctrl::run() { // Overridden from QApplicatoin
  theQVwMain.show();
  theApp.exec();
}
void    Ctrl::QVwErrShow(const QString &message) {
  theQVwErr.setMessage(message);
  theQVwErr.show();
}
QString Ctrl::MiMsgDatBytesStr(const QByteArray &message, int lastIndex) {
  assert((lastIndex >= -1) && (lastIndex < message.count()));
  if(lastIndex == -1) {
    lastIndex = message.count() - 1;
  }
  QStringList dataParts;
  for(int i = 1; i <= lastIndex; i++) {
        dataParts += QString("%1").
            arg(static_cast<uint>(message[i]), 2, 16, QChar('0'));
    }
    dataParts += tr("(%1 bytes)").arg(lastIndex);
    return dataParts.join(" ");
}
void    Ctrl::OnDrvChange() {
  theQVwMain.setMessageSendEnabled((theMidi.getDriver() != -1) && (theMidi.getOutputPort() != -1));
}
void    Ctrl::OnMiMsgTx(const QString &message) {
    // Convert the message to bytes.
    QStringList bytes = message.split(' ', QString::SkipEmptyParts);
    int count = bytes.count();
    QByteArray msg;
    for(int i = 0; i < count; i++) {
        QString byteStr = bytes[i];
        bool success;
        uint value = byteStr.toUInt(&success, 16);
        if((! success) || (value > 0xff)) {
            QVwErrShow(tr("'%1' is not a valid hexadecimal MIDI byte").arg(byteStr));
            return;
        }
        msg.append(static_cast<char>(static_cast<quint8>(value)));
    }

    // Make sure the bytes represent a valid MIDI message.
    MiMsgParse(msg);
    if(! valid) {
        QVwErrShow(tr("The given message is not a valid MIDI message."));
        return;
    }

    // Send the message.
    quint64 timeStamp = theMidi.OnMiMsgTx(msg);
    theQVwMain.MsgAddTX(timeStamp, strMiStat, strMiData,                                valid);
}
void    Ctrl::OnMiMsgRx(quint64 timeStamp, const QByteArray &i_msg) {
    MiMsgParse(i_msg);
    theQVwMain.addReceivedMessage(timeStamp, strMiStat, strMiData,                                    valid);
}
void Ctrl::MiMsgParse(const QByteArray &i_msg) {
  int      miMsgLen;
  uint8_t  miStat;
  int      lenMidiSpec;
  char     miStatStr[3];
  char     tStr[256];

// Make sure we have an actual i_msg.
  miMsgLen = i_msg.count();
  if(miMsgLen == 0) {
    strMiData = "";
    strcpy(tStr, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
    strMiStat = tStr;
    //strMiStat = "empty message";
    valid = false;
    return;
  }
  miStat = (uint8_t)(i_msg[0]);
  theTrMsg->ByteToString(miStat, miStatStr);
// Validate status byte.
  if(miStat < 0x80U) {
    strMiData = MiMsgDatBytesStr(i_msg);
    strcpy(tStr, theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_LOW));
    strcat(tStr, " ");
    strcat(tStr, miStatStr);
    strMiStat = tStr;
//  strMiStat = tr("%1 (invalid status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
//  strMiStat = tr("%1 (invalid status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
    valid     = false;
    return;
  }

// Validate length and data.
    lenMidiSpec = lenMidiSpecAry[miStat - 0x80];
    int lastDataIndex;
    switch (lenMidiSpec) {

    case -1:
        strMiData = MiMsgDatBytesStr(i_msg);
        strMiStat = tr("%1 (undefined status)").
            arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
        valid = false;
        return;

    case 0:
        if(miMsgLen == 1) {
            strMiData = "";
            strMiStat = tr("System Exclusive (no data)");
            valid = false;
            return;
        }
        if(static_cast<quint8>(i_msg[miMsgLen - 1]) != 0xf7) {
            strMiData = MiMsgDatBytesStr(i_msg);
            strMiStat = tr("System Exclusive (end not found)");
            valid = false;
            return;
        }
        lastDataIndex = miMsgLen - 2;
        break;

    default:
        if(miMsgLen != lenMidiSpec) {
            strMiData = MiMsgDatBytesStr(i_msg);
            strMiStat = tr("%1 (incorrect length)").
                arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
            valid = false;
            return;
        }
        lastDataIndex = miMsgLen - 1;
    }

    // Validate data bytes.
    for(int i = 1; i <= lastDataIndex; i++) {
        if(static_cast<quint8>(i_msg[i]) >= 0x80) {
            strMiData = MiMsgDatBytesStr(i_msg);
            strMiStat = tr("%1 (invalid data)").
                arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
            valid = false;
            return;
        }
    }

    // Convert i_msg to user-friendly strings.
    QString s;
    int value;
    valid = true;
    switch (miStat & (uint8_t)0xf0) {

    case 0x80:
        strMiData = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(i_msg[1]))).
            arg(static_cast<quint8>(i_msg[2]));
        strMiStat = tr("Note Off, Channel %1").arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0x90:
        strMiData = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(i_msg[1]))).
            arg(static_cast<quint8>(i_msg[2]));
        strMiStat = tr("Note On, Channel %1").arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xa0:
        strMiData = tr("Note: %1, Pressure: %2").
            arg(getMIDINoteString(static_cast<quint8>(i_msg[1]))).
            arg(static_cast<quint8>(i_msg[2]));
        strMiStat = tr("Aftertouch, Channel %1").
            arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xb0:
        strMiData = tr("CC: %1, Value: %2").
            arg(getMIDIControlString(static_cast<quint8>(i_msg[1]))).
            arg(static_cast<quint8>(i_msg[2]));
        strMiStat = tr("CC, Channel %1").
            arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xc0:
        strMiData = tr("Number: %1").arg(static_cast<quint8>(i_msg[1]));
        strMiStat = tr("Program Change, Channel %1").
            arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xd0:
        strMiData = tr("Pressure: %1").
            arg(static_cast<quint8>(i_msg[1]));
        strMiStat = tr("Channel Pressure, Channel %1").
            arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xe0:
        strMiData = tr("Value: %1").
            arg((((static_cast<qint16>(i_msg[2])) << 7) |
                 (static_cast<qint16>(i_msg[1]))) - 0x2000);
        strMiStat = tr("Pitch Wheel, Channel %1").
            arg((miStat & (uint8_t)0x0F) + 1);
        break;

    case 0xf0:
      switch (miStat & (uint8_t)0x0F) {
        case 0x0:
          strMiData = MiMsgDatBytesStr(i_msg, lastDataIndex);
          strMiStat = tr("System Exclusive");
          break;
        case 0x1:
          value = i_msg[0] & 0xf;
          switch (i_msg[0] & 0x70) {
            case 0x00:  strMiData = tr("Frames Low Nibble: %1"   ).arg(value);  break;
            case 0x10:  strMiData = tr("Frames High Nibble: %1"  ).arg(value);  break;
            case 0x20:  strMiData = tr("Seconds Low Nibble: %1"  ).arg(value);  break;
            case 0x30:  strMiData = tr("Seconds High Nibble: %1" ).arg(value);  break;
            case 0x40:  strMiData = tr("Minutes Low Nibble: %1"  ).arg(value);  break;
            case 0x50:  strMiData = tr("Minutes High Nibble: %1" ).arg(value);  break;
            case 0x60:  strMiData = tr("Hours Low Nibble: %1"    ).arg(value);  break;
            case 0x70:
              switch ((value & 0x6) >> 1) {
                case 0:                    s = tr("24 fps");                    break;
                case 1:                    s = tr("25 fps");                    break;
                case 2:                    s = tr("30 fps (drop-frame)");                    break;
                case 3:                    s = tr("30 fps");                    break;
                default:assert(false);                    // We shouldn't get here.
              }
              strMiData = tr("Hours High Nibble: %1, SMPTE Type: %2"). arg(value & 1).arg(s);
              break;
            default:  assert(false);                // We shouldn't get here.
          }
          strMiStat = tr("MTC Quarter Frame");
          break;
        case 0x2:
          strMiData = tr("MIDI Beat: %1").arg(((static_cast<qint16>(i_msg[2])) << 7) | (static_cast<qint16>(i_msg[1])));
          strMiStat = tr("Song Position Pointer");
          break;
        case 0x3:
          strMiData = tr("Song Number: %1").arg(static_cast<quint8>(i_msg[1]));
          strMiStat = tr("Song Select");
          break;
        case 0x6:  strMiData = "";  strMiStat = tr("Tune Request"  );  break;
        case 0x8:  strMiData = "";  strMiStat = tr("MIDI Clock"    );  break;
        case 0x9:  strMiData = "";  strMiStat = tr("MIDI Tick"     );  break;
        case 0xa:  strMiData = "";  strMiStat = tr("MIDI Start"    );  break;
        case 0xb:  strMiData = "";  strMiStat = tr("MIDI Continue" );  break;
        case 0xc:  strMiData = "";  strMiStat = tr("MIDI Stop"     );  break;
        case 0xe:  strMiData = "";  strMiStat = tr("Active Sense"  );  break;
        case 0xf:  strMiData = "";  strMiStat = tr("Reset"         );  break;
        default:   assert(false);   // We shouldn't get here.
        }
        break;
    default:   assert(false);       // We shouldn't get here.
  }
}

