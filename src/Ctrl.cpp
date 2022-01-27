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

#include <cassert>

#include <QtCore/QDebug>

#include "Ctrl.hpp"
#include "util/Error.hpp"
#include "util/util.hpp"

// Static data

const int statusLengths[0x80] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,        0, 2, 3, 2, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, 1, 1
};

// Class definition

Ctrl::Ctrl(App &i_theApp, QObject *i_parent)
: QObject(i_parent)
, theApp(i_theApp) {
    // Setup about view
    theQVwAbout.setMajorVersion(DMMSNOOP_MAJOR_VERSION);
    theQVwAbout.setMinorVersion(DMMSNOOP_MINOR_VERSION);
    theQVwAbout.setRevision(DMMSNOOP_REVISION);
    connect(&theQVwAbout, SIGNAL(closeRequest()), &theQVwAbout, SLOT(hide()));

    // Setup configure view
    int driverCount = theMidi.getDriverCount();
    if(! driverCount) {
        throw Error(tr("no MIDI drivers found"));
    }
    for (int i = 0; i < driverCount; i++) {
        theQVwConfig.addDriver(i, theMidi.getDriverName(i));
    }
    int driver = theMidi.getDriver();
    int outputPort = theMidi.getOutputPort();
    theQVwConfig.setDriver                        (driver);
    theQVwConfig.setInputPort(theMidi.getInputPort ());
    theQVwConfig.setIgnoreActiveSensingEvents     (theMidi.getIgnoreActiveSensingEvents());
    theQVwConfig.setIgnoreSystemExclusiveEvents   (theMidi.getIgnoreSystemExclusiveEvents());
    theQVwConfig.setIgnoreTimeEvents              (theMidi.getIgnoreTimeEvents());
    theQVwConfig.setOutputPort                    (outputPort);
    connect(&theQVwConfig, SIGNAL(closeRequest()),                                   &theQVwConfig, SLOT(hide()));
    connect(&theQVwConfig, SIGNAL(driverChangeRequest(int)),                         &theMidi, SLOT(setDriver(int)));
    connect(&theQVwConfig, SIGNAL(ignoreActiveSensingEventsChangeRequest(bool)),     &theMidi, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&theQVwConfig, SIGNAL(ignoreSystemExclusiveEventsChangeRequest(bool)),   &theMidi, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&theQVwConfig, SIGNAL(ignoreTimeEventsChangeRequest(bool)),              &theMidi, SLOT(setIgnoreTimeEvents(bool)));
    connect(&theQVwConfig, SIGNAL(inputPortChangeRequest(int)),                      &theMidi, SLOT(setInputPort(int)));
    connect(&theQVwConfig, SIGNAL(outputPortChangeRequest(int)),                     &theMidi, SLOT(setOutputPort(int)));

    // Setup error view
    connect(&theQVwErr, SIGNAL(closeRequest()),                              &theQVwErr,     SLOT(hide()));

    // Setup main view
    theQVwMain.setMessageSendEnabled((driver != -1) && (outputPort != -1));
    theQVwMain.SetTimeZero(theMidi.TimeGet());
    connect(&theQVwMain, SIGNAL(aboutRequest()),                               &theQVwAbout,     SLOT(show()));
    connect(&theQVwMain, SIGNAL(addMessageRequest()),                          &theQVwMsg,   SLOT(show()));
    connect(&theQVwMain, SIGNAL(clearMessagesRequest()),                       &theQVwMain,      SLOT(clearMessages()));
    connect(&theQVwMain, SIGNAL(configureRequest()),                           &theQVwConfig, SLOT(show()));
    connect(&theQVwMain, SIGNAL(closeRequest()),                               &theApp,   SLOT(quit()));

    // Setup message view
    connect(&theQVwMsg, SIGNAL(closeRequest()),                            &theQVwMsg,   SLOT(hide()));
    connect(&theQVwMsg, SIGNAL(sendRequest(const QString &)),              &theQVwMsg,   SLOT(hide()));
    connect(&theQVwMsg, SIGNAL(sendRequest(const QString &)),                              SLOT(handleMessageSend(const QString &)));

    // Setup theMidi
    connect(&theMidi, SIGNAL(messageReceived(quint64, const QByteArray &)),                   SLOT(handleReceivedMessage(quint64, const QByteArray &)));
    connect(&theMidi, SIGNAL(driverChanged(int)),                             &theQVwConfig, SLOT(setDriver(int)));
    connect(&theMidi, SIGNAL(driverChanged(int)),                                             SLOT(handleDriverChange()));
    connect(&theMidi, SIGNAL(ignoreActiveSensingEventsChanged(bool)),         &theQVwConfig, SLOT(setIgnoreActiveSensingEvents(bool)));
    connect(&theMidi, SIGNAL(ignoreSystemExclusiveEventsChanged(bool)),       &theQVwConfig, SLOT(setIgnoreSystemExclusiveEvents(bool)));
    connect(&theMidi, SIGNAL(ignoreTimeEventsChanged(bool)),                  &theQVwConfig, SLOT(setIgnoreTimeEvents(bool)));
    connect(&theMidi, SIGNAL(inputPortAdded(int, QString)),                   &theQVwConfig, SLOT(addInputPort(int, QString)));
    connect(&theMidi, SIGNAL(inputPortChanged(int)),                          &theQVwConfig, SLOT(setInputPort(int)));
    connect(&theMidi, SIGNAL(inputPortChanged(int)),                                          SLOT(handleDriverChange()));
    connect(&theMidi, SIGNAL(inputPortRemoved(int)),                          &theQVwConfig, SLOT(removeInputPort(int)));
    connect(&theMidi, SIGNAL(outputPortAdded(int, QString)),                  &theQVwConfig, SLOT(addOutputPort(int, QString)));
    connect(&theMidi, SIGNAL(outputPortChanged(int)),                         &theQVwConfig, SLOT(setOutputPort(int)));
    connect(&theMidi, SIGNAL(outputPortChanged(int)),                                         SLOT(handleDriverChange()));
    connect(&theMidi, SIGNAL(outputPortRemoved(int)),                         &theQVwConfig, SLOT(removeOutputPort(int)));

    // Setup theApp
    connect(&theApp, SIGNAL(eventError(QString)),                &theQVwErr, SLOT(setMessage(QString)));
    connect(&theApp, SIGNAL(eventError(QString)),                &theQVwErr, SLOT(show()));
}

