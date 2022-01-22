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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtUiTools/QUiLoader>

#include "util.h"

QString
getMIDIControlString(quint8 control)
{
    assert(control < 0x80);
    QString name;
    switch (control) {
    case 0x0:
        name = qApp->tr("Bank Select");
        break;
    case 0x1:
        name = qApp->tr("Modulation Wheel");
        break;
    case 0x2:
        name = qApp->tr("Breath Controller");
        break;
    case 0x4:
        name = qApp->tr("Foot Controller");
        break;
    case 0x5:
        name = qApp->tr("Portamento Time");
        break;
    case 0x6:
        name = qApp->tr("Data Entry MSB");
        break;
    case 0x7:
        name = qApp->tr("Channel Volume");
        break;
    case 0x8:
        name = qApp->tr("Balance");
        break;
    case 0xa:
        name = qApp->tr("Pan");
        break;
    case 0xb:
        name = qApp->tr("Expression Controller");
        break;
    case 0xc:
        name = qApp->tr("Effect Control 1");
        break;
    case 0xd:
        name = qApp->tr("Effect Control 2");
        break;
    case 0x10:
        name = qApp->tr("General Purpose Controller 1");
        break;
    case 0x11:
        name = qApp->tr("General Purpose Controller 2");
        break;
    case 0x12:
        name = qApp->tr("General Purpose Controller 3");
        break;
    case 0x13:
        name = qApp->tr("General Purpose Controller 4");
        break;
    case 0x20:
        name = qApp->tr("Bank Select LSB");
        break;
    case 0x21:
        name = qApp->tr("Modulation Wheel LSB");
        break;
    case 0x22:
        name = qApp->tr("Breath Controller LSB");
        break;
    case 0x23:
        name = qApp->tr("Control 3 LSB");
        break;
    case 0x24:
        name = qApp->tr("Foot Controller LSB");
        break;
    case 0x25:
        name = qApp->tr("Portamento Time LSB");
        break;
    case 0x26:
        name = qApp->tr("Data Entry LSB");
        break;
    case 0x27:
        name = qApp->tr("Channel Volume LSB");
        break;
    case 0x28:
        name = qApp->tr("Balance LSB");
        break;
    case 0x29:
        name = qApp->tr("Control 9 LSB");
        break;
    case 0x2a:
        name = qApp->tr("Pan LSB");
        break;
    case 0x2b:
        name = qApp->tr("Expression Controller LSB");
        break;
    case 0x2c:
        name = qApp->tr("Effect Control 1 LSB");
        break;
    case 0x2d:
        name = qApp->tr("Effect Control 2 LSB");
        break;
    case 0x2e:
        name = qApp->tr("Control 14 LSB");
        break;
    case 0x2f:
        name = qApp->tr("Control 15 LSB");
        break;
    case 0x30:
        name = qApp->tr("General Purpose Controller 1 LSB");
        break;
    case 0x31:
        name = qApp->tr("General Purpose Controller 2 LSB");
        break;
    case 0x32:
        name = qApp->tr("General Purpose Controller 3 LSB");
        break;
    case 0x33:
        name = qApp->tr("General Purpose Controller 4 LSB");
        break;
    case 0x34:
        name = qApp->tr("Control 20 LSB");
        break;
    case 0x35:
        name = qApp->tr("Control 21 LSB");
        break;
    case 0x36:
        name = qApp->tr("Control 22 LSB");
        break;
    case 0x37:
        name = qApp->tr("Control 23 LSB");
        break;
    case 0x38:
        name = qApp->tr("Control 24 LSB");
        break;
    case 0x39:
        name = qApp->tr("Control 25 LSB");
        break;
    case 0x3a:
        name = qApp->tr("Control 26 LSB");
        break;
    case 0x3b:
        name = qApp->tr("Control 27 LSB");
        break;
    case 0x3c:
        name = qApp->tr("Control 28 LSB");
        break;
    case 0x3d:
        name = qApp->tr("Control 29 LSB");
        break;
    case 0x3e:
        name = qApp->tr("Control 30 LSB");
        break;
    case 0x3f:
        name = qApp->tr("Control 31 LSB");
        break;
    case 0x40:
        name = qApp->tr("Damper Pedal On/Off");
        break;
    case 0x41:
        name = qApp->tr("Portamento On/Off");
        break;
    case 0x42:
        name = qApp->tr("Sostenuto On/Off");
        break;
    case 0x43:
        name = qApp->tr("Soft Pedal On/Off");
        break;
    case 0x44:
        name = qApp->tr("Legato Footswitch");
        break;
    case 0x45:
        name = qApp->tr("Hold 2");
        break;
    case 0x46:
        name = qApp->tr("Sound Controller 1 (Default: Sound Variation)");
        break;
    case 0x47:
        name = qApp->tr("Sound Controller 2 (Default: Timbre/Harmonic "
                        "Intensity)");
        break;
    case 0x48:
        name = qApp->tr("Sound Controller 3 (Default: Release Time)");
        break;
    case 0x49:
        name = qApp->tr("Sound Controller 4 (Default: Attack Time)");
        break;
    case 0x4a:
        name = qApp->tr("Sound Controller 5 (Default: Brightness)");
        break;
    case 0x4b:
        name = qApp->tr("Sound Controller 6 (Default: Decay Time)");
        break;
    case 0x4c:
        name = qApp->tr("Sound Controller 7 (Default: Vibrato Rate)");
        break;
    case 0x4d:
        name = qApp->tr("Sound Controller 8 (Default: Vibrato Depth)");
        break;
    case 0x4e:
        name = qApp->tr("Sound Controller 9 (Default: Vibrato Delay)");
        break;
    case 0x4f:
        name = qApp->tr("Sound Controller 10");
        break;
    case 0x50:
        name = qApp->tr("General Purpose Controller 5");
        break;
    case 0x51:
        name = qApp->tr("General Purpose Controller 6");
        break;
    case 0x52:
        name = qApp->tr("General Purpose Controller 7");
        break;
    case 0x53:
        name = qApp->tr("General Purpose Controller 8");
        break;
    case 0x5b:
        name = qApp->tr("Effects 1 Depth (Default: Reverb Send Level)");
        break;
    case 0x5c:
        name = qApp->tr("Effects 2 Depth");
        break;
    case 0x5d:
        name = qApp->tr("Effects 3 Depth (Default: Chorus Send Level");
        break;
    case 0x5e:
        name = qApp->tr("Effects 4 Depth");
        break;
    case 0x5f:
        name = qApp->tr("Effects 5 Depth");
        break;
    case 0x60:
        name = qApp->tr("Data Increment");
        break;
    case 0x61:
        name = qApp->tr("Data Decrement");
        break;
    case 0x62:
        name = qApp->tr("Non-Registered Parameter Number LSB");
        break;
    case 0x63:
        name = qApp->tr("Non-Registered Parameter Number MSB");
        break;
    case 0x64:
        name = qApp->tr("Registered Parameter Number LSB");
        break;
    case 0x65:
        name = qApp->tr("Registered Parameter Number MSB");
        break;
    case 0x78:
        name = qApp->tr("All Sound Off");
        break;
    case 0x79:
        name = qApp->tr("Reset All Controllers");
        break;
    case 0x7a:
        name = qApp->tr("Local Control On/Off");
        break;
    case 0x7b:
        name = qApp->tr("All Notes Off");
        break;
    case 0x7c:
        name = qApp->tr("Omni Mode Off");
        break;
    case 0x7d:
        name = qApp->tr("Omni Mode On");
        break;
    case 0x7e:
        name = qApp->tr("Mono Mode On");
        break;
    case 0x7f:
        name = qApp->tr("Poly Mode On");
        break;
    default:
        name = qApp->tr("Undefined");
    }
    return qApp->translate("midiControlFormat", "%1 - %2").
        arg(QLocale::system().toString(control), name);
}

