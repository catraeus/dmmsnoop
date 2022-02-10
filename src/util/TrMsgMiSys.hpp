
//=================================================================================================
// Original File Name : TrMsgMiSys.hpp
// Original Author    : duncang
// Creation Date      : Jan 27, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/* Architecture
    The meta-messages.  When things go wrong, need the user to become informed of coolness ...
       Pretty much everything that isn't either connecting or MIDI Spec verbiage.
*/
//==================================================================================================
/*
 * dmmsnoop - MIDI monitor and prober
 * Copyright (C) 2022 Duncan Gray
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

//==================================================================================================
//==================================================================================================
#ifdef __IN_TR_MSG_HPP_

  public:
    enum eSysType { // Second nibble iff first is F
      DEY_SYSEX       = 0x00000000U,
      DEY_TC_QF       = 0x00000001U,
      DEY_SONG_POS    = 0x00000002U,
      DEY_SONG_SEL    = 0x00000003U,
      DEY_UNDEF_4     = 0x00000004U,
      DEY_UNDEF_5     = 0x00000005U,
      DEY_TUNE_REQ    = 0x00000006U,
      DEY_END_SYSEX   = 0x00000007U,
      DEY_TIM_CLK     = 0x00000008U,
      DEY_UNDEF_9     = 0x00000009U,
      DEY_SONG_START  = 0x0000000AU,
      DEY_SONG_CONT   = 0x0000000BU,
      DEY_SONG_STOP   = 0x0000000CU,
      DEY_UNDEF_D     = 0x0000000DU,
      DEY_ACT_SENS    = 0x0000000EU,
      DEY_RESET       = 0x0000000FU,
                                    // No need to size it, MIDI spec's 16 of these, we'll mask to enforce count
      DEY_SYS_BIG     = 0XFFFFFFFFU
    };
char   *MsgMiSysGet(eSysType i_theType)  { return msgMiSys[i_theType]; }
  private:
    char  **msgMiSys;
    char ***msgMiSysPile;
    char  **msgMiSysEn;
    char  **msgMiSysFr;
    char  **msgMiSysDe;
    char  **msgMiSysEs;

//==================================================================================================
//==================================================================================================
// In the cpp file.
#else

  fprintf(stdout, "In TrMsgMiSys.hpp\n");fflush(stdout);
  msgMiSysPile   = new char**[DEL_NUM_LANG];   // As many as we have languages.  First dim is language enum
  msgMiSysEn     = new char *[16];   // Only 1 size in MIDI Sped
  msgMiSysFr     = new char *[16];   //
  msgMiSysDe     = new char *[16];   // I don't know Deutsch
  msgMiSysEs     = new char *[16];   // I don't know Espanol
  msgMiSysPile[DEL_ENGLISH]  = msgMiSysEn;
  msgMiSysPile[DEL_FRANCAIS] = msgMiSysFr;
  msgMiSysPile[DEL_DEUTSCH]  = msgMiSysDe;
  msgMiSysPile[DEL_ESPANOL]  = msgMiSysEs;

//==================================================================================================
//==== The English stuff
  tStrAry = msgMiSysPile[DEL_ENGLISH];
  tStrAry[DEY_SYSEX]      = (char *)"SysEx";
  tStrAry[DEY_TC_QF]      = (char *)"MTC QFrm";
  tStrAry[DEY_SONG_POS]   = (char *)"Song Pos";
  tStrAry[DEY_SONG_SEL]   = (char *)"Song Sel";
  tStrAry[DEY_UNDEF_4]    = (char *)"Undef 4";
  tStrAry[DEY_UNDEF_5]    = (char *)"Undef 5";
  tStrAry[DEY_TUNE_REQ]   = (char *)"Tune Req";
  tStrAry[DEY_END_SYSEX]  = (char *)"SysEx End";
  tStrAry[DEY_TIM_CLK]    = (char *)"Time Clock";
  tStrAry[DEY_UNDEF_9]    = (char *)"Undef 9";
  tStrAry[DEY_SONG_START] = (char *)"Song Start";
  tStrAry[DEY_SONG_CONT]  = (char *)"Song Cont";
  tStrAry[DEY_SONG_STOP]  = (char *)"Song Stop";
  tStrAry[DEY_UNDEF_D]    = (char *)"Undef D";
  tStrAry[DEY_ACT_SENS]   = (char *)"Active Sense";
  tStrAry[DEY_RESET]      = (char *)"Reset";

//==================================================================================================
//==== Les trucs francais
  tStrAry = msgMiSysPile[DEL_FRANCAIS];
  tStrAry[DEY_SYSEX]      = (char *)"SysEx";
  tStrAry[DEY_TC_QF]      = (char *)"MTC QFrm";
  tStrAry[DEY_SONG_POS]   = (char *)"Song Pos";
  tStrAry[DEY_SONG_SEL]   = (char *)"Song Select";
  tStrAry[DEY_UNDEF_4]    = (char *)"Undef 4";
  tStrAry[DEY_UNDEF_5]    = (char *)"Undef 5";
  tStrAry[DEY_TUNE_REQ]   = (char *)"Tune Req";
  tStrAry[DEY_END_SYSEX]  = (char *)"SysEx End";
  tStrAry[DEY_TIM_CLK]    = (char *)"Time Clock";
  tStrAry[DEY_UNDEF_9]    = (char *)"Undef 9";
  tStrAry[DEY_SONG_START] = (char *)"Song Start";
  tStrAry[DEY_SONG_CONT]  = (char *)"Song Cont";
  tStrAry[DEY_SONG_STOP]  = (char *)"Song Stop";
  tStrAry[DEY_UNDEF_D]    = (char *)"Undef D";
  tStrAry[DEY_ACT_SENS]   = (char *)"Active Sense";
  tStrAry[DEY_RESET]      = (char *)"Reset";
//==================================================================================================
//==== Das deutsche zeug
  tStrAry = msgMiSysPile[DEL_DEUTSCH];
  tStrAry[DEY_SYSEX]      = (char *)"SysEx";
  tStrAry[DEY_TC_QF]      = (char *)"MTC QFrm";
  tStrAry[DEY_SONG_POS]   = (char *)"Song Pos";
  tStrAry[DEY_SONG_SEL]   = (char *)"Song Select";
  tStrAry[DEY_UNDEF_4]    = (char *)"Undef 4";
  tStrAry[DEY_UNDEF_5]    = (char *)"Undef 5";
  tStrAry[DEY_TUNE_REQ]   = (char *)"Tune Req";
  tStrAry[DEY_END_SYSEX]  = (char *)"SysEx End";
  tStrAry[DEY_TIM_CLK]    = (char *)"Time Clock";
  tStrAry[DEY_UNDEF_9]    = (char *)"Undef 9";
  tStrAry[DEY_SONG_START] = (char *)"Song Start";
  tStrAry[DEY_SONG_CONT]  = (char *)"Song Cont";
  tStrAry[DEY_SONG_STOP]  = (char *)"Song Stop";
  tStrAry[DEY_UNDEF_D]    = (char *)"Undef D";
  tStrAry[DEY_ACT_SENS]   = (char *)"Active Sense";
  tStrAry[DEY_RESET]      = (char *)"Reset";
//==================================================================================================
//==== Las cosas españolas
  tStrAry = msgMiSysPile[DEL_ESPANOL];
  tStrAry[DEY_SYSEX]      = (char *)"SysEx";
  tStrAry[DEY_TC_QF]      = (char *)"MTC QFrm";
  tStrAry[DEY_SONG_POS]   = (char *)"Song Pos";
  tStrAry[DEY_SONG_SEL]   = (char *)"Song Select";
  tStrAry[DEY_UNDEF_4]    = (char *)"Undef 4";
  tStrAry[DEY_UNDEF_5]    = (char *)"Undef 5";
  tStrAry[DEY_TUNE_REQ]   = (char *)"Tune Req";
  tStrAry[DEY_END_SYSEX]  = (char *)"SysEx End";
  tStrAry[DEY_TIM_CLK]    = (char *)"Time Clock";
  tStrAry[DEY_UNDEF_9]    = (char *)"Undef 9";
  tStrAry[DEY_SONG_START] = (char *)"Song Start";
  tStrAry[DEY_SONG_CONT]  = (char *)"Song Cont";
  tStrAry[DEY_SONG_STOP]  = (char *)"Song Stop";
  tStrAry[DEY_UNDEF_D]    = (char *)"Undef D";
  tStrAry[DEY_ACT_SENS]   = (char *)"Active Sense";
  tStrAry[DEY_RESET]      = (char *)"Reset";

#endif