Ctrl::~Ctrl() {
    // Disconnect theMidi signals handled by the controller before the theMidi is deleted.
    disconnect(&theMidi, SIGNAL(messageReceived(quint64, const QByteArray &)),                   this, SLOT(handleReceivedMessage(quint64, const QByteArray &)));
    disconnect(&theMidi, SIGNAL(driverChanged(int)),                   this, SLOT(handleDriverChange()));
    disconnect(&theMidi, SIGNAL(inputPortChanged(int)),                   this, SLOT(handleDriverChange()));
    disconnect(&theMidi, SIGNAL(outputPortChanged(int)),                   this, SLOT(handleDriverChange()));
}

QString Ctrl::getGenericDataDescription(const QByteArray &message, int lastIndex) {
  assert((lastIndex >= -1) && (lastIndex < message.count()));
  if(lastIndex == -1) {
    lastIndex = message.count() - 1;
  }
  QStringList dataParts;
  for (int i = 1; i <= lastIndex; i++) {
        dataParts += QString("%1").
            arg(static_cast<uint>(message[i]), 2, 16, QChar('0'));
    }
    dataParts += tr("(%1 bytes)").arg(lastIndex);
    return dataParts.join(" ");
}

void Ctrl::handleDriverChange() {
    theQVwMain.setMessageSendEnabled((theMidi.getDriver() != -1) &&
                                   (theMidi.getOutputPort() != -1));
}

