/*
 * dmmsnoop - MIDI monitor and prober
 * Copyright (C) 2012 Devin Anderson
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

#include "_main.hpp"

int main(int argc, char **argv) {
  App          *theApp;
  QString       errorMessage; // MAGICK, this is templated somehow, it gets filled in ghostland of Qt.
  QString       directory;
  QString       language;
  QTranslator  *theTrQtf;
  QTranslator  *theTrApp;
  TrMsg        *theTrMsg;
  Ctrl         *theCtrl;
  int           result;

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
  theCtrl = new Ctrl(theApp); // Has to wait for theApp to be translate-stuffed

  theCtrl->run();

  if(errorMessage.isEmpty()) {
    fprintf(stdout, "Exiting without errors ...\n");  fflush(stdout);
    result = EXIT_SUCCESS;
  }
  else {
    fprintf(stdout, "Error: %s\n", errorMessage.toStdString().c_str());
    result = EXIT_FAILURE;
  }

  theApp->removeTranslator(theTrApp);
  theApp->removeTranslator(theTrQtf);

  return result;
}
