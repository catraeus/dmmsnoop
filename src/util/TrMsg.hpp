
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
#include <stdint.h>
#include <stdlib.h>

#define __IN_TR_MSG_HPP_

class TrMsg {
  public:
    enum eLang {
      DEL_ENGLISH    = 0x00000000U,
      DEL_FRANCAIS   = 0x00000001U,
      DEL_DEUTSCH    = 0x00000002U,
      DEL_ESPANOL    = 0x00000003U,
      DEL_NUM_LANG   = 0x00000005U,
      DEL_LANG_BIG   = 0xFFFFFFFFU
    };
  #include "TrMsgApp.hpp"
  #include "TrMsgMiMeta.hpp"
  #include "TrMsgMiStat.hpp"
  #include "TrMsgMiSys.hpp"
  private:
  public:
  private:
            TrMsg(eLang i_lang);       // For singleton pattern
  public:
    static  TrMsg  *GetInstance(eLang i_lang); // For singleton pattern
           ~TrMsg();
    void    SetLang(eLang i_lang);
  private:
    eLang   lang;


    char  **msgMiCc;
    char ***msgMiCcPile;
    char  **msgMiCcEn;
    char  **msgMiCcFr;
    char  **msgMiCcDe;
    char  **msgMiCcEs;

    char  **msgMiCcMode;
    char ***msgMiCcModePile;
    char  **msgMiCcModeEn;
    char  **msgMiCcModeFr;
    char  **msgMiCcModeDe;
    char  **msgMiCcModeEs;

private:
  static TrMsg *theTrMsgInstance;
};

#undef __IN_TR_MSG_HPP_

#endif
