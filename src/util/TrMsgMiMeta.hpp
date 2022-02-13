
//=================================================================================================
// Original File Name : TrMsgMiMeta.hpp
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
    enum eMetaType {
      DEM_ZRO_LEN      = 0x00000000U, // Weird man! The lower layer gave us an empty byte array.
      DEM_LEN_STAT     = 0x00000001U, // Given the Status, the number of bytes the lower layer gave us is wrong.
      DEM_LEN_SYS      = 0x00000002U, // Given the Status, the number of bytes the lower layer gave us is wrong.
      DEM_SYSEX_TRUNC  = 0x00000003U, // The message after the SysEx pile of bytes wasn't 0xF7
      DEM_SYSEND_START = 0x00000004U, // The message came up with just SysEx End.  Pretty goofy.
      DEM_STAT_LOW     = 0x00000005U, // Weirder man!  The first byte sent up from the lower layer was a non-Status byte.
      DEM_DATA_HIGH    = 0x00000006U, // The bytes after the Status byte have at least 1 greater than 0x7F
      DEM_RTM_FLT_FAIL = 0x00000007U, // RtMidi was command to filter, but it came through anyway.
      DEM_MTC_OOO      = 0x00000008U, // The MTC in Qtr Frm mode was out of order
      DEM_NONE         = 0x00000009U, // No Trouble Found.
      DEM_META_NUM     = 0x0000000AU, // To let is size things at new char *xxx[] time.
      DEM_META_BIG     = 0xFFFFFFFFU  // To force it to type uint32_t
    };
char   *MsgMiMetaGet(eMetaType i_theType)  { return msgMiMeta[i_theType]; }
  private:
    char  **msgMiMeta;
    char ***msgMiMetaPile;
    char  **msgMiMetaEn;
    char  **msgMiMetaFr;
    char  **msgMiMetaDe;
    char  **msgMiMetaEs;

//==================================================================================================
//==================================================================================================
#else
  msgMiMetaPile   = new char**[DEL_NUM_LANG];   // As many as we have languages.  First dim is language enum
  msgMiMetaEn     = new char *[DEM_META_NUM];   // I have no idea how many meta messages there will be.
  msgMiMetaFr     = new char *[DEM_META_NUM];   // Il y a je ne sais pas de numero dans ce section Meta
  msgMiMetaDe     = new char *[DEM_META_NUM];   // I don't know Deutsch
  msgMiMetaEs     = new char *[DEM_META_NUM];   // I don't know Espanol
  msgMiMetaPile[DEL_ENGLISH]  = msgMiMetaEn;
  msgMiMetaPile[DEL_FRANCAIS] = msgMiMetaFr;
  msgMiMetaPile[DEL_DEUTSCH]  = msgMiMetaDe;
  msgMiMetaPile[DEL_ESPANOL]  = msgMiMetaEs;

//==================================================================================================
//==== The English stuff
  tStrAry = msgMiMetaPile[DEL_ENGLISH];
  tStrAry[DEM_ZRO_LEN]         = (char *)"Zero-Length";
  tStrAry[DEM_LEN_STAT]        = (char *)"N bytes Stat";
  tStrAry[DEM_LEN_SYS]         = (char *)"N bytes Sys";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx no End";
  tStrAry[DEM_SYSEND_START]    = (char *)"End without SysEx";
  tStrAry[DEM_STAT_LOW]        = (char *)"Status Byte < 0x80";
  tStrAry[DEM_DATA_HIGH]       = (char *)"Data Byte >= 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi didn't Filter";
  tStrAry[DEM_NONE]            = (char *)"No Trouble";

//==================================================================================================
//==== Les trucs francais
  tStrAry = msgMiMetaPile[DEL_FRANCAIS];
  tStrAry[DEM_ZRO_LEN]         = (char *)"Longueur zero";
  tStrAry[DEM_LEN_STAT]        = (char *)"N octets Stat";
  tStrAry[DEM_LEN_SYS]         = (char *)"N octets Sys";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx sans End";
  tStrAry[DEM_SYSEND_START]    = (char *)"End sans SysEx";
  tStrAry[DEM_STAT_LOW]        = (char *)"L'Octet Status < 0x80";
  tStrAry[DEM_DATA_HIGH]       = (char *)"L'Octet Status >= 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi ne filtre pas";
  tStrAry[DEM_NONE]            = (char *)"Pas de problème";

//==================================================================================================
//==== Das deutsche zeug
  tStrAry = msgMiMetaPile[DEL_DEUTSCH];
  tStrAry[DEM_ZRO_LEN]         = (char *)"Länge Null";
  tStrAry[DEM_LEN_STAT]        = (char *)"N Bytes Stat";
  tStrAry[DEM_LEN_SYS]         = (char *)"N Bytes Sys";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx ohne End";
  tStrAry[DEM_SYSEND_START]    = (char *)"End ohne Sysex";
  tStrAry[DEM_STAT_LOW]        = (char *)"Status Byte < 0x80";
  tStrAry[DEM_DATA_HIGH]       = (char *)"Status Byte >= 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"nicht gefiltert";
  tStrAry[DEM_NONE]            = (char *)"Kein Problem";

//==================================================================================================
//==== Las cosas españolas
  tStrAry = msgMiMetaPile[DEL_ESPANOL];
  tStrAry[DEM_ZRO_LEN]         = (char *)"longitud cero";
  tStrAry[DEM_LEN_STAT]        = (char *)"N bytes Stat";
  tStrAry[DEM_LEN_SYS]         = (char *)"N bytes Sys";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx no End";
  tStrAry[DEM_SYSEND_START]    = (char *)"End sin que SysEx";
  tStrAry[DEM_STAT_LOW]        = (char *)"Byte de Status < 0x80";
  tStrAry[DEM_DATA_HIGH]       = (char *)"Byte de Status >= 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"no filtró";
  tStrAry[DEM_NONE]            = (char *)"No hay problema";

#endif
