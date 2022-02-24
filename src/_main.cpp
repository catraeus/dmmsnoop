
// $Id: _main.cpp 497 2019-08-17 20:46:44Z duncang $

//=================================================================================================
// Original File Name : _main.cpp
// Original Author    : duncang
// Creation Date      : Jan 14, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/*
  dmmsnoop - Developer MIDI Message Snoop.
    Based on Devin Anderson's midisnoop.
  + Prints a "disassembled" version of MIDI messages.
  + Sends messages that you build as ascii Hex and decodes them.
  + Hope to get it to make a cross-connect from any CC to any CC
*/
//
//=================================================================================================

/*
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

#include "_main.hpp"

int main(int argc, char **argv) {
  App          *theApp;
  QString       directory;
  QString       language;
  QTranslator  *theTrQtf;
  QTranslator  *theTrApp;
  TrMsg        *theTrMsg;
  Ctrl         *theCtrl;
  int           result;


  QVwMain      *theQVwMain;

  fprintf(stdout, "Build No.:  %d\n", BUILD_NO);fflush(stdout);

  theApp   = new App(argc, argv);
  theTrQtf = new QTranslator();
  theTrApp = new QTranslator();

  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);

  theApp->setApplicationName(theTrMsg->MsgAppGet(TrMsg::DAT_APP_DOM));
  theApp->setOrganizationDomain("dmmsnoop.catraeus.com");
  theApp->setOrganizationName("dmmsnoop.catraeus.com");

  directory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
  language  = QLocale::system().name();

  theTrQtf->load("qt_" + language, directory);
  theApp->installTranslator(theTrQtf);

  theTrApp->load("dmmsnoop_" + language);
  theApp->installTranslator(theTrApp);

  theQVwMain   = new QVwMain(theApp);

  theCtrl = new Ctrl(theApp); // Has to wait for theApp to be translate-stuffed
  theCtrl->SetWinMain(theQVwMain);
  theCtrl->BuildDrvIf       ();
  theCtrl->BuildWinConfig   ();

  theCtrl->BuildWinMain     ();
  theCtrl->ConnSigWinMain   ();
  theCtrl->ConnSigWinMsg    ();
  theCtrl->ConnSigDrvIf     ();
  theCtrl->ConnSigApp       ();

  theQVwMain->show();
  result = QApplication::exec();


  theApp->removeTranslator(theTrApp);
  theApp->removeTranslator(theTrQtf);
  delete  theCtrl;
  delete  theTrMsg;

  return result;
}
