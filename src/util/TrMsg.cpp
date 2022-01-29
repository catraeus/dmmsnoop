
//=================================================================================================
// Original File Name : CtlRsmp.cpp
// Original Author    : duncang
// Creation Date      : Jan27, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/* Architecture
    This class
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

        TrMsg::TrMsg(eLang i_lang) {
  lang = i_lang;
  msgMidiStatPile   = new char**[32];
  msgMidiStatEn     = new char*[32];
  msgMidiStatFr     = new char*[32];
  msgMidiStatDe     = new char*[32];
  msgMidiStatPile[DEL_ENGLISH]  = msgMidiStatEn;
  msgMidiStatPile[DEL_FRANCAIS] = msgMidiStatFr;
  msgMidiStatPile[DEL_DEUTSCH]  = msgMidiStatDe;
  msgMidiStat = msgMidiStatPile[i_lang];
}
        TrMsg::~TrMsg(){}

char   *TrMsg::MsgGet(eMsgType i_theType)  { (void)i_theType;    return (char *)0; }