QString
getMIDINoteString(quint8 note)
{
    assert(note < 0x80);
    QString noteStr;
    switch (note % 12) {
    case 0:
        noteStr = qApp->translate("cNote", "C");
        break;
    case 1:
        noteStr = qApp->translate("cSharpNote", "C#");
        break;
    case 2:
        noteStr = qApp->translate("dNote", "D");
        break;
    case 3:
        noteStr = qApp->translate("dSharpNote", "D#");
        break;
    case 4:
        noteStr = qApp->translate("eNote", "E");
        break;
    case 5:
        noteStr = qApp->translate("fNote", "F");
        break;
    case 6:
        noteStr = qApp->translate("fSharpNote", "F#");
        break;
    case 7:
        noteStr = qApp->translate("gNote", "G");
        break;
    case 8:
        noteStr = qApp->translate("gSharpNote", "G#");
        break;
    case 9:
        noteStr = qApp->translate("aNote", "A");
        break;
    case 10:
        noteStr = qApp->translate("aSharpNote", "A#");
        break;
    case 11:
        noteStr = qApp->translate("bNote", "B");
    }
    QLocale locale = QLocale::system();
    return qApp->translate("midiNoteFormat", "%1 (%2%3)").
        arg(locale.toString(static_cast<int>(note)), noteStr,
            locale.toString(static_cast<int>(note / 12) - 1));
}

QWidget *
loadForm(const QString &path, QWidget *parent)
{
    QFile file(path);

    // XX: Qt makes a jump based on uninitialized values the first time this
    // call is made.
    bool opened = file.open(QFile::ReadOnly);
    assert(opened);

    QUiLoader uiLoader;

    // XX: Qt also makes a jump based on uninitialized values the first time
    // this call is made.
    QWidget *widget = uiLoader.load(&file, parent);
    assert(widget);

    file.close();
    return widget;
}
