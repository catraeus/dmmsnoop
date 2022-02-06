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

#ifndef __MIDI_HPP_
#define __MIDI_HPP_

#include <stdint.h>

#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QObject>

#include "util/DmmTypes.hpp"
#include "util/DmmStr.hpp"
#include "util/TrMsg.hpp"


class Midi: public QObject {
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
      char err  [32] ; // Likewise
    };
              Midi();
             ~Midi();
    void      Parse(uint i_N, uint *i_bytes);
    sMsgSpec            *theMS;
  private:
    TrMsg               *theTrMsg;
    static const ullong  lenForStat[];
    static const ullong  lenForSys[];
    uint                 bStat;
    uint                 bStatBase;
    uint                 bStatSub;
};

#endif
