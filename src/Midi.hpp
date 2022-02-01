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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <stdint.h>

#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QObject>

#include "util/TrMsg.hpp"

#include <rtmidi/RtMidi.h>

class Midi: public QObject {
    Q_OBJECT

public:
  explicit  Midi(QObject *parent=0);
           ~Midi();
  int       getDriver() const;
  int       getDriverCount() const;
  QString   getDriverName(int index) const;
  bool      ModeIgnActSnGet() const;
  bool      ModeIgnSysExGet() const;
  bool      ModeIgnMiTimGet() const;
  int       getInputPort() const;
  int       getInputPortCount() const;
  QString   getInputPortName(int index) const;
  int       getOutputPort() const;
  int       getOutputPortCount() const;
  QString   getOutputPortName(int index) const;

public slots:
  void      setDriver(int index);
  void      OnModeIgnActSnChg(bool ignore);
  void      OnModeIgnSysExChg(bool ignore);
  void      OnModeIgnMiTimChg(bool ignore);
  void      setInputPort(int index);
  void      setOutputPort(int index);
  quint64   OnMiMsgTx(const QByteArray &message);

signals:
  void      EmDrvChange(int index);
  void      EmModeIgnActSnChg(bool ignore);
  void      EmModeIgnSysExChg(bool ignore);
  void      EmModeIgnMiTimChg(bool ignore);
  void      EmPortInpAdd(int index, const QString &name);
  void      EmPortInpChg(int index);
  void      EmPortInpDel(int index);
  void      outputPortAdded(int index, const QString &name);
  void      outputPortChanged(int index);
  void      outputPortRemoved(int index);
  void      EmMiMsgRx(quint64 timeStamp, const QByteArray &message);

public:
         quint64 TimeGet() const;
private:
  static void    handleMidiInput(double timeStamp,       std::vector<uint8_t> *message, void *engine);
         void    handleMidiInput(double timeStamp, const std::vector<uint8_t> &message);
         void    removePorts();
         void    updateEventFilter();

    TrMsg               *theTrMsg;
    int                  driver;
    QList<RtMidi::Api>   driverAPIs;
    QStringList          driverNames;
    bool                 modeIgnActSn;
    bool                 modeIgnSysEx;
    bool                 modeIgnMiTim;
    RtMidiIn            *input;
    int                  inputPort;
    QStringList          inputPortNames;
    RtMidiOut           *theRtMidiOut;
    int                  outputPort;
    QStringList          outputPortNames;
    bool                 virtualPortsAdded;
};

#endif