void Ctrl::handleMessageSend(const QString &message) {
    // Convert the message to bytes.
    QStringList bytes = message.split(' ', QString::SkipEmptyParts);
    int count = bytes.count();
    QByteArray msg;
    for (int i = 0; i < count; i++) {
        QString byteStr = bytes[i];
        bool success;
        uint value = byteStr.toUInt(&success, 16);
        if((! success) || (value > 0xff)) {
            showError(tr("'%1' is not a valid hexadecimal MIDI byte").
                      arg(byteStr));
            return;
        }
        msg.append(static_cast<char>(static_cast<quint8>(value)));
    }

    // Make sure the bytes represent a valid MIDI message.
    parseMessage(msg);
    if(! valid) {
        showError(tr("The given message is not a valid MIDI message."));
        return;
    }

    // Send the message.
    quint64 timeStamp = theMidi.sendMessage(msg);
    theQVwMain.MsgAddTX(timeStamp, sMidiStatus, sMidiData,                                valid);
}

void Ctrl::handleReceivedMessage(quint64 timeStamp, const QByteArray &message) {
    parseMessage(message);
    theQVwMain.addReceivedMessage(timeStamp, sMidiStatus, sMidiData,                                    valid);
}

void Ctrl::parseMessage(const QByteArray &message) {
    // Make sure we have an actual message.
    int length = message.count();
    if(! length) {
        sMidiData = "";
        sMidiStatus = tr("empty message");
        valid = false;
        return;
    }

    // Validate status byte.
    quint8 status = static_cast<quint8>(message[0]);
    if(status < 0x80) {
        sMidiData = getGenericDataDescription(message);
        sMidiStatus = tr("%1 (invalid status)").
            arg(static_cast<uint>(status), 2, 16, QChar('0'));
        valid = false;
        return;
    }

    // Validate length and data.
    QString errorMessage;
    int expectedLength = statusLengths[status - 0x80];
    int lastDataIndex;
    switch (expectedLength) {

    case -1:
        sMidiData = getGenericDataDescription(message);
        sMidiStatus = tr("%1 (undefined status)").
            arg(static_cast<uint>(status), 2, 16, QChar('0'));
        valid = false;
        return;

    case 0:
        if(length == 1) {
            sMidiData = "";
            sMidiStatus = tr("System Exclusive (no data)");
            valid = false;
            return;
        }
        if(static_cast<quint8>(message[length - 1]) != 0xf7) {
            sMidiData = getGenericDataDescription(message);
            sMidiStatus = tr("System Exclusive (end not found)");
            valid = false;
            return;
        }
        lastDataIndex = length - 2;
        break;

    default:
        if(length != expectedLength) {
            sMidiData = getGenericDataDescription(message);
            sMidiStatus = tr("%1 (incorrect length)").
                arg(static_cast<uint>(status), 2, 16, QChar('0'));
            valid = false;
            return;
        }
        lastDataIndex = length - 1;
    }

    // Validate data bytes.
    for (int i = 1; i <= lastDataIndex; i++) {
        if(static_cast<quint8>(message[i]) >= 0x80) {
            sMidiData = getGenericDataDescription(message);
            sMidiStatus = tr("%1 (invalid data)").
                arg(static_cast<uint>(status), 2, 16, QChar('0'));
            valid = false;
            return;
        }
    }

    // Convert message to user-friendly strings.
    QString s;
    int value;
    valid = true;
    switch (status & 0xf0) {

    case 0x80:
        sMidiData = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        sMidiStatus = tr("Note Off, Channel %1").arg((status & 0xf) + 1);
        break;

    case 0x90:
        sMidiData = tr("Note: %1, Velocity: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        sMidiStatus = tr("Note On, Channel %1").arg((status & 0xf) + 1);
        break;

    case 0xa0:
        sMidiData = tr("Note: %1, Pressure: %2").
            arg(getMIDINoteString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        sMidiStatus = tr("Aftertouch, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xb0:
        sMidiData = tr("CC: %1, Value: %2").
            arg(getMIDIControlString(static_cast<quint8>(message[1]))).
            arg(static_cast<quint8>(message[2]));
        sMidiStatus = tr("CC, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xc0:
        sMidiData = tr("Number: %1").arg(static_cast<quint8>(message[1]));
        sMidiStatus = tr("Program Change, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xd0:
        sMidiData = tr("Pressure: %1").
            arg(static_cast<quint8>(message[1]));
        sMidiStatus = tr("Channel Pressure, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xe0:
        sMidiData = tr("Value: %1").
            arg((((static_cast<qint16>(message[2])) << 7) |
                 (static_cast<qint16>(message[1]))) - 0x2000);
        sMidiStatus = tr("Pitch Wheel, Channel %1").
            arg((status & 0xf) + 1);
        break;

    case 0xf0:
        switch (status & 0x0f) {

        case 0x0:
            sMidiData = getGenericDataDescription(message, lastDataIndex);
            sMidiStatus = tr("System Exclusive");
            break;

        case 0x1:
            value = message[0] & 0xf;
            switch (message[0] & 0x70) {

            case 0x00:
                sMidiData = tr("Frames Low Nibble: %1").arg(value);
                break;

            case 0x10:
                sMidiData = tr("Frames High Nibble: %1").arg(value);
                break;

            case 0x20:
                sMidiData = tr("Seconds Low Nibble: %1").arg(value);
                break;

            case 0x30:
                sMidiData = tr("Seconds High Nibble: %1").arg(value);
                break;

            case 0x40:
                sMidiData = tr("Minutes Low Nibble: %1").arg(value);
                break;

            case 0x50:
                sMidiData = tr("Minutes High Nibble: %1").arg(value);
                break;

            case 0x60:
                sMidiData = tr("Hours Low Nibble: %1").arg(value);
                break;

            case 0x70:
                switch ((value & 0x6) >> 1) {

                case 0:
                    s = tr("24 fps");
                    break;

                case 1:
                    s = tr("25 fps");
                    break;

                case 2:
                    s = tr("30 fps (drop-frame)");
                    break;

                case 3:
                    s = tr("30 fps");
                    break;

                default:
                    // We shouldn't get here.
                    assert(false);
                }
                sMidiData = tr("Hours High Nibble: %1, SMPTE Type: %2").
                    arg(value & 1).arg(s);
                break;

            default:
                // We shouldn't get here.
                assert(false);
            }
            sMidiStatus = tr("MTC Quarter Frame");
            break;

        case 0x2:
            sMidiData = tr("MIDI Beat: %1").
                arg(((static_cast<qint16>(message[2])) << 7) |
                    (static_cast<qint16>(message[1])));
            sMidiStatus = tr("Song Position Pointer");
            break;

        case 0x3:
            sMidiData = tr("Song Number: %1").
                arg(static_cast<quint8>(message[1]));
            sMidiStatus = tr("Song Select");
            break;

        case 0x6:
            sMidiData = "";
            sMidiStatus = tr("Tune Request");
            break;

        case 0x8:
            sMidiData = "";
            sMidiStatus = tr("MIDI Clock");
            break;

        case 0x9:
            sMidiData = "";
            sMidiStatus = tr("MIDI Tick");
            break;

        case 0xa:
            sMidiData = "";
            sMidiStatus = tr("MIDI Start");
            break;

        case 0xb:
            sMidiData = "";
            sMidiStatus = tr("MIDI Continue");
            break;

        case 0xc:
            sMidiData = "";
            sMidiStatus = tr("MIDI Stop");
            break;

        case 0xe:
            sMidiData = "";
            sMidiStatus = tr("Active Sense");
            break;

        case 0xf:
            sMidiData = "";
            sMidiStatus = tr("Reset");
            break;

        default:
            // We shouldn't get here.
            assert(false);
        }
        break;

    default:
        // We shouldn't get here.
        assert(false);
    }
}

void Ctrl::run() {
    theQVwMain.show();
    theApp.exec();
}

void Ctrl::showError(const QString &message) {
    theQVwErr.setMessage(message);
    theQVwErr.show();
}
