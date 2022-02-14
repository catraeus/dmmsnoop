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
#include <unistd.h>

#include "Midi.hpp"


const ullong Midi::lenForStat[] = {3,    3, 3, 3, 2, 2, 3, 0};
const ullong Midi::lenForSys [] = {1024, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
Midi:: Midi() {
  theMS       = new sMsgSpec;
  theTrMsg    = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
  theMtcQf    = new MtcQf(&(theMS->mtc), theMS->err);
}
Midi::~Midi(){}


void Midi::Parse(uint i_len, uint *i_bytes) {

  valid = false;
  len = i_len;
  theMS->TS  [0] = '\0';
  sprintf(theMS->len, "%d", len);
  theMS->raw [0] = '\0';
  theMS->stat[0] = '\0';
  theMS->ch  [0] = '\0';
  theMS->note[0] = '\0';
  theMS->vel [0] = '\0';
  theMS->cc  [0] = '\0';
  theMS->prog[0] = '\0';
  theMS->bend[0] = '\0';
  theMS->pos [0] = '\0';
  theMS->song[0] = '\0';
  theMS->sys [0] = '\0';
  theMS->err [0] = '\0';
  strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));

  CheckErrors(i_bytes);
  //So it is either:
  //    invalid, go home and print as best as possible.
  //    A SysEx that is well formed, likewise go home.
  //    Needs the rest of the standard stuff filled in.
  //But we know the following is already valid
  //    raw
  //    stat
  //    sys
  //    len
  if(!valid) return;
  if(!((bStatBase == TrMsg::DES_SYSTEM) && (bStatSub == TrMsg::DEY_SYSEX))) { // We can get on with it
    if(bStatBase < TrMsg::DES_SYSTEM)    ParseController(i_bytes);
    else                                 ParseSystem    (i_bytes);
  }
  return;
}
void Midi::CheckErrors(uint *i_bytes) {
  uint lenSpec;

//==== Remember, order of evaluation matters, e.g. End without SysEx is more important than data byte count.
//---- Zero Length
  if(len == 0) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_ZRO_LEN));
  //fprintf(stdout, "ErrChk --   %s\n", theMS->err); fflush(stdout);
    valid = false;
    return;
  }
  bStat = (uint)i_bytes[0];
//==== Good enough to parse the byte string.
  BytesToString(len, i_bytes, theMS->raw);
//---- Status has MSB marker
  if(bStat < 0x80U) { // FIXME no protection against blobs (giant messages.)
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_STAT_LOW));
  //fprintf(stdout, "ErrChk --   %s  --  %s\n", theMS->err, theMS->raw); fflush(stdout);
    valid = false;
    return;
  }
//====  OK, we're good enough to distribute the Status byte -- Status and [System Command or Channel Number].
  bStatBase = (bStat >> 4) & 0x07U;
  bStatSub  = (bStat >> 0) & 0x0FU;
//---- Oops, Status IS SysEx End
  if(bStat == MCC_SYSEX_END) {
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_SYSEND_START));
  //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
    valid = false;
    return;
  }
//---- OK, now we can confidently put strings where they belong.
  strcpy(theMS->stat, theTrMsg->MsgMiStatGet((TrMsg::eStatType)bStatBase)); // MAGICK the enum is forced to be aligned with the MIDI spec.
  if(bStatBase == TrMsg::DES_SYSTEM) {
    strcpy(theMS->sys, theTrMsg->MsgMiSysGet((TrMsg::eSysType)bStatSub));
    lenSpec = lenForSys[bStatSub];
    //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
  }
  else {
    sprintf(theMS->ch, "%d", bStatSub);
    lenSpec = lenForStat[bStatBase];
  }
//---- Dispatch with SysEx, good or bad.
  //fprintf(stdout, "DEBUG: bStat-%02X-\n", bStat); fflush(stdout);
  if(bStat == MCC_SYSEX) {
    //---- Too short
    if((len < MCC_SYSEX_MIN) || (len > lenForSys[bStatSub])) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_SYS));
    //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
      valid = false;
      return;
    }
    //---- doesn't have an END byte at the, well ... the end
    if(i_bytes[len - 1] != MCC_SYSEX_END) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_SYSEX_TRUNC));
    //fprintf(stdout, "ErrChk --   %s  --  %s  --  %s\n", theMS->err, theMS->stat, theMS->raw); fflush(stdout);
      valid = false;
      return;
    }
    //---- One of the middle bytes is greater than 0x80
    for(uint i=1; i<len - 1; i++) { // skip the Status byte and stop before the END byte.
      if(i_bytes[i] >= 0x80U) {
        strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_DATA_HIGH));
      //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
        valid = false;
        return;
      } // Good Low Byte
    }
    strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));
  //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
    valid = true;
    return;
  } // End of SysEx
