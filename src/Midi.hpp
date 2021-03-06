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


#include "util/DmmTypes.hpp"
#include "util/DmmStr.hpp"
#include "util/TrMsg.hpp"

#include "MtcQf.hpp"


class Midi {
  public:
    struct sMsgSpec { // all are null-terminated ASCII c strings.  Whatever the calc is, I'll take it to a next 2^k
      char  TS   [16] ; // hh:mm:ss.sss or ssssss.sss (one day is 86400 sec so I figure 999999.999 sec should cover most sessions.)
      char  len  [ 4] ; // If we get a SysEx infinitely long blob, I'll just truncate and fake it.
      char  raw  [64] ; // ALL normal MIDI messages are 1, 2 or 3 bytes.  This will put out nn nnn nnn as hex dec dec
      char  stat [32] ; // If I can't make a reasonable human message in 32 characters, then something is wrong.
      char  ch   [ 4] ; // There are at most 16 channels possible printed as decimal
      char  note [ 8] ; // two decimal key num, space, 8va, Note, mod
      char  vel  [ 4] ; // for notes, controls and pressures.  These are 0 to 127 and will be printed decimal.
      char  cc   [ 4] ; // There are only 128 channels possible.  Print decimal.  FIXME But, there are LSB/MSB channel pairs to figure.
      char  prog [ 4] ; // 128 programs possible.
      char  bend [16] ; // Is a Speshul uint16_t
      char  pos  [16] ; // Is a Speshul uint16_t song position
      char  song [ 4] ; // 128 possible song numbers.
      char *mtc       ; // Special since non-MTC needs blank and MTC needs a built mtcWorking as hh:mm:ss:ff ccd will put a * into it when it is incomplete.
      char  sys  [32] ; // Same sentiment as stat above
      char  err  [64] ; // Likewise
    };
  private:
    enum eConst {
      MCC_SYSEX_MIN = 0x00000003U, // The smallest possible.  Status, 1 Mfr ID and SysEx-End
      MCC_SYSEX     = 0x000000F0U, // MAGICK to make the start detect faster.
      MCC_SYSEX_END = 0x000000F7U, // MAGICK to make the end detect faster.
      MCC_NUM       = 0x00000001U,
      MCC_BIG       = 0xFFFFFFFFU
    };
  public:
              Midi();
             ~Midi();
    void      Parse(uint i_len, uint *i_bytes);
    bool      GetValid(void) {return valid;};
    sMsgSpec            *theMS;
  private:
      void CheckErrors    (uint *i_bytes); // Also distributes Status, Sys but not Channel, Velocity etc.
      void ParseController(uint *i_bytes);
      void ParseSystem    (uint *i_bytes);
  public:
  private:
                 MtcQf  *theMtcQf;
                 TrMsg  *theTrMsg;
    static const ullong  lenForStat[];
    static const ullong  lenForSys[];
                 uint    len;
                 uint    bStat;
                 uint    bStatBase;
                 uint    bStatSub;
                 uint    bChNo;
                 uint    bSysNo;
                 uint    bNoteNo;
                 uint    vVelNo;
                 bool    valid;
};

#endif
