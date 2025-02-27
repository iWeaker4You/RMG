/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "EmulationThread.hpp"
#include <RMG-Core/Core.hpp>

using namespace Thread;

EmulationThread::EmulationThread(QObject *parent) : QThread(parent)
{
}

EmulationThread::~EmulationThread(void)
{
}

void EmulationThread::SetRomFile(QString file)
{
    this->rom_File = file;
}

void EmulationThread::SetDiskFile(QString file)
{
    this->disk_File = file;
}

void EmulationThread::run(void)
{
    emit this->on_Emulation_Started();

    bool ret;

    ret = CoreStartEmulation(this->rom_File.toStdString(), this->disk_File.toStdString());

    if (!ret)
    {
        this->error_Message = QString::fromStdString(CoreGetError());
    }

    emit this->on_Emulation_Finished(ret);
}

QString EmulationThread::GetLastError(void)
{
    return this->error_Message;
}
