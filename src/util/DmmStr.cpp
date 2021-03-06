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

#include <sys/time.h>
#include <cassert>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtUiTools/QUiLoader>

#include "DmmStr.hpp"

const char   hexAscii[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

QString getMIDIControlString(quint8 i_CC) {
    QString name;

  if((quint8)i_CC >= (quint8)0x80) {
    name = qApp->tr("ERROR: DATA is actually a STATUS");
  }
  else {
    switch (i_CC) {
      case 0x00:  name = qApp->tr("Bank Select"                                              );  break;
      case 0x01:  name = qApp->tr("Modulation Wheel"                                         );  break;
      case 0x02:  name = qApp->tr("Breath Controller"                                        );  break;
      case 0x04:  name = qApp->tr("Foot Controller"                                          );  break;
      case 0x05:  name = qApp->tr("Portamento Time"                                          );  break;
      case 0x06:  name = qApp->tr("Data Entry MSB"                                           );  break;
      case 0x07:  name = qApp->tr("Channel Volume"                                           );  break;
      case 0x08:  name = qApp->tr("Balance"                                                  );  break;
      case 0x0a:  name = qApp->tr("Pan"                                                      );  break;
      case 0x0b:  name = qApp->tr("Expression Controller"                                    );  break;
      case 0x0c:  name = qApp->tr("Effect Control 1"                                         );  break;
      case 0x0d:  name = qApp->tr("Effect Control 2"                                         );  break;
      case 0x10:  name = qApp->tr("General Purpose Controller 1"                             );  break;
      case 0x11:  name = qApp->tr("General Purpose Controller 2"                             );  break;
      case 0x12:  name = qApp->tr("General Purpose Controller 3"                             );  break;
      case 0x13:  name = qApp->tr("General Purpose Controller 4"                             );  break;
      case 0x20:  name = qApp->tr("Bank Select LSB"                                          );  break;
      case 0x21:  name = qApp->tr("Modulation Wheel LSB"                                     );  break;
      case 0x22:  name = qApp->tr("Breath Controller LSB"                                    );  break;
      case 0x23:  name = qApp->tr("Control 3 LSB"                                            );  break;
      case 0x24:  name = qApp->tr("Foot Controller LSB"                                      );  break;
      case 0x25:  name = qApp->tr("Portamento Time LSB"                                      );  break;
      case 0x26:  name = qApp->tr("Data Entry LSB"                                           );  break;
      case 0x27:  name = qApp->tr("Channel Volume LSB"                                       );  break;
      case 0x28:  name = qApp->tr("Balance LSB"                                              );  break;
      case 0x29:  name = qApp->tr("Control 9 LSB"                                            );  break;
      case 0x2a:  name = qApp->tr("Pan LSB"                                                  );  break;
      case 0x2b:  name = qApp->tr("Expression Controller LSB"                                );  break;
      case 0x2c:  name = qApp->tr("Effect Control 1 LSB"                                     );  break;
      case 0x2d:  name = qApp->tr("Effect Control 2 LSB"                                     );  break;
      case 0x2e:  name = qApp->tr("Control 14 LSB"                                           );  break;
      case 0x2f:  name = qApp->tr("Control 15 LSB"                                           );  break;
      case 0x30:  name = qApp->tr("General Purpose Controller 1 LSB"                         );  break;
      case 0x31:  name = qApp->tr("General Purpose Controller 2 LSB"                         );  break;
      case 0x32:  name = qApp->tr("General Purpose Controller 3 LSB"                         );  break;
      case 0x33:  name = qApp->tr("General Purpose Controller 4 LSB"                         );  break;
      case 0x34:  name = qApp->tr("Control 20 LSB"                                           );  break;
      case 0x35:  name = qApp->tr("Control 21 LSB"                                           );  break;
      case 0x36:  name = qApp->tr("Control 22 LSB"                                           );  break;
      case 0x37:  name = qApp->tr("Control 23 LSB"                                           );  break;
      case 0x38:  name = qApp->tr("Control 24 LSB"                                           );  break;
      case 0x39:  name = qApp->tr("Control 25 LSB"                                           );  break;
      case 0x3a:  name = qApp->tr("Control 26 LSB"                                           );  break;
      case 0x3b:  name = qApp->tr("Control 27 LSB"                                           );  break;
      case 0x3c:  name = qApp->tr("Control 28 LSB"                                           );  break;
      case 0x3d:  name = qApp->tr("Control 29 LSB"                                           );  break;
      case 0x3e:  name = qApp->tr("Control 30 LSB"                                           );  break;
      case 0x3f:  name = qApp->tr("Control 31 LSB"                                           );  break;
      case 0x40:  name = qApp->tr("Damper Pedal On/Off"                                      );  break;
      case 0x41:  name = qApp->tr("Portamento On/Off"                                        );  break;
      case 0x42:  name = qApp->tr("Sostenuto On/Off"                                         );  break;
      case 0x43:  name = qApp->tr("Soft Pedal On/Off"                                        );  break;
      case 0x44:  name = qApp->tr("Legato Footswitch"                                        );  break;
      case 0x45:  name = qApp->tr("Hold 2"                                                   );  break;
      case 0x46:  name = qApp->tr("Sound Controller 1 (Default: Sound Variation)"            );  break;
      case 0x47:  name = qApp->tr("Sound Controller 2 (Default: Timbre/Harmonic Intensity)"  );  break;
      case 0x48:  name = qApp->tr("Sound Controller 3 (Default: Release Time)"               );  break;
      case 0x49:  name = qApp->tr("Sound Controller 4 (Default: Attack Time)"                );  break;
      case 0x4a:  name = qApp->tr("Sound Controller 5 (Default: Brightness)"                 );  break;
      case 0x4b:  name = qApp->tr("Sound Controller 6 (Default: Decay Time)"                 );  break;
      case 0x4c:  name = qApp->tr("Sound Controller 7 (Default: Vibrato Rate)"               );  break;
      case 0x4d:  name = qApp->tr("Sound Controller 8 (Default: Vibrato Depth)"              );  break;
      case 0x4e:  name = qApp->tr("Sound Controller 9 (Default: Vibrato Delay)"              );  break;
      case 0x4f:  name = qApp->tr("Sound Controller 10"                                      );  break;
      case 0x50:  name = qApp->tr("General Purpose Controller 5"                             );  break;
      case 0x51:  name = qApp->tr("General Purpose Controller 6"                             );  break;
      case 0x52:  name = qApp->tr("General Purpose Controller 7"                             );  break;
      case 0x53:  name = qApp->tr("General Purpose Controller 8"                             );  break;
      case 0x5b:  name = qApp->tr("Effects 1 Depth (Default: Reverb Send Level)"             );  break;
      case 0x5c:  name = qApp->tr("Effects 2 Depth"                                          );  break;
      case 0x5d:  name = qApp->tr("Effects 3 Depth (Default: Chorus Send Level"              );  break;
      case 0x5e:  name = qApp->tr("Effects 4 Depth"                                          );  break;
      case 0x5f:  name = qApp->tr("Effects 5 Depth"                                          );  break;
      case 0x60:  name = qApp->tr("Data Increment"                                           );  break;
      case 0x61:  name = qApp->tr("Data Decrement"                                           );  break;
      case 0x62:  name = qApp->tr("Non-Registered Parameter Number LSB"                      );  break;
      case 0x63:  name = qApp->tr("Non-Registered Parameter Number MSB"                      );  break;
      case 0x64:  name = qApp->tr("Registered Parameter Number LSB"                          );  break;
      case 0x65:  name = qApp->tr("Registered Parameter Number MSB"                          );  break;
      case 0x78:  name = qApp->tr("All Sound Off"                                            );  break;
      case 0x79:  name = qApp->tr("Reset All Controllers"                                    );  break;
      case 0x7a:  name = qApp->tr("Local Control On/Off"                                     );  break;
      case 0x7b:  name = qApp->tr("All Notes Off"                                            );  break;
      case 0x7c:  name = qApp->tr("Omni Mode Off"                                            );  break;
      case 0x7d:  name = qApp->tr("Omni Mode On"                                             );  break;
      case 0x7e:  name = qApp->tr("Mono Mode On"                                             );  break;
      case 0x7f:  name = qApp->tr("Poly Mode On"                                             );  break;
      default:    name = qApp->tr("Undefined"                                                );  break;
    }
  }
  return qApp->translate("midiControlFormat", "%1 - %2").arg(QLocale::system().toString(i_CC), name);
}

QString  getMIDINoteString(quint8 i_note) {
  QString name;

  if((quint8)i_note >= (quint8)0x80) {
    name = qApp->tr("ERROR: DATA is actually a STATUS");
  }
  else {
    switch (i_note % 12) {
      case  0:  name = qApp->translate("cNote",      "C" );  break;
      case  1:  name = qApp->translate("cSharpNote", "C#");  break;
      case  2:  name = qApp->translate("dNote",      "D" );  break;
      case  3:  name = qApp->translate("dSharpNote", "D#");  break;
      case  4:  name = qApp->translate("eNote",      "E" );  break;
      case  5:  name = qApp->translate("fNote",      "F" );  break;
      case  6:  name = qApp->translate("fSharpNote", "F#");  break;
      case  7:  name = qApp->translate("gNote",      "G" );  break;
      case  8:  name = qApp->translate("gSharpNote", "G#");  break;
      case  9:  name = qApp->translate("aNote",      "A" );  break;
      case 10:  name = qApp->translate("aSharpNote", "A#");  break;
      case 11:  name = qApp->translate("bNote",      "B" );  break;
    }
  }
  QLocale locale = QLocale::system();
  return qApp->translate("midiNoteFormat", "%1 (%2%3)").arg(locale.toString(static_cast<int>(i_note)), name, locale.toString(static_cast<int>(i_note / 12) - 1));
}

QWidget *loadForm(const QString &path, QWidget *parent) {
  QFile file(path);
  bool opened;

// FIXME Qt makes a jump based on uninitialized values the first time this call is made.
  opened = file.open(QFile::ReadOnly);
  assert(opened);

  QUiLoader uiLoader;

// FIXME Qt makes a jump based on uninitialized values the first time this call is made.
  QWidget *widget = uiLoader.load(&file, parent);
  assert(widget);

  file.close();
  return widget;
}


void    TimeUsToStrSec (quint64 i_llTim, char *o_tStr) {
  llong llSec;
  llong llUSec;
  llong llTmp;
  llong pStr;

  strcpy(o_tStr, "000000.000000");
  pStr = strlen(o_tStr) - 1;

  llUSec = i_llTim % 1000000;
  llSec  = i_llTim - llUSec;
  llSec  = i_llTim / 1000000;
  for(uint i=0; i<6; i++) {
    llTmp = llUSec % 10;
    o_tStr[pStr--] = (char)('0' + llTmp);
    llUSec -= llTmp;
    llUSec /= 10;
  }
  pStr --; // get past the period
  for(uint i=0; i<6; i++) {
    llTmp = llSec % 10;
    o_tStr[pStr--] = (char)('0' + llTmp);
    llSec -= llTmp;
    llSec /= 10;
  }
  return;
}
void    ByteToString(uint i_byte, char *o_str) {
  o_str[0] = hexAscii[((i_byte & 0x000000F0U) >> 4)];
  o_str[1] = hexAscii[((i_byte & 0x0000000FU) >> 0)];
  o_str[2] = '\0';
  return;
}
void    BytesToString(uint i_N, uint *i_bytes, char *o_str) {
  char tStr[4]; // How's this for super tweaky fast without strings.
  o_str[0] = '\0';
  for(uint i=0; i<i_N; i++) {
    ByteToString(i_bytes[i], tStr);
    o_str[3*i + 0] = tStr[0];
    o_str[3*i + 1] = tStr[1];
    o_str[3*i + 2] = ' ';
  }
  o_str[3*i_N] = '\0';
  return;
}
ullong  GetTS() {
  struct timeval tv;
  ullong  TS;

  gettimeofday(&tv, NULL);
  TS = tv.tv_sec * 1000000 + tv.tv_usec;
  return TS;
}
