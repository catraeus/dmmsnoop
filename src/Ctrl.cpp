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



Ctrl:: Ctrl(App &i_theApp, QObject *i_parent) : QObject(i_parent), theApp(i_theApp) {
int driverCount;
int driver;
int outputPort;
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
  driverCount = theMidi.getDriverCount();
  if(! driverCount)        throw Error(tr("no MIDI drivers found"));
  for(int i = 0; i < driverCount; i++)        theQVwConfig.OnMidiDrvAdd(i, theMidi.getDriverName(i));
  driver = theMidi.MiDrvNumGet();
  outputPort = theMidi.getOutputPort();
  theQVwConfig.OnMidiDrvChg                        (driver);
  theQVwConfig.OnPortInpChg(theMidi.getInputPort ());
  theQVwConfig.OnModeIgnActSnChg   (theMidi.ModeIgnActSnGet());
  theQVwConfig.OnModeIgnSysExChg   (theMidi.ModeIgnSysExGet());
  theQVwConfig.OnModeIgnMiTimChg   (theMidi.ModeIgnMiTimGet());
  theQVwConfig.OnPortOutChg        (outputPort);
  connect(&theQVwConfig, SIGNAL(closeRequest      (                           )),  &theQVwConfig, SLOT(hide()));
  connect(&theQVwConfig, SIGNAL(EmMidiDrvChg      (int                        )),  &theMidi,      SLOT(OnDrvChg(int)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnActSnChg (bool                       )),  &theMidi,      SLOT(OnModeIgnActSnChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnSysExChg (bool                       )),  &theMidi,      SLOT(OnModeIgnSysExChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnMiTimChg (bool                       )),  &theMidi,      SLOT(OnModeIgnMiTimChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmPortInpChg      (int                        )),  &theMidi,      SLOT(OnPortInpChg(int)));
  connect(&theQVwConfig, SIGNAL(EmPortOutChg      (int                        )),  &theMidi,      SLOT(OnPortOutChg(int)));

//==================================================================================================
//==== Setup Error
  connect(&theQVwErr,  SIGNAL(closeRequest        (                           )),  &theQVwErr,    SLOT(hide()));

//==================================================================================================
//==== Setup Main
  theQVwMain.OnMiMsgTxEn((driver != -1) && (outputPort != -1));
  theQVwMain.SetTimeZero(theMidi.TimeGet());
  connect(&theQVwMain, SIGNAL(EmAppAbout          (                           )),  &theQVwAbout,  SLOT(show()));
  connect(&theQVwMain, SIGNAL(EmMiMsgTXAdd        (                           )),  &theQVwMsg,    SLOT(show()));
  connect(&theQVwMain, SIGNAL(EmMiMsgTabClr       (                           )),  &theQVwMain,   SLOT(OnMiMsgTabClr()));
  connect(&theQVwMain, SIGNAL(EmAppConfig         (                           )),  &theQVwConfig, SLOT(show()));
  connect(&theQVwMain, SIGNAL(closeRequest        (                           )),  &theApp,       SLOT(quit()));

//==================================================================================================
//==== Setup Message Send
  connect(&theQVwMsg, SIGNAL(closeRequest         (                           )),  &theQVwMsg,    SLOT(hide()));
  connect(&theQVwMsg, SIGNAL(sendRequest          (const QString &            )),  &theQVwMsg,    SLOT(hide()));
  connect(&theQVwMsg, SIGNAL(sendRequest          (const QString &            )),  this,          SLOT(OnMiMsgTx(const QString &)));

//==================================================================================================
//==== Setup Midi worker  FIXME, there are multiple "overloads and multiple inheritances" of these.  Does order matter ! ? ! ?
  connect(&theMidi, SIGNAL(EmMiMsgRx              (quint64, const QByteArray &)),  this,          SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  connect(&theMidi, SIGNAL(EmDrvChange            (int                        )),  &theQVwConfig, SLOT(OnMidiDrvChg(int)));
  connect(&theMidi, SIGNAL(EmDrvChange            (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(&theMidi, SIGNAL(EmModeIgnActSnChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnActSnChg(bool)));
  connect(&theMidi, SIGNAL(EmModeIgnSysExChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnSysExChg(bool)));
  connect(&theMidi, SIGNAL(EmModeIgnMiTimChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnMiTimChg(bool)));
  connect(&theMidi, SIGNAL(EmPortInpAdd           (int, QString               )),  &theQVwConfig, SLOT(OnPortInpAdd(int, QString)));
  connect(&theMidi, SIGNAL(EmPortInpChg           (int                        )),  &theQVwConfig, SLOT(OnPortInpChg(int)));
  connect(&theMidi, SIGNAL(EmPortInpChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(&theMidi, SIGNAL(EmPortInpDel           (int                        )),  &theQVwConfig, SLOT(OnPortInpDel(int)));
  connect(&theMidi, SIGNAL(EmPortOutAdd           (int, QString               )),  &theQVwConfig, SLOT(OnPortOutAdd(int, QString)));
  connect(&theMidi, SIGNAL(EmPortOutChg           (int                        )),  &theQVwConfig, SLOT(OnPortOutChg(int)));
  connect(&theMidi, SIGNAL(EmPortOutChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(&theMidi, SIGNAL(EmPortOutDel           (int                        )),  &theQVwConfig, SLOT(OnPortOutDel(int)));

//==================================================================================================
//==== Setup theApp
  connect(&theApp,  SIGNAL(eventError             (QString                    )),  &theQVwErr,    SLOT(setMessage(QString)));
  connect(&theApp,  SIGNAL(eventError             (QString                    )),  &theQVwErr,    SLOT(show()));
}
Ctrl::~Ctrl() {
// Disconnect theMidi signals handled by the controller before the theMidi is deleted.
  delete  theTrMsg;
  disconnect(&theMidi, SIGNAL(EmMiMsgRx    (quint64, const QByteArray &)), this, SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  disconnect(&theMidi, SIGNAL(EmDrvChange  (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(&theMidi, SIGNAL(EmPortInpChg (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(&theMidi, SIGNAL(EmPortOutChg (int                        )), this, SLOT(OnMidiDrvChg()));
}

void    Ctrl::run              (                      ) { // Overridden from QApplicatoin
  theQVwMain.show();
  theApp.exec();
}
void    Ctrl::QVwErrShow       (const QString &message) {
  theQVwErr.setMessage(message);
  theQVwErr.show();
}
char   *Ctrl::MiMsgDatBytesStr (              const QByteArray &i_msgbb, int i_dexLast) {
  //QStringList dataParts;
  char        dStr[32];
  uint        tInt;
  char        tStr[32];

  if(i_dexLast == -1)
    i_dexLast = i_msgbb.count() - 1;
  dStr[0] =  '\0';
  for(int i = 1; i <= i_dexLast; i++) {
    tInt = (uint)(i_msgbb[i]);
    theTrMsg->ByteToString(tInt, tStr);
    if(i > 1) strcat(dStr, " ");
    strcat(dStr, tStr);
    //dataParts += tStr;
  }
  sprintf(tStr, " (%d bytes)", i_dexLast);
  strcat(dStr, tStr);
  //dataParts += tr("(%1 bytes)").arg(i_dexLast);
  return &(dStr[0]);
  //return dataParts.join(" ");
}
void    Ctrl::OnMidiDrvChg     (                      ) {
  theQVwMain.OnMiMsgTxEn((theMidi.MiDrvNumGet() != -1) && (theMidi.getOutputPort() != -1));
}
void    Ctrl::OnMiMsgTx        (const QString &message) {
  QStringList bytes;
  int         count;
  bool        success;
  QByteArray  msg;
  QString     byteStr;
  uint        value;
  quint64     TS;

  bytes = message.split(' ', QString::SkipEmptyParts);// MAGICK The GUI gives us the message as text ...Convert the message to bytes.
  count = bytes.count();
  for(int i = 0; i < count; i++) {
    byteStr = bytes[i];
    value = byteStr.toUInt(&success, 16);
    if((! success) || (value > 0xff)) {
      QVwErrShow(tr("'%1' is not a valid hexadecimal MIDI byte").arg(byteStr));
      return;
    }
    msg.append(static_cast<char>(static_cast<quint8>(value)));
  }

  MiMsgParse(msg);  // Make sure the bytes represent a valid MIDI message.
 // if(! valid) {
 //   QVwErrShow(tr("The given message is not a valid MIDI message."));
//    return;
 // }

    // Send the message.
  TS = theMidi.OnMiMsgTx(msg);
  theQVwMain.OnMiMsgTX(TS, strMiStat, strMiData, valid);
}
void    Ctrl::OnMiMsgRx        (quint64 i_TS, const QByteArray &i_msg) {
    MiMsgParse(i_msg);
    theQVwMain.OnMiMsgRX(i_TS, strMiStat, strMiData, valid);
}
void    Ctrl::MiMsgParse       (              const QByteArray &i_msg) {
  int      miMsgLen;
  uint8_t  miStat;
  uint     miStatBase;
  int      lenMidiSpec;
  char     miStatByteStr[3]; // two nibbles and a trailing zero
  char     tStr[256];
  int      lastDataIndex;
  QString  s;
  int      value;

//========
  miMsgLen = i_msg.count();
  if(miMsgLen == 0) {  // WEIRD Make sure we have something ... anything.
    strMiData = "";
    strcpy(tStr, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
    strMiStat = tStr;
    valid = false;
    return;
  }


//========
  miStat = (uint8_t)(i_msg[0]);
  theTrMsg->ByteToString(miStat, miStatByteStr);

//========
  if(miStat < 0x80U) {// WEIRD Validate status byte.
    strMiData = MiMsgDatBytesStr(i_msg);
    strcpy(tStr, theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_LOW));
    strcat(tStr, " ");
    strcat(tStr, miStatByteStr);
    strMiStat = tStr;
//  strMiStat = tr("%1 (invalid status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
//  strMiStat = tr("%1 (invalid status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
    valid     = false;
    return;
  }

//========
// Validate length and data.
  lenMidiSpec = lenMidiSpecAry[miStat - 0x80];
  switch (lenMidiSpec) {
    case -1:  // FIXME  4, 5 and D are reserved, but 7 is SysEx End, we have to build a stateful machine to detect this being OK or Bad.
      strMiData = MiMsgDatBytesStr(i_msg);
      strMiStat = tr("%1 (undefined status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
      valid = false;
      return;
      break;
    case 0:
      if(miMsgLen == 1) {
        strMiData = "";
        strMiStat = tr("System Exclusive (no data)");
        valid = false;
        return;
      }
      if(static_cast<quint8>(i_msg[miMsgLen - 1]) != 0xf7) {
        strMiData = MiMsgDatBytesStr(i_msg);
        strMiStat = tr("System Exclusive (end not found)"); // So I guess that the RtMidi
        valid = false;
        return;
      }
      lastDataIndex = miMsgLen - 2;
      break;
    default:
      if(miMsgLen != lenMidiSpec) {
        strMiData = MiMsgDatBytesStr(i_msg);
        strMiStat = tr("%1 (incorrect length)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
        valid = false;
        return;
      }
      lastDataIndex = miMsgLen - 1;
      break;
    }

//========
// Validate data bytes.
  for(int i = 1; i <= lastDataIndex; i++) {
    if(static_cast<quint8>(i_msg[i]) >= 0x80) {
      strMiData = MiMsgDatBytesStr(i_msg);
      strMiStat = tr("%1 (invalid data)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
      valid = false;
      return;
    }
  }

//========
// Convert i_msg to user-friendly strings.
  valid = true;
  miStatBase   = (uint)miStat;
  miStatBase  &= 0x70U;
  miStatBase >>= 4;
  fprintf(stdout, "Stat: %X\n", miStatBase);
  switch (miStatBase) {
    case 0x0:
      strMiData = tr("N  %1, V   %2"   ). arg(getMIDINoteString   (static_cast<quint8>(i_msg[1]))).  arg(static_cast<quint8>(i_msg[2]));
      strMiStat = tr("NOff,  Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x1:
      strMiData = tr("N  %1, V   %2"   ). arg(getMIDINoteString   (static_cast<quint8>(i_msg[1]))).  arg(static_cast<quint8>(i_msg[2]));
      strMiStat = tr("NOn,   Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x2:
      strMiData = tr("N  %1, V   %2"   ). arg(getMIDINoteString   (static_cast<quint8>(i_msg[1]))).  arg(static_cast<quint8>(i_msg[2]));
      strMiStat = tr("PPr    Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x3:
      strMiData = tr("CC %1, V   %2"   ). arg(getMIDIControlString(static_cast<quint8>(i_msg[1]))).  arg(static_cast<quint8>(i_msg[2]));
      strMiStat = tr("CC,    Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x4:
      strMiData = tr("Prog   No  %1"   ). arg(static_cast<quint8>(i_msg[1]));
      strMiStat = tr("Prog,  Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x5:
      strMiData = tr("ChPres V   %1"   ). arg(static_cast<quint8>(i_msg[1]));
      strMiStat = tr("ChPres Ch  %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x6:
      strMiData = tr("PtchWh V   %1"   ). arg((((static_cast<qint16>(i_msg[2])) << 7) | (static_cast<qint16>(i_msg[1]))) - 0x2000);
      strMiStat = tr("PtchWh V   %1"   ). arg((miStat & (uint8_t)0x0F) + 1);
      break;
    case 0x7: // FIXME, unload this to a whole new function.
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
                default:   ;               // We shouldn't get here.
              }
              strMiData = tr("Hours High Nibble: %1, SMPTE Type: %2"). arg(value & 1).arg(s);
              break;
            default:        ;        // We shouldn't get here.
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
        default:        ;   // We shouldn't get here.
        }
        break;
    default:    ;  // We shouldn't get here.
  }
}

