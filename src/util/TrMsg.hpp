
//=================================================================================================
// Original File Name : TrMsg.hpp
// Original Author    : duncang
// Creation Date      : Jan 27, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/* Architecture
    This class
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

#ifndef __TR_MSG_HPP_
#define __TR_MSG_HPP_


class TrMsg {
  public:
    enum eLang {
      DEL_ENGLISH,
      DEL_FRANCAIS,
      DEL_DEUTSCH
    };
    enum eMsgType {
      DET_STATUS,
      DET_CC
    };
  public:
            TrMsg(eLang i_lang);
           ~TrMsg();
    char   *MsgGet(eMsgType i_msgType);
  private:
    eLang   lang;
    char  **msgMidiStat;
    char ***msgMidiStatPile;
    char  **msgMidiStatEn;
    char  **msgMidiStatFr;
    char  **msgMidiStatDe;
private:

};

#endif
