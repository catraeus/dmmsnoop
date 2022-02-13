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
const char  *Midi::sprintfMTC[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                   "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                   "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
                                   "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
                                   "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
                                   "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
                                   "60", "61", "62", "63", "64" };
const char  *Midi::sprintfFrm[] = {"24.", "26.", "29D", "30."};
const char   Midi::mtcBlank[]   = "";
const char   Midi::mtcBase[]    = "..:..:..|.. rrd";
const ulong  Midi::mtcFrmLims[] = {23, 24, 28, 29};

Midi:: Midi() {
  theMS       = new sMsgSpec;
  theTrMsg    = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
  theMS->mtc  = (char *)mtcBlank;
  strcpy(mtcWorking, mtcBase);
  mtcBseqPrev = MTQ_HRSH; // Make believe we are starting from scratch
  mtcFps      = MTF_24;
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
  theMS->mtc     = (char *)mtcBlank;
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
      // We escaped this long ago.
      break;
    case TrMsg::DEY_MTC_QF:
      ParseTimeCode(i_bytes[1]);
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
void Midi::ParseTimeCode(uint i_TC) {
  theMS->mtc = mtcWorking;
  mtcFrmMax = mtcFrmLims[mtcFps];
  //====  Just parse out everything and let the case statements simplify without the shif/mask confusion.
                                                                         fprintf(stdout, "#=============\n"        ); fflush(stdout);
  mtcBseq = (eMtcSeq)((i_TC    & MTP_SEQ_MSK       ) >> MTP_SEQ_SHR   ); fprintf(stdout, "mtcBseq: %02X\n", mtcBseq); fflush(stdout);
  mtcBhlf =           (mtcBseq & MTP_HBYT_MSK      )                   ; fprintf(stdout, "mtcBhlf: %02X\n", mtcBhlf); fflush(stdout);
  mtcBnum =           (i_TC    & MTP_VALUE_MSK     )                   ; fprintf(stdout, "mtcBnum: %02X\n", mtcBnum); fflush(stdout);
  mtcFrmL =           (mtcBnum & MTP_FRM_MSK_LO    )                   ; fprintf(stdout, "mtcFrmL: %02X\n", mtcFrmL); fflush(stdout);
  mtcFrmH =           (mtcBnum & MTP_FRM_MSK_HI    ) << MTP_VALHI_SHL  ; fprintf(stdout, "mtcFrmH: %02X\n", mtcFrmH); fflush(stdout);
  mtcSecL =           (mtcBnum & MTP_SECMIN_MSK_LO )                   ; fprintf(stdout, "mtcSecL: %02X\n", mtcSecL); fflush(stdout);
  mtcSecH =           (mtcBnum & MTP_SECMIN_MSK_HI ) << MTP_VALHI_SHL  ; fprintf(stdout, "mtcSecH: %02X\n", mtcSecH); fflush(stdout);
  mtcMinL =           (mtcBnum & MTP_SECMIN_MSK_LO )                   ; fprintf(stdout, "mtcMinL: %02X\n", mtcMinL); fflush(stdout);
  mtcMinH =           (mtcBnum & MTP_SECMIN_MSK_HI ) << MTP_VALHI_SHL  ; fprintf(stdout, "mtcMinH: %02X\n", mtcMinH); fflush(stdout);
  mtcHrsL =           (mtcBnum & MTP_HRS_MSK_LO    )                   ; fprintf(stdout, "mtcHrsL: %02X\n", mtcHrsL); fflush(stdout);
  mtcHrsH =           (mtcBnum & MTP_HRS_MSK_HI    ) << MTP_VALHI_SHL  ; fprintf(stdout, "mtcHrsH: %02X\n", mtcHrsH); fflush(stdout);
  mtcFpsL = (eMtcFrm)((mtcBnum & MTP_FRMRT_MSK     ) >> MTP_FRMRT_SHR ); fprintf(stdout, "mtcFps : %02X\n", mtcFps ); fflush(stdout);

  switch(mtcBseq) {
    //==============================================================================================
    case MTQ_FRML:
      switch (mtcBseqPrev) {
        case MTQ_FRMH: // Downward, sum & print
          mtcFrm    = mtcFrm + mtcFrmL;
          if(mtcFrm > mtcFrmMax) mtcFrm = mtcFrmMax;
          mtcWorking[ 9] = sprintfMTC[mtcFrm][0]; mtcWorking[10] = sprintfMTC[mtcFrm][1];
          break;
        case MTQ_HRSH: // Upward, reset, but wait, this is upward across a boundary ... reset everything else to 0.
          strcpy(mtcWorking, mtcBase); mtcFrm = 0; mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcFrm =          mtcFrmL;
          mtcWorking[ 9] = sprintfMTC[mtcFrm][0]; mtcWorking[10] = sprintfMTC[mtcFrm][1];
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
      //--------------------------------------------------------------------------------------------
    case MTQ_FRMH:
      switch (mtcBseqPrev) {
        case MTQ_FRML: // Upward, sum & print
          mtcFrm = mtcFrm + mtcFrmH;
          if(mtcFrm > mtcFrmMax) mtcFrm = mtcFrmMax;
          mtcWorking[ 9] = sprintfMTC[mtcFrm][0]; mtcWorking[10] = sprintfMTC[mtcFrm][1];
          break;
        case MTQ_SECL: // Downward, reset
          mtcFrm = mtcFrmH;
          mtcWorking[ 9] = sprintfMTC[mtcFrm][0]; mtcWorking[10] = sprintfMTC[mtcFrm][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
    //==============================================================================================
    case MTQ_SECL:
      switch (mtcBseqPrev) {
        case MTQ_SECH: // Downward, sum & print
          mtcSec = mtcSec + mtcSecL;
          if(mtcSec > MTP_SECMIN_MAX) mtcSec = MTP_SECMIN_MAX;
          mtcWorking[ 6] = sprintfMTC[mtcSec][0]; mtcWorking[7] = sprintfMTC[mtcSec][1];
          break;
        case MTQ_FRMH: // Upward, reset
          mtcSec =          mtcSecL;
          mtcWorking[ 6] = sprintfMTC[mtcSec][0]; mtcWorking[7] = sprintfMTC[mtcSec][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
      //--------------------------------------------------------------------------------------------
    case MTQ_SECH:
      switch (mtcBseqPrev) {
        case MTQ_SECL: // Upward, sum & print
          mtcSec = mtcSec + mtcSecH;
          if(mtcSec > MTP_SECMIN_MAX) mtcSec = MTP_SECMIN_MAX;
          mtcWorking[ 6] = sprintfMTC[mtcSec][0]; mtcWorking[ 7] = sprintfMTC[mtcSec][1];
          break;
        case MTQ_MINL: // Downward, reset
          mtcSec = mtcSecH;
          mtcWorking[ 6] = sprintfMTC[mtcSec][0]; mtcWorking[ 7] = sprintfMTC[mtcSec][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
    //==============================================================================================
    case MTQ_MINL:
      switch (mtcBseqPrev) {
        case MTQ_MINH: // Downward, sum & print
          mtcMin = mtcMin + mtcMinL;
          if(mtcMin > MTP_SECMIN_MAX) mtcMin = MTP_SECMIN_MAX;
          mtcWorking[ 3] = sprintfMTC[mtcMin][0]; mtcWorking[4] = sprintfMTC[mtcMin][1];
          break;
        case MTQ_SECH: // Upward, reset
          mtcMin =          mtcMinL;
          mtcWorking[ 3] = sprintfMTC[mtcMin][0]; mtcWorking[4] = sprintfMTC[mtcMin][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcMin = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
      //--------------------------------------------------------------------------------------------
    case MTQ_MINH:
      switch (mtcBseqPrev) {
        case MTQ_MINL: // Upward, sum & print
          mtcMin = mtcMin + mtcMinH;
          if(mtcMin > MTP_SECMIN_MAX) mtcMin = MTP_SECMIN_MAX;
          mtcWorking[ 3] = sprintfMTC[mtcMin][0]; mtcWorking[ 4] = sprintfMTC[mtcMin][1];
          break;
        case MTQ_HRSL: // Downward, reset
          mtcMin = mtcMinH;
          mtcWorking[ 3] = sprintfMTC[mtcMin][0]; mtcWorking[ 4] = sprintfMTC[mtcMin][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcMin = 0; mtcMin = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
    //==============================================================================================
    case MTQ_HRSL:
      switch (mtcBseqPrev) {
        case MTQ_HRSH: // Downward, sum & print
          mtcHrs = mtcHrs + mtcHrsL;
          if(mtcHrs > MTP_HRS_MAX) mtcHrs = MTP_HRS_MAX;
          mtcWorking[ 0] = sprintfMTC[mtcHrs][0]; mtcWorking[1] = sprintfMTC[mtcHrs][1];
          break;
        case MTQ_MINH: // Upward, reset
          mtcHrs =          mtcHrsL;
          mtcWorking[ 0] = sprintfMTC[mtcHrs][0]; mtcWorking[1] = sprintfMTC[mtcHrs][1];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcHrs = 0; mtcHrs = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
      //--------------------------------------------------------------------------------------------
    case MTQ_HRSH:
      switch (mtcBseqPrev) {
        case MTQ_HRSL: // Upward, sum & print
          mtcHrs = mtcHrs + mtcHrsH;
          mtcFps = mtcFpsL;
          if(mtcHrs > MTP_HRS_MAX) mtcHrs = MTP_HRS_MAX;
          mtcWorking[ 0] = sprintfMTC[mtcHrs][0]; mtcWorking[ 1] = sprintfMTC[mtcHrs][1];
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
        case MTQ_FRML: // Downward, reset but wait, this is a boundary cross, reset everything
          strcpy(mtcWorking, mtcBase); mtcFrm = 0; mtcSec = 0; mtcMin = 0; mtcHrs = 0;
          mtcHrs = mtcHrsH;
          mtcFps = mtcFpsL;
          mtcWorking[ 0] = sprintfMTC[mtcHrs][0]; mtcWorking[ 1] = sprintfMTC[mtcHrs][1];
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
        default:  // cough and spit
          strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcHrs = 0; mtcHrs = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
    //==============================================================================================
    default      :  // cough and spit more
      strcpy(theMS->err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
      strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
      mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
      break;
  }
  mtcBseqPrev = mtcBseq;
  return;
}
