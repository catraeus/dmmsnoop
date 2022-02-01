
//=================================================================================================
// Original File Name : CtlRsmp.cpp
// Original Author    : duncang
// Creation Date      : Jan27, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/* Architecture
    This class gives us the following:
      strings as non constants so we don't have to coerce const to non-const
      Movable, triple-starred old-school strings so we can instantly shift languages.
      an enum framework for groups of messages based on what part of the MIDI spec ehty come from
      an enum framework for language specification that is easily growable for the whole world.
      Dang, you can even change language on the fly!
*/
//=================================================================================================
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

#include "TrMsg.hpp"
#include <stdio.h>

        TrMsg *TrMsg::theTrMsgInstance = (TrMsg *)0;
  const char   TrMsg::hexAscii[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

         TrMsg::TrMsg(eLang i_lang) {
  char **tStrAry; // Only really for use in the following language files
  #include "TrMsgApp.hpp"
  #include "TrMsgMiMeta.hpp"
  #include "TrMsgMiStat.hpp"
  #include "TrMsgMiSys.hpp"
  SetLang(i_lang);
  return;
}
TrMsg  *TrMsg::GetInstance(eLang i_lang) {
  if((uint64_t)theTrMsgInstance != (uint64_t)0) {
    theTrMsgInstance->SetLang(i_lang);
  }
  else {
    theTrMsgInstance = new TrMsg(i_lang);
  }
  return theTrMsgInstance;
}
        TrMsg::~TrMsg(){}

void    TrMsg::SetLang(eLang i_lang) {
  if(i_lang >= DEL_NUM_LANG)
    i_lang = DEL_ENGLISH;
  msgMiMeta   = msgMiMetaPile  [i_lang];
  msgMiStat   = msgMiStatPile  [i_lang];  // MAGICK, the enum has been defined so this array location stuff just works.
  msgMiSys    = msgMiSysPile   [i_lang];  // MAGICK, the enum has been defined so this array location stuff just works.
  msgApp      = msgAppPile     [i_lang];
//msgMiCc     = msgMiCcPile    [i_lang];
//msgMiCcMode = msgMiCcModePile[i_lang];
  return;
}
void    TrMsg::ByteToString(uint i_byte, char *o_str) {
  o_str[0] = hexAscii[((i_byte & 0x000000F0U) >> 8)];
  o_str[1] = hexAscii[((i_byte & 0x0000000FU) >> 0)];
  o_str[2] = '\0';
  return;
}

