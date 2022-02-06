
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
      DEM_SYSEX_TRUNC  = 0x00000002U, // The message after the SysEx pile of bytes wasn't 0xF7
      DEM_STAT_UNDEF   = 0x00000003U, // Stat was one of the Undefined values, returning raw bytes in the message.
      DEM_STAT_LOW     = 0x00000004U, // Weirder man!  The first byte sent up from the lower layer was a non-Status byte.
      DEM_RTM_FLT_FAIL = 0x00000005U, // RtMidi was command to filter, but it came through anyway.
      DEM_META_NUM     = 0x00000006U, // To let is size things at new char *xxx[] time.
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
  tStrAry[DEM_LEN_STAT]        = (char *)"Byte Count";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx stream didn't have SysEx End next";
  tStrAry[DEM_STAT_UNDEF]      = (char *)"Undefined Status Byte";
  tStrAry[DEM_STAT_LOW]        = (char *)"Status Byte < 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi Failed to Filter a message";

//==================================================================================================
//==== Les trucs francais
  tStrAry = msgMiMetaPile[DEL_FRANCAIS];
  tStrAry[DEM_ZRO_LEN]         = (char *)"Message nulle";
  tStrAry[DEM_LEN_STAT]        = (char *)"Le nombre d'octets est incorrect pour le Status";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"Le flux SysEx n'avait pas de SysEx End ensuite";
  tStrAry[DEM_STAT_UNDEF]      = (char *)"Octet Status non défini ";
  tStrAry[DEM_STAT_LOW]        = (char *)"L'Octet Status < 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi Failed to Filter a message";

//==================================================================================================
//==== Das deutsche zeug
  tStrAry = msgMiMetaPile[DEL_DEUTSCH];
  tStrAry[DEM_ZRO_LEN]         = (char *)"MIDI-Meldung ohne Länge";
  tStrAry[DEM_LEN_STAT]        = (char *)"Byte Count ist für den Status falsch";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"Der SysEx-Stream hatte kein SysEx-End als nächstes";
  tStrAry[DEM_STAT_UNDEF]      = (char *)"Undefiniertes Statusbyte";
  tStrAry[DEM_STAT_LOW]        = (char *)"Status Byte < 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi Failed to Filter a message";

//==================================================================================================
//==== Las cosas españolas
  tStrAry = msgMiMetaPile[DEL_ESPANOL];
  tStrAry[DEM_ZRO_LEN]         = (char *)"Mensaje MIDI de longitud cero";
  tStrAry[DEM_LEN_STAT]        = (char *)"El recuento de bytes es incorrecto para el Status";
  tStrAry[DEM_SYSEX_TRUNC]     = (char *)"SysEx stream no tenía SysEx End a continuación";
  tStrAry[DEM_STAT_UNDEF]      = (char *)"Byte de Status indefinido";
  tStrAry[DEM_STAT_LOW]        = (char *)"Byte de Status < 0x80";
  tStrAry[DEM_RTM_FLT_FAIL]    = (char *)"RtMidi Failed to Filter a message";

#endif
