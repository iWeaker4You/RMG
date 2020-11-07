/*
 * Rosalie's Mupen GUI - https://github.com/Rosalie241/RMG
 *  Copyright (C) 2020 Rosalie Wanders <rosalie@mailbox.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "SettingsDialog.hpp"
#include "../../Globals.hpp"

using namespace UserInterface::Dialog;
using namespace M64P::Wrapper;

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    this->setupUi(this);

    this->treeWidget->topLevelItem(0)->setSelected(true);
    this->treeWidget->expandAll();

    this->loadCoreSettings();
    this->loadGameSettings();
    this->loadPluginSettings();
}

SettingsDialog::~SettingsDialog(void)
{
}

void SettingsDialog::restoreDefaults(int stackedWidgetIndex)
{
    switch (stackedWidgetIndex)
    {
    default:
    case 0:
        loadCoreSettings();
        break;
    case 1:
        loadGameSettings();
        break;
    case 2:
        loadGameCoreSettings();
        break;
    case 3:
        loadGamePluginSettings();
        break;
    case 4:
        loadPluginSettings();
        break;
    }
}

void SettingsDialog::reloadSettings(int stackedWidgetIndex)
{
    switch (stackedWidgetIndex)
    {
    default:
    case 0:
        loadCoreSettings();
        break;
    case 1:
        loadGameSettings();
        break;
    case 2:
        loadGameCoreSettings();
        break;
    case 3:
        loadGamePluginSettings();
        break;
    case 4:
        loadPluginSettings();
        break;
    }
}

void SettingsDialog::loadCoreSettings(void)
{
    bool disableExtraMem = false;
    int counterFactor = 0;
    int cpuEmulator = 0;
    int siDmaDuration = -1;
    bool randomizeInterrupt = true;
    bool debugger = false;

    g_MupenApi.Config.GetOption(QString("Core"), QString("DisableExtraMem"), &disableExtraMem);
    g_MupenApi.Config.GetOption(QString("Core"), QString("CountPerOp"), &counterFactor);
    g_MupenApi.Config.GetOption(QString("Core"), QString("R4300Emulator"), &cpuEmulator);
    g_MupenApi.Config.GetOption(QString("Core"), QString("SiDmaDuration"), &siDmaDuration);
    g_MupenApi.Config.GetOption(QString("Core"), QString("RandomizeInterrupt"), &randomizeInterrupt);
    g_MupenApi.Config.GetOption(QString("Core"), QString("EnableDebugger"), &debugger);

    this->coreMemorySize->setCurrentIndex(!disableExtraMem);
    this->coreCounterFactor->setCurrentIndex(counterFactor);
    this->coreCpuEmulator->setCurrentIndex(cpuEmulator);
    this->coreSiDmaDuration->setValue(siDmaDuration);
    this->coreRandomizeTiming->setChecked(randomizeInterrupt);
    this->coreDebugger->setChecked(debugger);
}

void SettingsDialog::loadGameSettings(void)
{
    if (!g_EmuThread->isRunning())
    {
        this->hideEmulationInfoText();
        this->treeWidget->topLevelItem(1)->setDisabled(true);
        return;
    }

    M64P::Wrapper::RomInfo_t gameInfo = {0};

    g_MupenApi.Core.GetRomInfo(&gameInfo);

    this->gameGoodName->setText(gameInfo.Settings.goodname);
    this->gameMemorySize->setCurrentIndex(!gameInfo.Settings.disableextramem);
    this->gameSaveType->setCurrentIndex(gameInfo.Settings.savetype);
    this->gameCounterFactor->setCurrentIndex(gameInfo.Settings.countperop);
    this->gameSiDmaDuration->setValue(gameInfo.Settings.sidmaduration);
}

void SettingsDialog::loadGameCoreSettings(void)
{
}

void SettingsDialog::loadGamePluginSettings(void)
{
}

void SettingsDialog::loadPluginSettings(void)
{
    QList<Plugin_t> pluginList;
    QComboBox *comboBoxArray[4] = {this->videoPlugins, this->audioPlugins, this->inputPlugins, this->rspPlugins};
    PluginType type;
    QComboBox *comboBox;
    Plugin_t plugin_t;

    for (int i = 0; i < 4; i++)
    {
        type = (PluginType)i;
        comboBox = comboBoxArray[i];
        comboBox->clear();

        pluginList.clear();
        pluginList = g_MupenApi.Core.GetPlugins((PluginType)i);

        int pluginIndex = -1, stopCounting = 0;
        for (Plugin_t &p : pluginList)
        {
            comboBox->addItem(p.Name);

            if (stopCounting == 1)
                continue;

            pluginIndex++;

            if (g_MupenApi.Core.GetCurrentPlugin(type, &plugin_t) &&
                plugin_t.FileName == p.FileName)
            {
                stopCounting = 1;
            }
        }

        comboBox->setCurrentIndex(pluginIndex);
    }
}

void SettingsDialog::saveSettings(void)
{
    this->saveCoreSettings();
    this->saveGameSettings();
    this->savePluginSettings();
}

void SettingsDialog::saveCoreSettings(void)
{
    bool disableExtraMem = (this->coreMemorySize->currentIndex() == 0);
    int counterFactor = this->coreCounterFactor->currentIndex();
    int cpuEmulator = this->coreCpuEmulator->currentIndex();
    int siDmaDuration = this->coreSiDmaDuration->value();
    bool randomizeInterrupt = this->coreRandomizeTiming->isChecked();
    bool debugger = this->coreDebugger->isChecked();

    g_MupenApi.Config.SetOption(QString("Core"), QString("DisableExtraMem"), disableExtraMem);
    g_MupenApi.Config.SetOption(QString("Core"), QString("CountPerOp"), counterFactor);
    g_MupenApi.Config.SetOption(QString("Core"), QString("R4300Emulator"), cpuEmulator);
    g_MupenApi.Config.SetOption(QString("Core"), QString("SiDmaDuration"), siDmaDuration);
    g_MupenApi.Config.SetOption(QString("Core"), QString("RandomizeInterrupt"), randomizeInterrupt);
    g_MupenApi.Config.SetOption(QString("Core"), QString("EnableDebugger"), debugger);
}

void SettingsDialog::saveGameSettings(void)
{
}

void SettingsDialog::savePluginSettings(void)
{
    QList<Plugin_t> pluginList;
    QComboBox *comboBoxArray[4] = {this->videoPlugins, this->audioPlugins, this->inputPlugins, this->rspPlugins};
    QString settingsArray[4] = {SETTINGS_PLUGIN_GFX, SETTINGS_PLUGIN_AUDIO, SETTINGS_PLUGIN_INPUT, SETTINGS_PLUGIN_RSP};
    PluginType type;
    QComboBox *comboBox;

    for (int i = 0; i < 4; i++)
    {
        type = (PluginType)i;
        comboBox = comboBoxArray[i];

        pluginList.clear();
        pluginList = g_MupenApi.Core.GetPlugins((PluginType)i);

        for (Plugin_t &p : pluginList)
        {
            if (p.Name == comboBox->currentText())
            {
                g_MupenApi.Config.SetOption(SETTINGS_SECTION, settingsArray[i], p.FileName);
                g_MupenApi.Core.SetPlugin(p);
                break;
            }
        }
    }
}

void SettingsDialog::hideEmulationInfoText(void)
{
    QHBoxLayout *layouts[] = 
    {
        this->emulationInfoLayout,
        this->emulationInfoLayout_2
    };

    for (const QHBoxLayout *layout : layouts)
    {
        for (int i = 0; i < layout->count(); i++)
        {
            QWidget *widget = layout->itemAt(i)->widget();
            widget->hide();
        }
    }
}

void SettingsDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    int topLevelCount = this->treeWidget->topLevelItemCount();
    int index = this->treeWidget->indexOfTopLevelItem(current);

    // count all children up until our item
    for (int i = 0; i < topLevelCount && i < index; i++)
    {
        index += this->treeWidget->topLevelItem(i)->childCount();
    }

    // if we're a child ourselves,
    // get index of parent and add that onto our own index
    if (index == -1)
    {
        index = this->treeWidget->indexOfTopLevelItem(current->parent());
        index += current->parent()->indexOfChild(current) + 1;
    }

    this->stackedWidget->setCurrentIndex(index);
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QPushButton *pushButton = (QPushButton *)button;
    QPushButton *resetButton = this->buttonBox->button(QDialogButtonBox::Reset);
    QPushButton *defaultButton = this->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    QPushButton *saveButton = this->buttonBox->button(QDialogButtonBox::Save);
    QPushButton *cancelButton = this->buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton *okButton = this->buttonBox->button(QDialogButtonBox::Ok);


    if (pushButton == resetButton)
    {
        this->reloadSettings(this->stackedWidget->currentIndex());
    }
    else if (pushButton == defaultButton)
    {
        this->restoreDefaults(this->stackedWidget->currentIndex());
    }
    else if (pushButton == saveButton)
    {
        this->saveSettings();
    }
    else if (pushButton == cancelButton)
    {
        this->reject();
    }
    else if (pushButton == okButton)
    {
        this->saveSettings();
        this->accept();
    }
}
