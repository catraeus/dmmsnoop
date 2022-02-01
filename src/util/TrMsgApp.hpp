
//=================================================================================================
// Original File Name : TrMsgMiApp.hpp
// Original Author    : duncang
// Creation Date      : Jan 27, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/* Architecture
    The App messages.  All that fun stuff about how wonderful the app and developers are.
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
    enum eAppType {
      DAT_APP_NAME     = 0x00000000U,
      DAT_APP_DOM      = 0x00000001U,
      DAT_APP_URL_CODE = 0x00000002U,
      DAT_APP_LANG     = 0x00000003U,
      DAT_APP_NUM      = 0x00000004U,
      DAT_APP_BIG      = 0xFFFFFFFFU
    };
char   *MsgAppGet (eAppType  i_theType)  { return msgApp[i_theType]; }
  private:
    char  **msgApp;
    char ***msgAppPile;
    char  **msgAppEn;
    char  **msgAppFr;
    char  **msgAppDe;
    char  **msgAppEs;

//==================================================================================================
//==================================================================================================
// In the cpp file.
#else

  msgAppPile   = new char**[DEL_NUM_LANG];   // As many as we have languages.  First dim is language enum
  msgAppEn     = new char *[DEM_META_NUM];   // I have no idea how many meta messages there will be.
  msgAppFr     = new char *[DEM_META_NUM];   // Il y a je ne sais pas de numero dans ce section App
  msgAppDe     = new char *[DEM_META_NUM];   // I don't know Deutsch
  msgAppEs     = new char *[DEM_META_NUM];   // I don't know Espanol
  msgAppPile[DEL_ENGLISH]  = msgAppEn;
  msgAppPile[DEL_FRANCAIS] = msgAppFr;
  msgAppPile[DEL_DEUTSCH]  = msgAppDe;
  msgAppPile[DEL_ESPANOL]  = msgAppEs;

//==================================================================================================
//==== The English stuff
  tStrAry = msgAppPile[DEL_ENGLISH];
  tStrAry[DAT_APP_NAME]     = (char *)"dmmsnoop";
  tStrAry[DAT_APP_DOM]      = (char *)"catraeus.com";
  tStrAry[DAT_APP_URL_CODE] = (char *)"https://github.com/catraeus/dmmsnoop";
  tStrAry[DAT_APP_LANG]     = (char *)"English";

//==================================================================================================
//==== Les trucs francais
  tStrAry = msgAppPile[DEL_FRANCAIS];
  tStrAry[DAT_APP_NAME]     = (char *)"dmmsnoop";
  tStrAry[DAT_APP_DOM]      = (char *)"catraeus.com";
  tStrAry[DAT_APP_URL_CODE] = (char *)"https://github.com/catraeus/dmmsnoop";
  tStrAry[DAT_APP_LANG]     = (char *)"français";
//==================================================================================================
//==== Das deutsche zeug
  tStrAry = msgAppPile[DEL_DEUTSCH];
  tStrAry[DAT_APP_NAME]     = (char *)"dmmsnoop";
  tStrAry[DAT_APP_DOM]      = (char *)"catraeus.com";
  tStrAry[DAT_APP_URL_CODE] = (char *)"https://github.com/catraeus/dmmsnoop";
  tStrAry[DAT_APP_LANG]     = (char *)"Deutsch";
//==================================================================================================
//==== Las cosas españolas
  tStrAry = msgAppPile[DEL_ESPANOL];
  tStrAry[DAT_APP_NAME]     = (char *)"dmmsnoop";
  tStrAry[DAT_APP_DOM]      = (char *)"catraeus.com";
  tStrAry[DAT_APP_URL_CODE] = (char *)"https://github.com/catraeus/dmmsnoop";
  tStrAry[DAT_APP_LANG]     = (char *)"español";

#endif
