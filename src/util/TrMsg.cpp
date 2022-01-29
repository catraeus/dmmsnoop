
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
