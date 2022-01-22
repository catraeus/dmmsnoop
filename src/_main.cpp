/*
 * midisnoop - MIDI monitor and prober
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

#include <cstdio>
#include <cstdlib>
//#include <exception>

#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QTextStream>

#include "controller.h"
#include "error.h"

int main(int argc, char **argv) {
  Application   theApp(argc, argv);
  QString       errorMessage;
  QTranslator   qtTranslator;
  QTranslator   translator;
  int           result;

  theApp.setApplicationName("midisnoop");
  theApp.setOrganizationDomain("midisnoop.catraeus.com");
  theApp.setOrganizationName("midisnoop.catraeus.com");

// Translations
  QString      directory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
  QString      language = QLocale::system().name();
    
  qtTranslator.load("qt_" + language, directory);
  theApp.installTranslator(&qtTranslator);
    
  translator.load("midisnoop_" + language);
  theApp.installTranslator(&translator);
  qDebug() <<   theApp.tr("Translations loaded.");

//    try {
// Controller
  qDebug() <<   theApp.tr("Creating core application objects ...");
  Controller controller(  theApp);
  qDebug() <<   theApp.tr("Core application objects created.");

// Run the program
  qDebug() <<   theApp.tr("Running ...");
  controller.run();

//    } catch (Error &e) {
//        errorMessage = e.getMessage();
//    } catch (std::exception &e) {
//        errorMessage = e.what();
//    }

    // Deal with errors.
  if (errorMessage.isEmpty()) {
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
