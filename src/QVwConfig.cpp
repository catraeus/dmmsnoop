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

#include "QVwConfig.hpp"
#include "util/util.hpp"

QVwConfig::QVwConfig(QObject *parent):
    DesignerView(":/dmmsnoop/QVwConfig.ui", parent) {
    QWidget *rootWidget = getRootWidget();

    driver = getChild<QComboBox>(rootWidget, "driver");
    connect(driver, SIGNAL(activated(int)),  SLOT(handleDriverActivation(int)));

    ignoreActiveSensingEvents =
        getChild<QCheckBox>(rootWidget, "ignoreActiveSensingEvents");
    connect(ignoreActiveSensingEvents, SIGNAL(clicked(bool)),  SIGNAL(ignoreActiveSensingEventsChangeRequest(bool)));

    ignoreSystemExclusiveEvents =
        getChild<QCheckBox>(rootWidget, "ignoreSystemExclusiveEvents");
    connect(ignoreSystemExclusiveEvents, SIGNAL(clicked(bool)),  SIGNAL(ignoreSystemExclusiveEventsChangeRequest(bool)));

    ignoreTimeEvents = getChild<QCheckBox>(rootWidget, "ignoreTimeEvents");
    connect(ignoreTimeEvents, SIGNAL(clicked(bool)),  SIGNAL(ignoreTimeEventsChangeRequest(bool)));

    inputPort = getChild<QComboBox>(rootWidget, "inputPort");
    connect(inputPort, SIGNAL(activated(int)), SLOT(handleInputPortActivation(int)));

    outputPort = getChild<QComboBox>(rootWidget, "outputPort");
    connect(outputPort, SIGNAL(activated(int)), SLOT(handleOutputPortActivation(int)));

    closeButton = getChild<QPushButton>(rootWidget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));
}

QVwConfig::~QVwConfig() {}

void QVwConfig::addDriver                      (int  index, const QString &name) {                      driver->insertItem              (index + 1, name); }
void QVwConfig::addInputPort                   (int  index, const QString &name) {                   inputPort->insertItem              (index + 1, name); }
void QVwConfig::addOutputPort                  (int  index, const QString &name) {                  outputPort->insertItem              (index + 1, name); }
void QVwConfig::handleDriverActivation         (int  index                     ) { emit                         driverChangeRequest     (index - 1      ); }
void QVwConfig::handleInputPortActivation      (int  index                     ) { emit                         inputPortChangeRequest  (index - 1      ); }
void QVwConfig::handleOutputPortActivation     (int  index                     ) { emit                         outputPortChangeRequest (index - 1      ); }
void QVwConfig::removeInputPort                (int  index                     ) {                   inputPort->removeItem              (index + 1      ); }
void QVwConfig::removeOutputPort               (int  index                     ) {                  outputPort->removeItem              (index + 1      ); }
void QVwConfig::setDriver                      (int  index                     ) {                      driver->setCurrentIndex         (index + 1      ); }
void QVwConfig::setIgnoreActiveSensingEvents   (bool ignore                    ) {   ignoreActiveSensingEvents->setChecked              (ignore         ); }
void QVwConfig::setIgnoreSystemExclusiveEvents (bool ignore                    ) { ignoreSystemExclusiveEvents->setChecked              (ignore         ); }
void QVwConfig::setIgnoreTimeEvents            (bool ignore                    ) {            ignoreTimeEvents->setChecked              (ignore         ); }
void QVwConfig::setInputPort                   (int  index                     ) {                   inputPort->setCurrentIndex         (index + 1      ); }
void QVwConfig::setOutputPort                  (int  index                     ) {                  outputPort->setCurrentIndex         (index + 1      ); }

