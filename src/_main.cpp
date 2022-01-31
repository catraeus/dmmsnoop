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
  App           theApp(argc, argv);
  QString       errorMessage;
  QTranslator   qtTranslator;
  QTranslator   translator;
  TrMsg        *theTrMsg;
  int           result;

  theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);

  theApp.setApplicationName(theTrMsg->MsgAppGet(TrMsg::DAT_APP_DOM));
  theApp.setOrganizationDomain("dmmsnoop.catraeus.com");
  theApp.setOrganizationName("dmmsnoop.catraeus.com");

// Translations
  QString      directory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
  QString      language = QLocale::system().name();

  qtTranslator.load("qt_" + language, directory);
  theApp.installTranslator(&qtTranslator);

  translator.load("dmmsnoop_" + language);
  theApp.installTranslator(&translator);
  qDebug() <<   theApp.tr("Translations loaded.");

// Ctrl
  fprintf(stdout, "Instantiating the Controller\n");  fflush(stdout);
  Ctrl theCtrl(  theApp);
  fprintf(stdout, "Controller instantiated.\n");  fflush(stdout);

// Run the program
  qDebug() <<   theApp.tr("Running ...");
  theCtrl.run();


    // Deal with errors.
  if(errorMessage.isEmpty()) {
    qDebug() <<   theApp.tr("Exiting without errors ...");
    result = EXIT_SUCCESS;
  }
  else {
    QTextStream(stderr) <<   theApp.tr("Error: %1\n").arg(errorMessage);
    result = EXIT_FAILURE;
  }

// Cleanup
  qDebug() <<   theApp.tr("Unloading translations ...");
  theApp.removeTranslator(&translator);
  theApp.removeTranslator(&qtTranslator);

  return result;
}
