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

#ifndef __MTC_QF_HPP_
#define __MTC_QF_HPP_

#include <stdint.h>

#include "util/DmmTypes.hpp"
#include "util/DmmStr.hpp"
#include "util/TrMsg.hpp"

class MtcQf {
  public:
  private:
    enum eMtcParse {
      MTP_SEQ_MSK        = 0x00000070U, // The Sequence is here, there are only 8 of them.
      MTP_SEQ_SHR        = 0x00000004U, // The Sequence shifts this much to make it zero-based
      MTP_HBYT_MSK       = 0x00000001U, // The half-byte marker is here (after bringing down the Sequence)
      MTP_VALUE_MSK      = 0x0000000FU, // The starting point for the numbers.
      MTP_VALHI_SHL      = 0x00000004U, // All numbers, the high half goes left by this many bits
      MTP_FRM_MSK_LO     = 0x0000000FU, // Frames      in the high half can only be 1 bit
      MTP_FRM_MSK_HI     = 0x00000001U, // Frames      in the high half can only be 4 bit
      MTP_SECMIN_MSK_LO  = 0x0000000FU, // Sec and Min in the low  half can only be 2 bit
      MTP_SECMIN_MSK_HI  = 0x00000003U, // Sec and Min in the high half can only be 4 bit
      MTP_SECMIN_MAX     = 0x0000003BU, // 59 zero based.
      MTP_HRS_MSK_LO     = 0x0000000FU, // Hours       in the low  half can only be 4 bit
      MTP_HRS_MSK_HI     = 0x00000001U, // Hours       in the high half can only be 1 bit
      MTP_HRS_MAX        = 0x00000017U, // 23 hrs in a 24-hr clock
      MTP_FRMRT_MSK      = 0x00000006U, // The location of the Rate enum.
      MTP_FRMRT_SHR      = 0x00000001U  // The shift-down of the Rate enum.
    };
    enum eMtcSeq {
      MTQ_FRML = 0x00000000U,
      MTQ_FRMH = 0x00000001U,
      MTQ_SECL = 0x00000002U,
      MTQ_SECH = 0x00000003U,
      MTQ_MINL = 0x00000004U,
      MTQ_MINH = 0x00000005U,
      MTQ_HRSL = 0x00000006U,
      MTQ_HRSH = 0x00000007U,
      MTQ_BIG  = 0xFFFFFFFFU
    };
    enum eMtcFrm {
      MTF_24  = 0x00000000U,
      MTF_25  = 0x00000001U,
      MTF_29D = 0x00000002U,
      MTF_30  = 0x00000003U,
      MTF_BIG = 0xFFFFFFFFU
    };
  public:
              MtcQf(char **i_mtc, char *i_err);
             ~MtcQf();
    void      Parse(uint i_TC);
    bool      GetValid(void) {return valid;};
  private:
  public:
  private:
                 TrMsg  *theTrMsg;
                 char  **mtc;
                 char   *err;
    static const char   *sprintfMTC[];
    static const char   *sprintfFrm[];
    static const char    mtcBlank[];
    static const char    mtcBase[];
    static const ulong   mtcFrmLims[];
                 bool    valid;

                 char    mtcWorking[32];
                 eMtcSeq mtcBseq;
                 eMtcSeq mtcBseqPrev;
                 uint    mtcBhlf;
                 uint    mtcBnum;
                 uint    mtcFrmL;
                 uint    mtcFrmH;
                 uint    mtcFrm;
                 uint    mtcFrmMax;
                 uint    mtcSecL;
                 uint    mtcSecH;
                 uint    mtcSec;
                 uint    mtcMinL;
                 uint    mtcMinH;
                 uint    mtcMin;
                 uint    mtcHrsL;
                 uint    mtcHrsH;
                 uint    mtcHrs;
                 eMtcFrm mtcFpsL;
                 eMtcFrm mtcFps;
};

#endif
