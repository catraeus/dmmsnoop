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

#if 0
#define F_PRINTF fprintf
#define F_FLUSH  fflush
#else
#define F_PRINTF (void)
#define F_FLUSH  (void)
#endif

const ullong Midi::lenForStat[] = {3,    3, 3, 3, 2, 2, 3, 0};
const ullong Midi::lenForSys [] = {1024, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

Midi:: Midi() {
  theMS    = new sMsgSpec;
  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
}
Midi::~Midi(){}


void Midi::Parse(uint i_len, uint *i_bytes) {

  valid = false;
  len = i_len;
  theMS->TS[0]   = '\0';
  sprintf(theMS->len, "%d", len);
  theMS->raw[0]  = '\0';
  theMS->stat[0] = '\0';
  theMS->ch[0]   = '\0';
  theMS->vel[0]  = '\0';
  theMS->cc[0]   = '\0';
  theMS->prog[0] = '\0';
  theMS->bend[0] = '\0';
  theMS->sys[0]  = '\0';
  strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));

  CheckErrors(i_bytes);
  if(!valid) return;
  if(!((bStatBase == TrMsg::DES_SYSTEM) && (bStatSub == TrMsg::DEY_SYSEX))) { // We can get on with it
  }
  F_PRINTF(stdout, "good so far.\n"); F_FLUSH(stdout);
  return;
}
void Midi::CheckErrors(uint *i_bytes) {
  uint lenSpec;

//==== Remember, order of evaluation matters, e.g. End without SysEx is more important than data byte count.
//---- Zero Length
  if(len == 0) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
    F_PRINTF(stdout, "ErrChk --   %s\n", theMS->err); F_FLUSH(stdout);
    valid = false;
    return;
  }
  bStat = (uint)i_bytes[0];
//==== Good enough to parse the byte string.
  BytesToString(len, i_bytes, theMS->raw);
//---- Status has MSB marker
  if(bStat < 0x80U) { // FIXME no protection against blobs (giant messages.)
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_LOW));
    F_PRINTF(stdout, "ErrChk --   %s  --  %s\n", theMS->err, theMS->raw); F_FLUSH(stdout);
    valid = false;
    return;
  }
//====  OK, we're good enough to distribute the Status byte -- Status and [System Command or Channel Number].
  bStatBase = (bStat >> 4) & 0x07U;
  bStatSub  = (bStat >> 0) & 0x0FU;
//---- Oops, Status IS SysEx End
  if(bStat == MCC_SYSEX_END) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_SYSEND_START));
    F_PRINTF(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
    valid = false;
    return;
  }
//---- OK, now we can confidently put strings where they belong.
  strcpy(theMS->stat, theTrMsg->MsgMiStatGet((TrMsg::eStatType)bStatBase)); // MAGICK the enum is forced to be aligned with the MIDI spec.
  if(bStatBase == TrMsg::DES_SYSTEM) {
    strcpy(theMS->sys, theTrMsg->MsgMiSysGet((TrMsg::eSysType)bStatSub));
    lenSpec = lenForSys[bStatSub];
  }
  else {
    sprintf(theMS->ch, "%d", bStatSub);
    lenSpec = lenForStat[bStatBase];
  }
//---- Dispatch with SysEx, good or bad.
  //F_PRINTF(stdout, "DEBUG: bStat-%02X-\n", bStat); F_FLUSH(stdout);
  if(bStat == MCC_SYSEX) {
    //---- Too short
    if((len < MCC_SYSEX_MIN) || (len > lenForSys[bStatSub])) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_SYS));
      F_PRINTF(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
      valid = false;
      return;
    }
    //---- doesn't have an END byte at the, well ... the end
    if(i_bytes[len - 1] != MCC_SYSEX_END) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_SYSEX_TRUNC));
      F_PRINTF(stdout, "ErrChk --   %s  --  %s  --  %s\n", theMS->err, theMS->stat, theMS->raw); F_FLUSH(stdout);
      valid = false;
      return;
    }
    //---- One of the middle bytes is greater than 0x80
    for(uint i=1; i<len - 1; i++) { // skip the Status byte and stop before the END byte.
      if(i_bytes[i] >= 0x80U) {
        strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_DATA_HIGH));
        F_PRINTF(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
        valid = false;
        return;
      } // Good Low Byte
    }
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));
    F_PRINTF(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
    valid = true;
    return;
  } // End of SysEx
//---- Check for length
  if(len != lenSpec) {
    //F_PRINTF(stdout, "DEBUG: len-%d-\n", len); F_FLUSH(stdout);
    if(bStatBase != TrMsg::DES_SYSTEM)
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_STAT));
    else
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_SYS));
    F_PRINTF(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
    valid = false;
    return;
  } // Good Low Byte
//---- Data Byte >= 0x80 AND These damned SysEx messages are going to be the death of me.
  for(uint i=1; i<len; i++) { // Don't forget to skip the Status byte.  It accidenally doesn't check if there aren't any :-)
    if(i_bytes[i] >= 0x80U) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_DATA_HIGH));
      F_PRINTF(stdout, "ErrChk --   %s  --  %s :: %s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); F_FLUSH(stdout);
      valid = false;
      return;
    } // Good Low Byte
  }
//---- WHOAH!  We're done.
  strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));
  valid = true;
  return;
}