//---- Check for length
  if(len != lenSpec) {
    //fprintf(stdout, "DEBUG: len-%d-\n", len); fflush(stdout);
    if(bStatBase != TrMsg::DES_SYSTEM)
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_STAT));
    else
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_LEN_SYS));
  //fprintf(stdout, "ErrChk --   %s  --  %s::%s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
    valid = false;
    return;
  } // Good Low Byte
//---- Data Byte >= 0x80 AND These damned SysEx messages are going to be the death of me.
  for(uint i=1; i<len; i++) { // Don't forget to skip the Status byte.  It accidenally doesn't check if there aren't any :-)
    if(i_bytes[i] >= 0x80U) {
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_DATA_HIGH));
    //fprintf(stdout, "ErrChk --   %s  --  %s :: %s  --  %s\n", theMS->err, theMS->stat, theMS->sys, theMS->raw); fflush(stdout);
      valid = false;
      return;
    } // Good Low Byte
  }
//---- WHOAH!  We're done.
  strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_NONE));
  valid = true;
  return;
}
void Midi::ParseController(uint *i_bytes) {
  uint bendM;
  uint bendL;
  int  bend;

  bChNo = bStatSub;
  sprintf(theMS->ch, "%d", bChNo + 1); // Everyone below System Fx has a channel
  switch(bStatBase) {
    case TrMsg::DES_NOTE_OFF:
    case TrMsg::DES_NOTE_ON:
    case TrMsg::DES_POLY_PRES:
      sprintf(theMS->note, "%3d", i_bytes[1]);
      sprintf(theMS->vel,  "%3d", i_bytes[2]);
      break;
    case TrMsg::DES_CC:
      sprintf(theMS->cc,   "%3d", i_bytes[1] + 1);
      sprintf(theMS->vel,  "%3d", i_bytes[2]);
      break;
    case TrMsg::DES_PROG_CHNG:
      sprintf(theMS->prog, "%3d", i_bytes[1]    ); // It is so weird where MIDI does 0 based and 1 based ... sometimes yes, sometimes no
      break;
    case TrMsg::DES_CHAN_PRES:
      sprintf(theMS->vel,  "%3d", i_bytes[1]    );
      break;
    case TrMsg::DES_PITCH_BEND:
      bendM = i_bytes[2];
      bendL = i_bytes[1];
      bendM <<= 7;
      bendM += bendL;
      bend = (int)bendM;
      bend -= 8192;
      sprintf(theMS->bend,  "%5d", bend    );
      break;
    default:
      break;
  }
  return;
}
void Midi::ParseSystem(uint *i_bytes) {
  uint posM;
  uint posL;
  int  pos;

  bSysNo = bStatSub;

  switch(bSysNo) {
    case TrMsg::DEY_SYSEX:
      // We dealt with this long ago.
      break;
    case TrMsg::DEY_MTC_QF:
      theMtcQf->Parse(i_bytes[1]);
      break;
    case TrMsg::DEY_SONG_POS:
      posM = i_bytes[2];
      posL = i_bytes[1];
      posM <<= 7;
      posM += posL;
      pos = (int)posM;
      sprintf(theMS->pos,  "%5d", pos    );
      break;
    case TrMsg::DEY_SONG_SEL:
      posM = i_bytes[1];
      sprintf(theMS->song, "%d", posM);
      strcpy(theMS->sys, theTrMsg->MsgMiSysGet(TrMsg::DEY_SONG_SEL));
      break;
    case TrMsg::DEY_UNDEF_4:
      strcpy(theMS->sys, theTrMsg->MsgMiSysGet(TrMsg::DEY_UNDEF_4));
      break;
    case TrMsg::DEY_UNDEF_5:
      strcpy(theMS->sys, theTrMsg->MsgMiSysGet(TrMsg::DEY_UNDEF_5));
      break;
    case TrMsg::DEY_TUNE_REQ:
      break;
    case TrMsg::DEY_END_SYSEX:
      // Likewise this is long gone.
      break;
    case TrMsg::DEY_TIM_CLK:
      break;
    case TrMsg::DEY_UNDEF_9:
      strcpy(theMS->sys, theTrMsg->MsgMiSysGet(TrMsg::DEY_UNDEF_9));
      break;
    case TrMsg::DEY_SONG_START:
      break;
    case TrMsg::DEY_SONG_CONT:
      break;
    case TrMsg::DEY_SONG_STOP:
      break;
    case TrMsg::DEY_UNDEF_D:
      strcpy(theMS->sys, theTrMsg->MsgMiSysGet(TrMsg::DEY_UNDEF_D));
      break;
    case TrMsg::DEY_ACT_SENS:
      break;
    case TrMsg::DEY_RESET:
      break;
    default:
      break;
  }
  return;
}
