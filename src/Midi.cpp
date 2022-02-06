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
#include <sys/time.h>

#include "Midi.hpp"


const ullong Midi::lenForStat[] = {3,    3, 3, 3, 2, 2, 3, 0};
const ullong Midi::lenForSys [] = {1024, 2, 3, 2, 1, 1, 1, 1};

Midi:: Midi() {
  theMS    = new sMsgSpec;
  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
}
Midi::~Midi(){}


void Midi::Parse(uint i_N, uint *i_bytes) {
  uint lenSpec;

  theMS->TS[0]   = '\0';
  sprintf(theMS->len, "%d", i_N);
  theMS->raw[0]  = '\0';
  theMS->stat[0] = '\0';
  theMS->ch[0]   = '\0';
  theMS->vel[0]  = '\0';
  theMS->cc[0]   = '\0';
  theMS->prog[0] = '\0';
  theMS->bend[0] = '\0';
  theMS->sys[0]  = '\0';
  theMS->err[0]  = '\0';
//=================================================================
//==== Meta
//---- Zero Length
  if(i_N == 0) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
    fprintf(stdout, "Parser --   %s\n", theMS->err);
    return;
  }
  bStat = (uint)i_bytes[0];
//---- Status has MSB marker
  BytesToString(i_N, i_bytes, theMS->raw);
  if(bStat < 0x80U) { // FIXME no protection against blobs (giant messages.)
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_LOW));
    fprintf(stdout, "Parser --   %s  --  %s\n", theMS->err, theMS->raw);
    return;
  }
//---- Length appropriate to Status, but escape for Sys messages
  bStatBase = (bStat >> 4) & 0x07U;
  bStatSub  = (bStat >> 0) & 0x0FU;
  lenSpec   = lenForStat[bStatBase];
  strcpy(theMS->stat, theTrMsg->MsgMiStatGet((TrMsg::eStatType)bStatBase)); // MAGICK the enum is forced to be aligned with the MIDI spec.
  if((lenSpec != i_N) && (bStatBase < 0x0FU)) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_STAT));
    fprintf(stdout, "Parser --   %s  --  %s  --  %s\n", theMS->err, theMS->stat, theMS->raw);
    return;
  }
  return;
}
