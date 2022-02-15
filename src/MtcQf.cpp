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

#include "MtcQf.hpp"


const char  *MtcQf::sprintfMTC[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
                                    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
                                    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
                                    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
                                    "60", "61", "62", "63", "64" };
const char  *MtcQf::sprintfFrm[] = {"24.", "26.", "29D", "30."};
const char   MtcQf::mtcBlank  [] = "";
const char   MtcQf::mtcBase   [] = "..:..:..|.. rrd";
const ulong  MtcQf::mtcFrmLims[] = {23, 24, 28, 29};

MtcQf:: MtcQf(char **i_mtc, char *i_err) {
  mtc         = i_mtc;
  err         = i_err;
  theTrMsg    = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
 *mtc        = (char *)mtcBlank;
  strcpy(mtcWorking, mtcBase);
  mtcBseqPrev = MTQ_HRSH; // Make believe we are starting from scratch
  mtcFps      = MTF_24;
}
MtcQf::~MtcQf(){}


void MtcQf::Parse(uint i_TC) {
  *mtc = mtcWorking;
  mtcFrmMax = mtcFrmLims[mtcFps];
  //====  Just parse out everything and let the case statements simplify without the shif/mask confusion.
                                                                         //fprintf(stdout, "#=============\n"        ); fflush(stdout);
  mtcBseq = (eMtcSeq)((i_TC    & MTP_SEQ_MSK       ) >> MTP_SEQ_SHR   ); //fprintf(stdout, "mtcBseq: %02X\n", mtcBseq); fflush(stdout);
  mtcBhlf =           (mtcBseq & MTP_HBYT_MSK      )                   ; //fprintf(stdout, "mtcBhlf: %02X\n", mtcBhlf); fflush(stdout);
  mtcBnum =           (i_TC    & MTP_VALUE_MSK     )                   ; //fprintf(stdout, "mtcBnum: %02X\n", mtcBnum); fflush(stdout);
  mtcFrmL =           (mtcBnum & MTP_FRM_MSK_LO    )                   ; //fprintf(stdout, "mtcFrmL: %02X\n", mtcFrmL); fflush(stdout);
  mtcFrmH =           (mtcBnum & MTP_FRM_MSK_HI    ) << MTP_VALHI_SHL  ; //fprintf(stdout, "mtcFrmH: %02X\n", mtcFrmH); fflush(stdout);
  mtcSecL =           (mtcBnum & MTP_SECMIN_MSK_LO )                   ; //fprintf(stdout, "mtcSecL: %02X\n", mtcSecL); fflush(stdout);
  mtcSecH =           (mtcBnum & MTP_SECMIN_MSK_HI ) << MTP_VALHI_SHL  ; //fprintf(stdout, "mtcSecH: %02X\n", mtcSecH); fflush(stdout);
  mtcMinL =           (mtcBnum & MTP_SECMIN_MSK_LO )                   ; //fprintf(stdout, "mtcMinL: %02X\n", mtcMinL); fflush(stdout);
  mtcMinH =           (mtcBnum & MTP_SECMIN_MSK_HI ) << MTP_VALHI_SHL  ; //fprintf(stdout, "mtcMinH: %02X\n", mtcMinH); fflush(stdout);
  mtcHrsL =           (mtcBnum & MTP_HRS_MSK_LO    )                   ; //fprintf(stdout, "mtcHrsL: %02X\n", mtcHrsL); fflush(stdout);
  mtcHrsH =           (mtcBnum & MTP_HRS_MSK_HI    ) << MTP_VALHI_SHL  ; //fprintf(stdout, "mtcHrsH: %02X\n", mtcHrsH); fflush(stdout);
  mtcFpsL = (eMtcFrm)((mtcBnum & MTP_FRMRT_MSK     ) >> MTP_FRMRT_SHR ); //fprintf(stdout, "mtcFps : %02X\n", mtcFps ); fflush(stdout);

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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
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
          strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
          strcpy(mtcWorking, mtcBase); mtcHrs = 0; mtcHrs = 0; mtcHrs = 0;
          mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
          break;
      }
      break;
    //==============================================================================================
    default      :  // cough and spit more
      strcpy(err, theTrMsg->MsgMiMetaGet(TrMsg::DEM_MTC_OOO));
      strcpy(mtcWorking, mtcBase); mtcSec = 0; mtcMin = 0; mtcHrs = 0;
      mtcWorking[12] = sprintfFrm[mtcFps][0]; mtcWorking[13] = sprintfFrm[mtcFps][1]; mtcWorking[14] = sprintfFrm[mtcFps][2];
      break;
  }
  mtcBseqPrev = mtcBseq;
  return;
}
