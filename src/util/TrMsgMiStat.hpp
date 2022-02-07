
//=================================================================================================
// Original File Name : TrMsgMiStat.hpp
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
    enum eStatType { // These have had the leading 1 bit in the uint8_t masked off.
      DES_NOTE_OFF    = 0x00000000U,
      DES_NOTE_ON     = 0x00000001U,
      DES_POLY_PRES   = 0x00000002U,
      DES_CC          = 0x00000003U,
      DES_PROG_CHNG   = 0x00000004U,
      DES_CHAN_PRES   = 0x00000005U,
      DES_PITCH_BEND  = 0x00000006U,
      DES_SYSTEM      = 0x00000007U,
                                    // No need to size it, MIDI spec's 8 of these, we'll mask to enforce count
      DES_STAT_BIG    = 0xFFFFFFFFU // This will force it to be a uint32_t
    };
char   *MsgMiStatGet(eStatType i_theType)  { return msgMiStat[i_theType]; }
  private:
    char  **msgMiStat;
    char ***msgMiStatPile;
    char  **msgMiStatEn;
    char  **msgMiStatFr;
    char  **msgMiStatDe;
    char  **msgMiStatEs;

//==================================================================================================
//==================================================================================================
// In the cpp file.
#else

  msgMiStatPile   = new char**[DEL_NUM_LANG];   // As many as we have languages.  First dim is language enum
  msgMiStatEn     = new char *[8];   // I have no idea how many meta messages there will be.
  msgMiStatFr     = new char *[8];   // Il y a je ne sais pas de numero dans ce section Stat
  msgMiStatDe     = new char *[8];   // I don't know Deutsch
  msgMiStatEs     = new char *[8];   // I don't know Espanol
  msgMiStatPile[DEL_ENGLISH]  = msgMiStatEn;
  msgMiStatPile[DEL_FRANCAIS] = msgMiStatFr;
  msgMiStatPile[DEL_DEUTSCH]  = msgMiStatDe;
  msgMiStatPile[DEL_ESPANOL]  = msgMiStatEs;

//==================================================================================================
//==== The English stuff
  tStrAry = msgMiStatPile[DEL_ENGLISH];
  tStrAry[DES_NOTE_OFF]    = (char *)"Nt Off";
  tStrAry[DES_NOTE_ON]     = (char *)"Nt On";
  tStrAry[DES_POLY_PRES]   = (char *)"Poly Pres";
  tStrAry[DES_CC]          = (char *)"CC";
  tStrAry[DES_PROG_CHNG]   = (char *)"Prog";
  tStrAry[DES_CHAN_PRES]   = (char *)"Chan Pres";
  tStrAry[DES_PITCH_BEND]  = (char *)"Pitch Bend";
  tStrAry[DES_SYSTEM]      = (char *)"System";

//==================================================================================================
//==== Les trucs francais
  tStrAry = msgMiStatPile[DEL_FRANCAIS];
  tStrAry[DES_NOTE_OFF]    = (char *)"Nt Off";
  tStrAry[DES_NOTE_ON]     = (char *)"Nt On";
  tStrAry[DES_POLY_PRES]   = (char *)"Poly Pres";
  tStrAry[DES_CC]          = (char *)"CC";
  tStrAry[DES_PROG_CHNG]   = (char *)"Prog";
  tStrAry[DES_CHAN_PRES]   = (char *)"Chan Pres";
  tStrAry[DES_PITCH_BEND]  = (char *)"Pitch Bend";
  tStrAry[DES_SYSTEM]      = (char *)"System";
//==================================================================================================
//==== Das deutsche zeug
  tStrAry = msgMiStatPile[DEL_DEUTSCH];
  tStrAry[DES_NOTE_OFF]    = (char *)"Nt Off";
  tStrAry[DES_NOTE_ON]     = (char *)"Nt On";
  tStrAry[DES_POLY_PRES]   = (char *)"Poly Pres";
  tStrAry[DES_CC]          = (char *)"CC";
  tStrAry[DES_PROG_CHNG]   = (char *)"Prog";
  tStrAry[DES_CHAN_PRES]   = (char *)"Chan Pres";
  tStrAry[DES_PITCH_BEND]  = (char *)"Pitch Bend";
  tStrAry[DES_SYSTEM]      = (char *)"System";
//==================================================================================================
//==== Las cosas españolas
  tStrAry = msgMiStatPile[DEL_ESPANOL];
  tStrAry[DES_NOTE_OFF]    = (char *)"Nt Off";
  tStrAry[DES_NOTE_ON]     = (char *)"Nt On";
  tStrAry[DES_POLY_PRES]   = (char *)"Poly Pres";
  tStrAry[DES_CC]          = (char *)"CC";
  tStrAry[DES_PROG_CHNG]   = (char *)"Prog";
  tStrAry[DES_CHAN_PRES]   = (char *)"Chan Pres";
  tStrAry[DES_PITCH_BEND]  = (char *)"Pitch Bend";
  tStrAry[DES_SYSTEM]      = (char *)"System";

#endif
