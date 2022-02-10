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
#include <sys/time.h>

#include <QtCore/QDebug>
#include <QtCore/QMetaType>

#include "Ctrl.hpp"

#include "util/DmmTypes.hpp"
#include "util/Error.hpp"
#include "util/DmmStr.hpp"

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
/*0xF0*/ 0,  2,  3,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1  // Aren't we Speecial?  The SysEx and beyond controls
};



Ctrl:: Ctrl(App &i_theApp, QObject *i_parent) : QObject(i_parent), theApp(i_theApp) {
  int      driverCount;
  int      driver;
  int      outputPort;
  quint64   TS;
  char     tStr[256];

//==================================================================================================
//==== Do the important first things.
  theDrvIf = new DrvIf();
  theMidi = new Midi();
  theDrvIf->SetMidi(theMidi);
  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
//==================================================================================================
//==== Do time things.
  TS = GetTS();
  TimeUsToStrSec(TS, tStr);
  fprintf(stdout, "MICROSECONDS:  %s\n", tStr);

//==================================================================================================
//==== Setup About
  theQVwAbout.setMajorVersion(DMMSNOOP_MAJOR_VERSION);
  theQVwAbout.setMinorVersion(DMMSNOOP_MINOR_VERSION);
  theQVwAbout.setRevision(DMMSNOOP_REVISION);
  connect(&theQVwAbout, SIGNAL(closeRequest()), &theQVwAbout, SLOT(hide()));

//==================================================================================================
//==== Setup Config
  driverCount = theDrvIf->getDriverCount();
  if(! driverCount)        throw Error(tr("no MIDI drivers found"));
  for(int i = 0; i < driverCount; i++)        theQVwConfig.OnMidiDrvAdd(i, theDrvIf->getDriverName(i));
  driver = theDrvIf->MiDrvNumGet();
  outputPort = theDrvIf->getOutputPort();
  theQVwConfig.OnMidiDrvChg                        (driver);
  theQVwConfig.OnPortInpChg(theDrvIf->getInputPort ());
  theQVwConfig.OnModeIgnActSnChg   (theDrvIf->ModeIgnActSnGet());
  theQVwConfig.OnModeIgnSysExChg   (theDrvIf->ModeIgnSysExGet());
  theQVwConfig.OnModeIgnMiTimChg   (theDrvIf->ModeIgnMiTimGet());
  theQVwConfig.OnPortOutChg        (outputPort);
  connect(&theQVwConfig, SIGNAL(closeRequest      (                           )),  &theQVwConfig, SLOT(hide()));
  connect(&theQVwConfig, SIGNAL(EmMidiDrvChg      (int                        )),  theDrvIf,      SLOT(OnDrvChg(int)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnActSnChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnActSnChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnSysExChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnSysExChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmModeIgnMiTimChg (bool                       )),  theDrvIf,      SLOT(OnModeIgnMiTimChg(bool)));
  connect(&theQVwConfig, SIGNAL(EmPortInpChg      (int                        )),  theDrvIf,      SLOT(OnPortInpChg(int)));
  connect(&theQVwConfig, SIGNAL(EmPortOutChg      (int                        )),  theDrvIf,      SLOT(OnPortOutChg(int)));

//==================================================================================================
//==== Setup Error
  connect(&theQVwErr,  SIGNAL(closeRequest        (                           )),  &theQVwErr,    SLOT(hide()));

//==================================================================================================
//==== Setup Main
  theQVwMain.OnMiMsgTxEn((driver != -1) && (outputPort != -1));
  TS = GetTS();
  theQVwMain.SetTimeZero(TS);
  connect(&theQVwMain, SIGNAL(EmAppAbout          (                           )),  &theQVwAbout,  SLOT(show()));
  connect(&theQVwMain, SIGNAL(EmMiMsgTXAdd        (                           )),  &theQVwMsg,    SLOT(show()));
  connect(&theQVwMain, SIGNAL(EmMiMsgTabClr       (                           )),  &theQVwMain,   SLOT(OnMiMsgTabClr()));
  connect(&theQVwMain, SIGNAL(EmAppConfig         (                           )),  &theQVwConfig, SLOT(show()));
  connect(&theQVwMain, SIGNAL(closeRequest        (                           )),  &theApp,       SLOT(quit()));

//==================================================================================================
//==== Setup Message Send
  connect(&theQVwMsg, SIGNAL(closeRequest         (                           )),  &theQVwMsg,    SLOT(hide()));
  connect(&theQVwMsg, SIGNAL(EmMsgSend            (const QString &            )),  &theQVwMsg,    SLOT(hide()));
  connect(&theQVwMsg, SIGNAL(EmMsgSend            (const QString &            )),  this,          SLOT(OnMiMsgTx(const QString &)));

//==================================================================================================
//==== Setup DrvIf worker  FIXME, there are multiple "overloads and multiple inheritances" of these.  Does order matter ! ? ! ?
  connect(theDrvIf, SIGNAL(EmMiMsgRx              (quint64, const QByteArray &)),  this,          SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  connect(theDrvIf, SIGNAL(EmDrvChange            (int                        )),  &theQVwConfig, SLOT(OnMidiDrvChg(int)));
  connect(theDrvIf, SIGNAL(EmDrvChange            (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmModeIgnActSnChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnActSnChg(bool)));
  connect(theDrvIf, SIGNAL(EmModeIgnSysExChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnSysExChg(bool)));
  connect(theDrvIf, SIGNAL(EmModeIgnMiTimChg      (bool                       )),  &theQVwConfig, SLOT(OnModeIgnMiTimChg(bool)));
  connect(theDrvIf, SIGNAL(EmPortInpAdd           (int, QString               )),  &theQVwConfig, SLOT(OnPortInpAdd(int, QString)));
  connect(theDrvIf, SIGNAL(EmPortInpChg           (int                        )),  &theQVwConfig, SLOT(OnPortInpChg(int)));
  connect(theDrvIf, SIGNAL(EmPortInpChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmPortInpDel           (int                        )),  &theQVwConfig, SLOT(OnPortInpDel(int)));
  connect(theDrvIf, SIGNAL(EmPortOutAdd           (int, QString               )),  &theQVwConfig, SLOT(OnPortOutAdd(int, QString)));
  connect(theDrvIf, SIGNAL(EmPortOutChg           (int                        )),  &theQVwConfig, SLOT(OnPortOutChg(int)));
  connect(theDrvIf, SIGNAL(EmPortOutChg           (int                        )),  this,          SLOT(OnMidiDrvChg()));
  connect(theDrvIf, SIGNAL(EmPortOutDel           (int                        )),  &theQVwConfig, SLOT(OnPortOutDel(int)));

//==================================================================================================
//==== Setup theApp
  connect(&theApp,  SIGNAL(eventError             (QString                    )),  &theQVwErr,    SLOT(setMessage(QString)));
  connect(&theApp,  SIGNAL(eventError             (QString                    )),  &theQVwErr,    SLOT(show()));
}
Ctrl::~Ctrl() {
// Disconnect theDrvIf signals handled by the controller before the theDrvIf is deleted.
  delete  theTrMsg;
  disconnect(theDrvIf, SIGNAL(EmMiMsgRx    (quint64, const QByteArray &)), this, SLOT(OnMiMsgRx(quint64, const QByteArray &)));
  disconnect(theDrvIf, SIGNAL(EmDrvChange  (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(theDrvIf, SIGNAL(EmPortInpChg (int                        )), this, SLOT(OnMidiDrvChg()));
  disconnect(theDrvIf, SIGNAL(EmPortOutChg (int                        )), this, SLOT(OnMidiDrvChg()));
}

void    Ctrl::run              (                      ) { // Overridden from QApplicatoin
  theQVwMain.show();
  theApp.exec();
}
void    Ctrl::QVwErrShow       (const QString &message) {
  theQVwErr.setMessage(message);
  theQVwErr.show();
}
void    Ctrl::MiMsgDatBytesStr (const QByteArray &i_msgbb, char *o_dStr, int i_dexLast) {
  uint        tInt;
  char        tStr[32];

  if(i_dexLast == -1)
    i_dexLast = i_msgbb.count() - 1;
  o_dStr[0] =  '\0';
  for(int i = 1; i <= i_dexLast; i++) {
    tInt = (uint)(i_msgbb[i]);
    ByteToString(tInt, tStr);
    if(i > 1) strcat(o_dStr, " ");
    strcat(o_dStr, tStr);
  }
  sprintf(tStr, " (%d bytes)", i_dexLast);
  strcat(o_dStr, tStr);
  return;
}
void    Ctrl::OnMidiDrvChg     (                      ) {
  theQVwMain.OnMiMsgTxEn((theDrvIf->MiDrvNumGet() != -1) && (theDrvIf->getOutputPort() != -1));
}
void    Ctrl::OnMiMsgTx        (const QString &i_miMsgStr) {
  QStringList lines;
  QStringList bytes;
  uint        byteCount;
  uint        lineCount;
  bool        success;
  QByteArray  miMsgBytes;
  QString     byteStr;
  uint        value;
  quint64      TS;

  lines = i_miMsgStr.split('\n', QString::SkipEmptyParts);
  lineCount = lines.count();
//bytes = i_miMsgStr.split(' ', QString::SkipEmptyParts);// MAGICK The GUI gives us the i_miMsgStr as text ...Convert the i_miMsgStr to bytes.
  for(uint lineDex=0; lineDex<lineCount; lineDex++) {
    bytes = lines[lineDex].split(' ', QString::SkipEmptyParts);// MAGICK The GUI gives us the i_miMsgStr as text ...Convert the i_miMsgStr to bytes.
    byteCount = bytes.count();
    miMsgBytes.clear();
    for(uint byteDex = 0; byteDex<byteCount; byteDex++) {
      byteStr = bytes[byteDex];
      value = byteStr.toUInt(&success, 16);
      if((! success) || (value > 0xff)) {
        QVwErrShow(tr("'%1' is not a valid hexadecimal MIDI byte").arg(byteStr));
        return;
      }
      miMsgBytes.append(static_cast<char>(static_cast<quint8>(value)));
    }

    MiMsgParse(miMsgBytes);  // Make sure the bytes represent a valid MIDI i_miMsgStr.
    if(valid)
      TS = theDrvIf->OnMiMsgTx(miMsgBytes);
    else
      TS = GetTS();
    theQVwMain.OnMiMsgTX(TS, strMiStat, strMiData, theMidi, valid);
  }
}
void    Ctrl::OnMiMsgRx        (quint64 i_TS, const QByteArray &i_msg) {
    MiMsgParse(i_msg);
    theQVwMain.OnMiMsgRX(i_TS, strMiStat, strMiData, theMidi, valid);
}
void    Ctrl::MiMsgParse       (              const QByteArray &i_msg) {
  uint     lenMidiSpec;
  char     miStatByteStr[3]; // two nibbles and a trailing zero
  uint     miBytes[1024];
  char     tStr[256];
  int      lastDataIndex;
  QString  s;
  int      value;

//========
  miMsgLen = i_msg.count();
  for(uint i=0; i<miMsgLen; i++)
    miBytes[i] = ((uint)i_msg[i] & 0x000000FFU); // That freq king QByteArray is signed and sign extends on coersion!
  theMidi->Parse(miMsgLen, miBytes);
  if(miMsgLen == 0) {  // WEIRD Make sure we have something ... anything.
    strMiData = "";
    strcpy(tStr, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
    strMiStat = tStr;
    valid = false;
    return;
  }


//========
  miStat = (uint8_t)(i_msg[0]);
  ByteToString(miStat, miStatByteStr);

//========
  if(miStat < 0x80U) {// WEIRD Validate status byte.
    MiMsgDatBytesStr(i_msg, tStr);
    strMiData = tStr;
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
    //case -1:  // FIXME  4, 5 and D are reserved, but 7 is SysEx End, we have to build a stateful machine to detect this being OK or Bad.
    //  MiMsgDatBytesStr(i_msg, tStr);
    //  strMiData = tStr;
    //  strMiStat = tr("%1 (undefined status)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
    //  valid = false;
    //  return;
    //  break;
    case 0:
    //  if(miMsgLen == 1) {
    //    strMiData = "";
    //    strMiStat = tr("System Exclusive (no data)");
    //    valid = false;
    //    return;
    //  }
    //  if(static_cast<quint8>(i_msg[miMsgLen - 1]) != 0xf7) {
    //    MiMsgDatBytesStr(i_msg, tStr);
    //    strMiData = tStr;
    //    strMiStat = tr("System Exclusive (end not found)"); // So I guess that the RtMidi
    //    valid = false;
    //    return;
    //  }
      lastDataIndex = miMsgLen - 2;
        break;
    default:
      //if(miMsgLen != lenMidiSpec) {
      //  MiMsgDatBytesStr(i_msg, tStr);
      //  strMiData = tStr;
      //  strMiStat = tr("%1 (incorrect length)").arg(static_cast<uint>(miStat), 2, 16, QChar('0'));
      //  valid = false;
      //  return;
      //}
      //else {
        MiMsgDatBytesStr(i_msg, tStr);
        strMiData = tStr;
        strMiStat = tr("Workin' it");
        valid = false;
      //}
      lastDataIndex = miMsgLen - 1;
      break;
    }

//========
// Validate data bytes.
  for(int i = 1; i <= lastDataIndex; i++) {
    if(static_cast<quint8>(i_msg[i]) >= 0x80) {
      MiMsgDatBytesStr(i_msg, tStr);
      strMiData = tStr;
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
  switch (miStatBase) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
      strMiData = tr(""   );
      strMiStat = tr(""   );
      break;
    case 0x7: // FIXME, unload this to a whole new function.
      switch (miStat & (uint8_t)0x0F) {
        case 0x0:
          strMiData = tr("");
          strMiStat = tr("");
          break;
        case 0x1: // FIXME, MTC needs order validation, accumulation and time correlation.
          value = i_msg[1] & 0x0fU;
          switch (i_msg[1] & 0x70U) {
            case 0x00:   strMiData = tr("MTC ..:..:..+.f %1 rrd"  ).arg(value);  break;
            case 0x10:   strMiData = tr("MTC ..:..:..+f. %1 rrd"  ).arg(value);  break;
            case 0x20:   strMiData = tr("MTC ..:..:.s+.. %1 rrd"  ).arg(value);  break;
            case 0x30:   strMiData = tr("MTC ..:..:s.+.. %1 rrd"  ).arg(value);  break;
            case 0x40:   strMiData = tr("MTC ..:.m:..+.. %1 rrd"  ).arg(value);  break;
            case 0x50:   strMiData = tr("MTC ..:m.:..+.. %1 rrd"  ).arg(value);  break;
            case 0x60:   strMiData = tr("MTC .h:..:..+.. %1 rrd"  ).arg(value);  break;
            case 0x70:
              switch ((value & 0x7) >> 1) {
                case 0:  strMiData = tr("MTC h.:..:..+..  0 24."    );            break;
                case 1:  strMiData = tr("MTC h.:..:..+..  1 24."    );            break;
                case 2:  strMiData = tr("MTC h.:..:..+..  0 25."    );            break;
                case 3:  strMiData = tr("MTC h.:..:..+..  1 25."    );            break;
                case 4:  strMiData = tr("MTC h.:..:..+..  0 30D"    );            break;
                case 5:  strMiData = tr("MTC h.:..:..+..  1 30D"    );            break;
                case 6:  strMiData = tr("MTC h.:..:..+..  0 30."    );            break;
                case 7:  strMiData = tr("MTC h.:..:..+..  1 30."    );            break;
                default:   ;               // We shouldn't get here.
              }
              break;
            default:        ;        // We shouldn't get here.
          }
          strMiStat = tr("MTC QF");
          break;
        case 0x2:
        case 0x3:
          strMiData = "";
          strMiStat = "";
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

