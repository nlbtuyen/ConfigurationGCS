#include "uavconfig.h"
#include "ui_uavconfig.h"
#include "seriallinkinterface.h"
#include "seriallink.h"
#include "linkmanager.h"
#include "mainwindow.h"
#include "uas.h"
#include "uasmanager.h"
#include "uasinterface.h"
#include "aq_telemetryView.h"
#include "qwt_plot_marker.h"
#include "qwt_symbol.h"
#include "debugconsole.h"
#include "mavlinkprotocol.h"
#include "mavlinkdecoder.h"

#include <QDebug>
#include <QWidget>
#include <QFileDialog>
#include <QTextBrowser>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QMovie>


const int UAVConfig::i_const[] = {0,0,1,2,3,4,5,6};
const int UAVConfig::x_loca[] = {0,75,150,250,350,450,550,600};
int max_thr, max_yaw, max_pit, max_roll, min_thr, min_yaw, min_pit, min_roll;

UAVConfig::UAVConfig(QWidget *parent) :
    QWidget(parent),
    paramaq(NULL),
    uas(NULL),
    connectedLink(NULL),
    ui(new Ui::UAVConfig)
{
    // strict field name matching of common parameter
    fldnameRx.setPattern("^(COMM|CTRL|DOWNLINK|GMBL|GPS|IMU|L1|MOT|NAV|PPM|RADIO|SIG|SPVR|UKF|VN100|QUATOS|LIC|PF|OSD)_[A-Z0-9_]+$");
    dupeFldnameRx.setPattern("___N[0-9]"); // for having duplicate field names, append ___N# after the field name (three underscores, "N", and a unique number)
    // strict field name matching of profile parameter
    filePF.setPattern("^(KP|KI|KD|KRATE|P|H|D|RATE)_(PITCH|ROLL|YAW|CUT|LEVEL).*");
    ui->setupUi(this);

    //location of update firmware program
    aqBinFolderPath = QCoreApplication::applicationDirPath() + "/aq/bin/";
    platformExeExt = ".exe";
    LastFilePath = settings.value("AUTOQUAD_LAST_PATH").toString();

    for (int i=0; i < 8; ++i) {
        allRadioChanProgressBars << ui->groupBox_Radio_Values->findChild<QProgressBar *>(QString("progressBar_chan_%1").arg(i));
        allRadioChanValueLabels << ui->groupBox_Radio_Values->findChild<QLabel *>(QString("label_chanValue_%1").arg(i));
    }
    allRadioChanCombos.append(ui->frame_RadioTab->findChildren<QComboBox *>(QRegExp("^(RADIO|NAV|GMBL|QUATOS|VIDEO)_.+_(CH|KNOB)")));
    allRadioChanCombos.append(ui->frame_PIDTurning->findChildren<QComboBox *>(QRegExp("^(PF|VIDEO)_.+")));
    foreach (QComboBox* cb, allRadioChanCombos){
        connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(validateRadioSettings(int)));
    }
    // connect some things only after settings are loaded to prevent erroneous signals
    connect(ui->spinBox_rcGraphRefreshFreq, SIGNAL(valueChanged(int)), this, SLOT(delayedSendRcRefreshFreq()));
    connect(uas, SIGNAL(remoteControlRSSIChanged(float)), this, SLOT(setRssiDisplayValue(float)));
    //RC Config
    connect(&delayedSendRCTimer, SIGNAL(timeout()), this, SLOT(sendRcRefreshFreq()));
    delayedSendRCTimer.start(800);


    //new USA created
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(createAQParamWidget(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(UASDeleted(UASInterface*)), this, SLOT(uasDeleted(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(createAQParamWidget(UASInterface*)), Qt::UniqueConnection);

    //Flash Firmware event
    connect(ui->flashButton, SIGNAL(clicked()), this, SLOT(flashFW()));
    connect(ui->SelectFirmwareButton, SIGNAL(clicked()), this, SLOT(selectFWToFlash()));
    //process Slots for Update Firmware
    ps_master.setProcessChannelMode(QProcess::MergedChannels);
    connect(&ps_master, SIGNAL(finished(int)), this, SLOT(prtstexit(int)));
    connect(&ps_master, SIGNAL(readyReadStandardOutput()), this, SLOT(prtstdout()));
    connect(&ps_master, SIGNAL(error(QProcess::ProcessError)), this, SLOT(extProcessError(QProcess::ProcessError)));

    //Charts Feature TABS
    aqtelemetry = new AQTelemetryView(this);
    ui->scrollArea_Charts->setWidget(aqtelemetry);

    //Calibration event
    connect(ui->btn_StartCalib, SIGNAL(clicked()), this, SLOT(startCalib()));


    //Update RADIO type
    //    connect(ui->RADIO_SETUP, SIGNAL(currentIndexChanged(int)), this, SLOT(radioType_changed(int)));
    //    connect(this,SIGNAL(firmwareInfoUpdated()), this, SLOT(setupRadioTypes()));
    //    setupRadioTypes(); //update Radio Type

    //update variable for RC Chart
    rc_rate = 50;

    //Update UI stylesheet
    updateButtonView();
//    load3DModel(); //3D model in IMU Tab
    loadSettings();
    updateImgForRC(); //RC Tabs
    BLHeliTab(); //@Trung BLHeli Tab

    //Communication Console
    ui->scrollArea_debugConsole->setWidget(new DebugConsole(this));
    mavlink = new MAVLinkProtocol();
    mavlinkDecoder = new MAVLinkDecoder(mavlink, this);
    DebugConsole *debugConsole = dynamic_cast<DebugConsole*>(ui->scrollArea_debugConsole->widget());
    connect(mavlinkDecoder, SIGNAL(textMessageReceived(int, int, int, const QString)), debugConsole, SLOT(receiveTextMessage(int, int, int, const QString)));

    //Buton Write: click event
    connect(ui->btn_Save_RC, SIGNAL(clicked()), this, SLOT(saveAQSetting()));
    connect(ui->btn_Write_PitchExpo, SIGNAL(clicked()),this, SLOT(saveAQSetting()));
    connect(ui->btn_Write_RollExpo, SIGNAL(clicked()),this, SLOT(saveAQSetting()));
    connect(ui->btn_Write_YawExpo, SIGNAL(clicked()),this, SLOT(saveAQSetting()));
    connect(ui->btn_Write_PIDTurning, SIGNAL(clicked()),this, SLOT(saveAQSetting()));
    connect(ui->btn_Write_IMU, SIGNAL(clicked()),this, SLOT(saveAQSetting()));

    connect(ui->btn_Read_PitchExpo, SIGNAL(clicked()), this, SLOT(refreshParam()));
    connect(ui->btn_Read_RollExpo, SIGNAL(clicked()), this, SLOT(refreshParam()));
    connect(ui->btn_Read_YawExpo, SIGNAL(clicked()), this, SLOT(refreshParam()));
    connect(ui->btn_Read_TPA, SIGNAL(clicked()), this, SLOT(refreshParam()));
    connect(ui->btn_Read_IMU, SIGNAL(clicked()), this, SLOT(refreshParam()));
    connect(ui->btn_Read_PIDTurning, SIGNAL(clicked()), this, SLOT(refreshParam()));

}

UAVConfig::~UAVConfig()
{
    view.releaseResources();
    delete ui;
}

QString UAVConfig::paramNameGuiToOnboard(QString paraName) {
    paraName = paraName.replace(dupeFldnameRx, "");

    if (!paramaq)
        return paraName;

    // check for old param names
    QString tmpstr;
    if (paraName.indexOf(QRegExp("NAV_ALT_SPED_.+")) > -1 && !paramaq->paramExistsAQ(paraName)){
        tmpstr = paraName.replace(QRegExp("NAV_ALT_SPED_(.+)"), "NAV_ATL_SPED_\\1");
        if (paramaq->paramExistsAQ(tmpstr))
            paraName = tmpstr;
    }
    else if (paraName.indexOf(QRegExp("QUATOS_.+")) > -1 && !paramaq->paramExistsAQ(paraName)) {
        tmpstr = paraName.replace(QRegExp("QUATOS_(.+)"), "L1_ATT_\\1");
        if (paramaq->paramExistsAQ(tmpstr))
            paraName = tmpstr;
    }

    // ignore depricated radio_type param
    if (paraName == "RADIO_TYPE" && useRadioSetupParam)
        paraName += "_void";

    return paraName;
}

void UAVConfig::loadParametersToUI()
{
    useRadioSetupParam = paramaq->paramExistsAQ("RADIO_SETUP");
    emit firmwareInfoUpdated();
    getGUIPara(ui->tab_aq_setting);
}



/**
 * @Leo: update UI
 */
void UAVConfig::getGUIPara(QWidget *parent)
{
    bool ok;
    int precision = 6;
    int tmp;
    QString paraName; //parameter editted
    QString valstr; //chuyen value -> string roi add vao QLineEdit
    QVariant val;
    QList<QWidget*> wdgtList = parent->findChildren<QWidget *>(fldnameRx);
    int ID = 0;
    foreach (QWidget* w, wdgtList)
    {
        //get param name
        paraName = paramNameGuiToOnboard(w->objectName());
        //get param value
        val = paramaq->getParaAQ(paraName);


        /*
         * Update
         */
        ok = true;
        if (paraName == "RADIO_SETUP")
            val = val.toInt() & 0x0f;

        //ComboBox
        if (QComboBox* cb = qobject_cast<QComboBox *>(w))
        {
            if (cb->isEditable())
            {
                if ((tmp = cb->findText(val.toString())) > -1)
                {
                    cb->setCurrentIndex(tmp);
                } else
                {
                    cb->insertItem(0, val.toString());
                    cb->setCurrentIndex(0);
                }
            }
            else if ((tmp = cb->findData(val)) > -1)
            {
                cb->setCurrentIndex(tmp);
            } else {
                cb->setCurrentIndex(abs(val.toInt(&ok)));
            }

            //if QCombobox is Profile with code PF_USING => update ID to get value of this profile
            if(cb->objectName() == "PF_USING")
            {
                ID = cb->currentIndex();
            }
        }
        //Throttle D-band && Pitch/Roll D-band
        else if (QSpinBox* sb = qobject_cast<QSpinBox *>(w))
        {
            sb->setValue(val.toInt((&ok)));
        }
        //QLineEdit
        else if (QLineEdit* le = qobject_cast<QLineEdit *>(w))
        {
            if (le->objectName() == "RADIO_THROT_TPA")
            {
                TPA_breakpoint = val.toFloat();
            }
            //change Float to String
            valstr.setNum(val.toFloat(), 'g', precision);
            le->setText(valstr);
            le->setValidator(new QDoubleValidator(-1000000.0, 1000000.0, 8, le));

        }
        else
            continue;
    }

    //update PID tab with profile ID
    updatePID(parent, ID);

    //RC Charts : Draw immediately after start
    if(ui->RADIO_P_EXPO && ui->RADIO_P_RATE)
        drawChartRC(1);
    if(ui->RADIO_R_EXPO && ui->RADIO_R_RATE)
        drawChartRC(2);
    if(ui->RADIO_YAW_EXPO && ui->RADIO_YAW_RATE)
        drawChartRC(3);
    if (ui->RADIO_THROT_TPA && ui->RADIO_DYN_THROT)
        TPAChart();
}


void UAVConfig::updatePID(QWidget *parent, int pfID)
{
    QString paraName;
    QVariant val;
    QList<QWidget*> wdgtList = parent->findChildren<QWidget *>(filePF);
    QString valstr;
    QString pf;
    if (pfID == 1)
    {
        pf = "PF1_";
    }
    else if (pfID == 2)
    {
        pf = "PF2_";
    }
    else if (pfID == 3)
    {
        pf = "PF3_";
    }
    else if (pfID == 4)
    {
        pf = "PF4_";
    }
    else if (pfID == 5)
    {
        pf = "PF5_";
    }
    else if (pfID == 6)
    {
        pf = "PF6_";
    }
    else if (pfID == 7)
    {
        pf = "PF7_";
    }
    else {
        MainWindow::instance()->showCriticalMessage("Error", "Profile ID must between 1 and 7");
    }
    foreach (QWidget* w, wdgtList)
    {
        //get param name
        QString ap = pf + w->objectName();
        paraName = paramNameGuiToOnboard(ap);

        //get param value
        val = paramaq->getParaAQ(paraName);
        if (QLineEdit* le = qobject_cast<QLineEdit *>(w))
        {
            //change Float to String
            valstr.setNum(val.toFloat(), 'g', 6);
            le->setText(valstr);
            le->setValidator(new QDoubleValidator(-1000000.0, 1000000.0, 8, le));
        }
    }
}

int UAVConfig::calcRadioSetting()
{
    //    int radioSetup = ui->RADIO_SETUP->itemData(ui->RADIO_SETUP->currentIndex()).toInt();
    int radioSetup = 0;
    return radioSetup;
}

/*
 * ========= Update FW ==========
 */

bool UAVConfig::checkAqConnected(bool interactive)
{
    if (!uas || !paramaq || uas->getCommunicationStatus() != uas->COMM_CONNECTED ) {
        if (interactive){
            MainWindow::instance()->showCriticalMessage("Error", "No AutoQuad connected!");
        }
        return false;
    } else
        return true;
}

void UAVConfig::selectFWToFlash()
{
    QString dirPath;
    if ( LastFilePath == "")
        dirPath = QCoreApplication::applicationDirPath();
    else
        dirPath = LastFilePath;
    QFileInfo dir(dirPath);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Firmware File"), dir.absoluteFilePath(),
                                                    tr("AQ firmware") + " (*.hex *.bin)");

    if (fileName.length())
    {
        QString fileNameLocale = QDir::toNativeSeparators(fileName);
        QFile file(fileNameLocale);
        if (!file.open(QIODevice::ReadOnly))
        {
            MainWindow::instance()->showCriticalMessage(tr("Warning!"),
                                                        tr("Could not open firmware file. %1").arg(file.errorString()));
            return;
        }
        ui->fileLabel->setText(fileNameLocale);
        ui->fileLabel->setToolTip(fileNameLocale);
        fileToFlash = file.fileName();
        LastFilePath = fileToFlash;
        file.close();
    }
}

void UAVConfig::flashFW()
{
    if (checkProcRunning())
        return;

    //    QString msg = "";

    //    msg += tr("Make sure your AQ is connected via USB and is already in bootloader mode.  To enter bootloader mode,"
    //              "first connect the BOOT pins (or hold the BOOT button) and then turn the AQ on.\n\n");

    //    msg += "Do you wish to continue flashing?";

    //    QMessageBox::StandardButton qrply = QMessageBox::warning(this, tr("Confirm Firmware Flashing"), msg, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
    //    if (qrply == QMessageBox::Cancel)
    //        return;

    if (connectedLink)
        connectedLink->disconnect();

    activeProcessStatusWdgt = ui->textFlashOutput;
    fwFlashActive = true;

    flashFwDfu();

}

void UAVConfig::flashFwDfu()
{
    QString AppPath = QDir::toNativeSeparators(aqBinFolderPath + "dfu-util" + platformExeExt);
    QStringList Arguments;
    Arguments.append("-a 0");                   // alt 0 is start of internal flash
    Arguments.append("-d 0483:df11" );          // device ident stm32
    Arguments.append("-s 0x08000000:leave");    // start address (:leave to exit DFU mode after flash)
    Arguments.append("-R");                     // reset after upload
    Arguments.append("-D");                     // firmware file
    Arguments.append(QDir::toNativeSeparators(ui->fileLabel->text()));

    QString cmdLine = AppPath;
    foreach (const QString arg, Arguments)
        cmdLine += " " + arg;
    ui->textFlashOutput->append(cmdLine + "\n\n");

    ps_master.start(AppPath , Arguments, QProcess::Unbuffered | QProcess::ReadWrite);
}


bool UAVConfig::checkProcRunning(bool warn)
{
    if (ps_master.state() == QProcess::Running) {
        if (warn)
            MainWindow::instance()->showCriticalMessage(
                        tr("Process already running."),
                        tr("There appears to be an external process (calculation step or firmware flashing) already running. Please abort it first."));
        return true;
    }
    return false;
}

//Print output to textbox
void UAVConfig::prtstexit(int stat)
{
    prtstdout();
    if ( fwFlashActive ) {  // firmware flashing mode
        ui->flashButton->setEnabled(true);
        if (!stat)
            activeProcessStatusWdgt->insertPlainText("Successful. Restart the device.");
        fwFlashActive = false;
        if (connectedLink) {
            connectedLink->connect();
        }
    }
}

void UAVConfig::prtstdout()
{
    QString output = ps_master.readAllStandardOutput();
    if (output.contains(QRegExp("\\[(uWrote|H)"))) {
        output = output.replace(QRegExp(".\\[[uH]"), "");
        activeProcessStatusWdgt->clear();
    }
    activeProcessStatusWdgt->insertPlainText(output);
    activeProcessStatusWdgt->ensureCursorVisible();

    QStringList argument = ps_master.arguments();
    qDebug() << "Argument: " << argument;
}

QString UAVConfig::extProcessError(QProcess::ProcessError err)
{
    QString msg;
    switch(err) {
    case QProcess::FailedToStart:
        msg = tr("Failed to start.");
        break;
    case QProcess::Crashed:
        msg = tr("Process terminated (aborted or crashed).");
        break;
    case QProcess::Timedout:
        msg = tr("Timeout waiting for process.");
        break;
    case QProcess::WriteError:
        msg = tr("Cannot write to process, exiting.");
        break;
    case QProcess::ReadError:
        msg = tr("Cannot read from process, exiting.");
        break;
    default:
        msg = tr("Unknown error");
        break;
    }
    return msg;
}

/*
 * ================= Radio channels display ==================
 */

void UAVConfig::toggleRadioValuesUpdate(bool enable)
{
    if (!uas)
        enable = false;

    ui->toolButton_toggleRadioGraph->setChecked(enable);
    ui->conatiner_radioGraphValues->setEnabled(enable);

    if (!enable)
        return;

    int min, max, tmin, tmax;

    //    if ( ui->checkBox_raw_value->isChecked() ){
    //        tmax = 1500;
    //        tmin = -100;
    //        max = 1024;
    //        min = -1024;
    //    } else {
    tmax = -500;
    tmin = 1500;
    max = -1500;
    min = 1500;
    //    }

    foreach (QProgressBar* pb, allRadioChanProgressBars) {
        if (pb->objectName().contains("chan_0")) {
            pb->setMaximum(tmax);
            pb->setMinimum(tmin);
            qDebug() << "channel 0: " << pb->maximum() << pb->minimum();
        } else {
            pb->setMaximum(max);
            pb->setMinimum(min);
            qDebug() << "channel: " << pb->maximum() << pb->minimum();

        }
    }
}

void UAVConfig::onToggleRadioValuesRefresh(const bool on)
{
    if (!on || !uas)
        toggleRadioValuesUpdate(false);
    else if (!ui->spinBox_rcGraphRefreshFreq->value())
        ui->spinBox_rcGraphRefreshFreq->setValue(1);

    toggleRadioStream(on);
}


void UAVConfig::toggleRadioStream(int r)
{
    if (uas)
        uas->enableRCChannelDataTransmission(r);
}

void UAVConfig::delayedSendRcRefreshFreq()
{
    delayedSendRCTimer.start();
}

void UAVConfig::sendRcRefreshFreq()
{
    delayedSendRCTimer.stop();
    toggleRadioValuesUpdate(ui->spinBox_rcGraphRefreshFreq->value());
    toggleRadioStream(ui->spinBox_rcGraphRefreshFreq->value());
}

void UAVConfig::setRadioChannelDisplayValue(int channelId, float normalized)
{
    int val;
    QString lblTxt;

    if (!ui->conatiner_radioGraphValues->isEnabled())
        toggleRadioValuesUpdate(true);

    if (channelId >= allRadioChanProgressBars.size())
        return;

    QProgressBar* bar = allRadioChanProgressBars.at(channelId);
    QLabel* lbl = allRadioChanValueLabels.at(channelId);

    // Scaled values
    val = (int)((normalized*10000.0f)/13);
    if (channelId == 0)
        val += 750;

    if (lbl) {
        lblTxt.sprintf("%+d", val);
        lbl->setText(lblTxt);
    }
    if (bar) {
        if (val > bar->maximum())
            val = bar->maximum();
        if (val < bar->minimum())
            val = bar->minimum();

        bar->setValue(val);
    }
}

void UAVConfig::setRssiDisplayValue(float normalized)
{
    QProgressBar* bar = ui->progressBar_rssi;
    int val = (int)(normalized);

    if (bar && val <= bar->maximum() && val >= bar->minimum())
        bar->setValue(val);
}

/*
 * ======================== UAS =====================
 */
void UAVConfig::uasConnected()
{
    uas->sendCommmandToAq(MAV_CMD_AQ_REQUEST_VERSION, 1);
}

void UAVConfig::uasDeleted(UASInterface *mav)
{
    if (uas && mav && mav == uas) {
        removeActiveUAS();
        toggleRadioValuesUpdate(false);
    }
}

void UAVConfig::removeActiveUAS()
{
    if (uas) {
        disconnect(uas, 0, this, 0);
        if (paramaq) {
            disconnect(paramaq, 0, this, 0);
            paramaq->deleteLater();
            paramaq = NULL;
        }
        uas = NULL;
        toggleRadioValuesUpdate(false);
    }
}

void UAVConfig::handleStatusText(int uasId, int compid, int severity, QString text)
{
    Q_UNUSED(severity);
    Q_UNUSED(compid);
    QRegExp versionRe("^(?:A.*Q.*: )?(\\d+\\.\\d+(?:\\.\\d+)?)([\\s\\-A-Z]*)(?:r(?:ev)?(\\d{1,5}))?(?: b(\\d+))?,?(?: (?:HW ver: (\\d) )?(?:hw)?rev(\\d))?\n?$");
    QString aqFirmwareVersionQualifier;
    bool ok;

    // parse version number
    if (uasId == uas->getUASID()) {
        if (text.contains(versionRe)) {
            QStringList vlist = versionRe.capturedTexts();
            //        qDebug() << vlist.at(1) << vlist.at(2) << vlist.at(3) << vlist.at(4) << vlist.at(5);
            aqFirmwareVersion = vlist.at(1);
            aqFirmwareVersionQualifier = vlist.at(2);
            aqFirmwareVersionQualifier.replace(QString(" "), QString(""));
            if (vlist.at(3).length()) {
                aqFirmwareRevision = vlist.at(3).toInt(&ok);
                if (!ok) aqFirmwareRevision = 0;
            }
            if (vlist.at(4).length()) {
                aqBuildNumber = vlist.at(4).toInt(&ok);
                if (!ok) aqBuildNumber = 0;
            }
            if (vlist.at(5).length()) {
                aqHardwareVersion = vlist.at(5).toInt(&ok);
                if (!ok) aqHardwareVersion = 0;
            }
            if (vlist.at(6).length()) {
                aqHardwareRevision = vlist.at(6).toInt(&ok);
                if (!ok) aqHardwareRevision = -1;
            }

            setHardwareInfo();
            setFirmwareInfo();

            if (aqFirmwareVersion.length()) {
                QString verStr = QString("AutoQuad FW: v. %1%2").arg(aqFirmwareVersion).arg(aqFirmwareVersionQualifier);
                if (aqFirmwareRevision > 0)
                    verStr += QString(" r%1").arg(QString::number(aqFirmwareRevision));
                if (aqBuildNumber > 0)
                    verStr += QString(" b%1").arg(QString::number(aqBuildNumber));
                verStr += QString(" HW: v. %1").arg(QString::number(aqHardwareVersion));
                if (aqHardwareRevision > -1)
                    verStr += QString(" r%1").arg(QString::number(aqHardwareRevision));

                //                ui->lbl_aq_fw_version->setText(verStr);
            }
//            else
                //                ui->lbl_aq_fw_version->setText("AutoQuad Firmware v. [unknown]");
                //        }
                //        else if (text.contains("Quatos enabled", Qt::CaseInsensitive)) {
                //            setAqHasQuatos(true);
        }
    }
}

void UAVConfig::setHardwareInfo()
{
    pwmPortTimers.clear();
    switch (aqHardwareVersion) {
    case 8:
        maxPwmPorts = 9;
        pwmPortTimers << 3 << 3 << 4 << 4 << 4 << 4 << 8 << 8 << 9;
        break;
    case 7:
        maxPwmPorts = 9;
        pwmPortTimers << 1 << 1 << 1 << 1 << 4 << 4 << 9 << 9 << 11;
        break;
    case 6:
    default:
        maxPwmPorts = 14;
        pwmPortTimers << 1 << 1 << 1 << 1 << 4 << 4 << 4 << 4 << 9 << 9 << 2 << 2 << 10 << 11;
        break;
    }
    emit hardwareInfoUpdated();
}

void UAVConfig::setFirmwareInfo()
{
    if (aqBuildNumber) {
        if (aqBuildNumber < 1663)
            motPortTypeCAN_H = false;

        if (aqBuildNumber < 1423)
            maxMotorPorts = 14;

        if (aqBuildNumber < 1418)
            motPortTypeCAN = false;

        if (aqBuildNumber < 1790)
            useRadioSetupParam = false;

        if (aqBuildNumber >= 1740)
            aqCanReboot = true;

    }
    emit firmwareInfoUpdated();
}

// setActiveUAS()
void UAVConfig::createAQParamWidget(UASInterface *uastmp)
{
    if (!uastmp)
        return;

    removeActiveUAS();
    uas = uastmp;
    paramaq = new AQParamWidget(uas, this);

    //RC message
    connect(uas, SIGNAL(textMessageReceived(int,int,int,QString)), this, SLOT(handleStatusText(int, int, int, QString)));
    connect(uas, SIGNAL(remoteControlRSSIChanged(float)), this, SLOT(setRssiDisplayValue(float)));

    connect(uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this, SLOT(setRadioChannelDisplayValue(int,float)));
    connect(paramaq, SIGNAL(requestParameterRefreshed()), this, SLOT(loadParametersToUI()));
    connect(paramaq, SIGNAL(parameterListRequested()), this, SLOT(uasConnected()));

    paramaq->requestParameterList();
    if (DebugConsole *debugConsole = dynamic_cast<DebugConsole*>(ui->scrollArea_debugConsole->widget()))
        connect(uas, SIGNAL(textMessageReceived(int,int,int,QString)), debugConsole, SLOT(receiveTextMessage(int,int,int,QString)));

}

/*
 * ================= Save AQSetting ===============
 */

void UAVConfig::loadSettings()
{
    settings.beginGroup("AUTOQUAD_SETTINGS");

    // if old style Aq.ini file exists, copy settings to QGC shared storage
    if (QFile("Aq.ini").exists()) {
        QSettings aq_settings("Aq.ini", QSettings::IniFormat);
        aq_settings.beginGroup("AUTOQUAD_SETTINGS");
        foreach (QString childKey, aq_settings.childKeys())
            settings.setValue(childKey, aq_settings.value(childKey));
        settings.sync();
        QFile("Aq.ini").rename("Aq.ini.bak");
        //        qDebug() << "Copied settings from Aq.ini to QGC shared config storage.";
    }

    if (settings.contains("AUTOQUAD_FW_FILE") && settings.value("AUTOQUAD_FW_FILE").toString().length()) {
        ui->fileLabel->setText(settings.value("AUTOQUAD_FW_FILE").toString());
        ui->fileLabel->setToolTip(settings.value("AUTOQUAD_FW_FILE").toString());
        //        ui->checkBox_verifyFwFlash->setChecked(settings.value("AUTOQUAD_FW_VERIFY", true).toBool());
    }

    LastFilePath = settings.value("AUTOQUAD_LAST_PATH").toString();

    settings.endGroup();
    settings.sync();
}

void UAVConfig::saveAQSetting()
{
    if (!validateRadioSettings(0)) {
        MainWindow::instance()->showCriticalMessage(tr("Error"), tr("You have the same port assigned to multiple controls!"));
        return;
    }
    saveSettingsToAq(ui->tab_aq_setting);
}

void UAVConfig::refreshParam()
{
    if ( !checkAqConnected(true) )
        return;
    paramaq->requestParameterList();
}

void UAVConfig::TabRadio()
{
    ui->tab_aq_setting->setCurrentIndex(0);
    updateButtonView();
}

void UAVConfig::TabMotor()
{
    ui->tab_aq_setting->setCurrentIndex(1);
    updateButtonView();
}

void UAVConfig::TabIMU()
{
    ui->tab_aq_setting->setCurrentIndex(2);
    updateButtonView();
}

void UAVConfig::TabPID()
{
    ui->tab_aq_setting->setCurrentIndex(3);
    updateButtonView();
}

void UAVConfig::TabChart()
{
    ui->tab_aq_setting->setCurrentIndex(4);
    updateButtonView();
}

void UAVConfig::TabOSD()
{
    ui->tab_aq_setting->setCurrentIndex(5);
    updateButtonView();
}

void UAVConfig::TabUpgrade()
{
    ui->tab_aq_setting->setCurrentIndex(6);
    updateButtonView();
}

void UAVConfig::TabBLHeli()
{
    ui->tab_aq_setting->setCurrentIndex(7);
    updateButtonView();
}

bool UAVConfig::validateRadioSettings(int)
{
    QList<QString> conflictPorts, portsUsed, essentialPorts;
    QString cbname, cbtxt;
    bool ok = true;

    foreach (QComboBox* cb, allRadioChanCombos) {
        cbname = cb->objectName();
        cbtxt = cb->currentText();
        if (cbtxt.contains(tr("Off"), Qt::CaseInsensitive))
            continue;
        if (portsUsed.contains(cbtxt))
            conflictPorts.append(cbtxt);
        if (cbname.contains(QRegExp("^RADIO_(THRO|PITC|ROLL|RUDD|FLAP|AUX2)_CH")))
            essentialPorts.append(cbtxt);
        portsUsed.append(cbtxt);
    }

    foreach (QComboBox* cb, allRadioChanCombos) {
        if (conflictPorts.contains(cb->currentText())) {
            if (essentialPorts.contains(cb->currentText())) {
                cb->setStyleSheet("background-color: rgba(255, 0, 0, 160)");
                ok = false;
            } else
                cb->setStyleSheet("background-color: rgba(255, 140, 0, 130)");
        } else
            cb->setStyleSheet("");
    }

    return ok;
}

void UAVConfig::saveDialogButtonClicked(QAbstractButton *btn)
{
    paramSaveType = 0;
    if (btn->objectName() == "btn_saveToRam")
        paramSaveType = 1;
    else if (btn->objectName() == "btn_saveToRom")
        paramSaveType = 2;
}

/*
 * @Leo: Write param to Board
 */


bool UAVConfig::saveSettingsToAq(QWidget *parent, bool interactive)
{
    float val_uas, val_local;
    QString paraName, msg;
    QStringList errors;
    bool ok, chkstate;
    quint8 errLevel = 0;  // 0=no error; 1=soft error; 2=hard error
    QList<float> changeVals;
    QMap<QString, QList<float> > changeList; // param name, old val, new val
    QVariant tmp;

    if ( !checkAqConnected(interactive) )
        return false;

    QList<QWidget*> wdgtList = parent->findChildren<QWidget *>(fldnameRx);
    QList<QObject*> objList = *reinterpret_cast<QList<QObject *>*>(&wdgtList);
    if (!QString::compare(parent->objectName(), "tab_aq_settings")) {
        QList<QButtonGroup *> grpList = this->findChildren<QButtonGroup *>(fldnameRx);
        objList.append(*reinterpret_cast<QList<QObject *>*>(&grpList));
    }

    foreach (QObject* w, objList) {
        paraName = paramNameGuiToOnboard(w->objectName());

        if (!paramaq->paramExistsAQ(paraName))
            continue;

        ok = true;
        val_uas = paramaq->getParaAQ(paraName).toFloat(&ok);

        if (QLineEdit* le = qobject_cast<QLineEdit *>(w))
            val_local = le->text().toFloat(&ok);
        else if (QComboBox* cb = qobject_cast<QComboBox *>(w)) {
            if (cb->isEditable()) {
                val_local = cb->currentText().toFloat(&ok);
            }
            else {
                tmp = cb->itemData(cb->currentIndex());
                if (tmp.isValid())
                    val_local = tmp.toFloat(&ok);
                else
                    val_local = static_cast<float>(cb->currentIndex());
            }
        } else if (QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox *>(w))
            val_local = (float)sb->value();
        else if (QSpinBox* sb = qobject_cast<QSpinBox *>(w))
            val_local = (float)sb->value();
        else if (QButtonGroup* bg = qobject_cast<QButtonGroup *>(w)) {
            val_local = 0.0f;
            foreach (QAbstractButton* abtn, bg->buttons()) {
                if (abtn->isChecked()) {
                    if (bg->exclusive()) {
                        val_local = bg->id(abtn);
                        break;
                    } else
                        val_local += bg->id(abtn);
                }
            }
        }
        else
            continue;

        if (!ok){
            errors.append(paraName);
            continue;
        }

        // special case for reversing gimbal servo direction
        if (paraName == "GMBL_SCAL_PITCH" || paraName == "GMBL_SCAL_ROLL" || paraName == "SIG_BEEP_PRT") {
            if (paraName == "GMBL_SCAL_PITCH")
                chkstate = parent->findChild<QCheckBox *>("reverse_gimbal_pitch")->checkState();
            else if (paraName == "GMBL_SCAL_ROLL")
                chkstate = parent->findChild<QCheckBox *>("reverse_gimbal_roll")->checkState();
            else if (paraName == "SIG_BEEP_PRT")
                chkstate = parent->findChild<QCheckBox *>("checkBox_useSpeaker")->checkState();

            if (chkstate)
                val_local = 0.0f - val_local;
        }

        // FIXME with a real float comparator
        if (val_uas != val_local) {
            changeVals.clear();
            changeVals.append(val_uas);
            changeVals.append(val_local);
            changeList.insert(paraName, changeVals);
        }
    }

    if (errors.size()) {
        errors.insert(0, tr("One or more parameter(s) could not be saved:"));
        if (errors.size() >= changeList.size())
            errLevel = 2;
        else
            errLevel = 1;
    }

    if ( changeList.size() ) {
        paramSaveType = 1;  // save to volatile

        if (interactive) {
            paramSaveType = 0;

            QString msgBoxText = tr("%n parameter(s) modified:<br>", "one or more params have changed", changeList.size());
            msg = tr("<table border=\"0\"><thead><tr><th>Parameter </th><th>Old Value </th><th>New Value </th></tr></thead><tbody>\n");
            QMapIterator<QString, QList<float> > i(changeList);
            QString val1, val2, restartFlag;
            while (i.hasNext()) {
                i.next();
                val1.setNum(i.value().at(0), 'g', 8);
                val2.setNum(i.value().at(1), 'g', 8);

                msg += QString("<tr><td style=\"padding: 1px 7px 0 1px;\"><span style=\"color: rgba(255, 0, 0, 200); font-weight: bold; border-color: #383127;\">%1</span>%2</td><td>%3 </td><td>%4</td></tr>\n").arg(restartFlag, i.key(), val1, val2);
            }
            msg += "</tbody></table>\n";
            QDialog* dialog = new QDialog(this);
            dialog->setSizeGripEnabled(true);
            dialog->setWindowTitle(tr("Verify Changed Parameters"));
            dialog->setWindowModality(Qt::ApplicationModal);
            dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

            QSizePolicy sizepol(QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::Label);
            sizepol.setVerticalStretch(0);
            QLabel* prompt = new QLabel(msgBoxText, dialog);
            prompt->setTextFormat(Qt::RichText);
            prompt->setSizePolicy(sizepol);
            QLabel* prompt2 = new QLabel(tr("Do you wish to continue?"), dialog);
            prompt2->setSizePolicy(sizepol);

            QTextEdit* message = new QTextEdit(msg, dialog);
            message->setReadOnly(true);
            message->setAcceptRichText(true);

            QDialogButtonBox* bbox = new QDialogButtonBox(Qt::Horizontal, dialog);
            QPushButton *btn_saveToRam = bbox->addButton(tr("Save to RAM"), QDialogButtonBox::AcceptRole);
            btn_saveToRam->setToolTip(tr("The settings will be immediately active and persist UNTIL the flight controller is restarted."));
            btn_saveToRam->setObjectName("btn_saveToRam");
            btn_saveToRam->setAutoDefault(false);
            QPushButton *btn_saveToRom = bbox->addButton(tr("Save to ROM"), QDialogButtonBox::AcceptRole);
            btn_saveToRom->setToolTip(tr("The settings will be immediately active and persist AFTER flight controller is restarted."));
            btn_saveToRom->setObjectName("btn_saveToRom");
            btn_saveToRom->setAutoDefault(false);
            QPushButton *btn_cancel = bbox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
            btn_cancel->setToolTip(tr("Do not save any settings."));
            btn_cancel->setDefault(true);

            QVBoxLayout* dlgLayout = new QVBoxLayout(dialog);
            dlgLayout->setSpacing(8);
            dlgLayout->addWidget(prompt);
            dlgLayout->addWidget(message);
            QHBoxLayout* promptLayout = new QHBoxLayout;
            promptLayout->setSpacing(8);
            promptLayout->addWidget(prompt2);
            dlgLayout->addLayout(promptLayout);
            dlgLayout->addWidget(bbox);

            dialog->setLayout(dlgLayout);

            connect(btn_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
            connect(btn_saveToRam, SIGNAL(clicked()), dialog, SLOT(accept()));
            connect(btn_saveToRom, SIGNAL(clicked()), dialog, SLOT(accept()));
            connect(bbox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(saveDialogButtonClicked(QAbstractButton*)));

            bool dlgret = dialog->exec();
            dialog->deleteLater();

            if (dlgret == QDialog::Rejected || !paramSaveType)
                return false;

        }

        QMapIterator<QString, QList<float> > i(changeList);
        while (i.hasNext()) {
            i.next();
            paramaq->setParaAQ(i.key(), i.value().at(1));
        }

        if (paramSaveType == 2) {
            uas->writeParametersToStorageAQ();
        }

        return true;
    } else {
        if (interactive){
            MainWindow::instance()->showCriticalMessage(tr("Warning"), tr("No changed parameters detected.  Nothing saved."));
        }
        return false;
    }
}

bool UAVConfig::checkAqSerialConnection(QString port)
{
    bool IsConnected = false;
    connectedLink = NULL;

    if (!checkAqConnected(false))
        return false;

    if ( uas != NULL ) {
        for ( int i=0; i < uas->getLinks()->count(); i++) {
            connectedLink = uas->getLinks()->at(i);
            if ( connectedLink->isConnected() == true && (port == "" ||  connectedLink->getName().contains(port))) {
                IsConnected = true;
                break;
            }
        }
    }

    if (!IsConnected)
        connectedLink = NULL;

    return IsConnected;
}

void UAVConfig::setValueLineEdit(QString str)
{
    Q_UNUSED(str);
    //    int val = str.toInt();
    //    ui->slider_CTRL_YAW_RTE_D->setValue((int)((val)*100/1000));
}

void UAVConfig::updateTextEdit(int i)
{
    Q_UNUSED(i);
    //    i = i*1000/100;
    //    QString str;
    //    str.setNum(i, 10);
    //    ui->CTRL_YAW_RTE_D->setText(str);
}

void UAVConfig::updateButtonView()
{
    int index = ui->tab_aq_setting->currentIndex();
    switch (index) {
    case 0:
    {
        emit TabClicked(0);
        break;
    }
    case 1:
    {
        emit TabClicked(1);
        break;
    }
    case 2:
    {
        emit TabClicked(2);
        break;
    }
    case 3:
    {
        emit TabClicked(3);
        break;
    }
    case 4:
    {
        emit TabClicked(4);
        break;
    }
    case 5:
    {
        emit TabClicked(5);
        break;
    }
    case 6:
    {
        emit TabClicked(6);
        break;
    }
    case 7:
    {
        emit TabClicked(7);
        break;
    }
    default:
    {
        break;
    }
        break;
    }
}

/*
 * ================= RC-Config ===============
 */

void UAVConfig::updateImgForRC()
{
    QPixmap left(":/images/redpoint.png");
    ui->label_redpoint_left->setPixmap(left);
    ui->label_redpoint_right->setPixmap(left);
    ui->label_redpoint_left->setScaledContents(true);
    ui->label_redpoint_right->setScaledContents(true);
    movie_left = new QMovie(":/images/arr_left.gif");
    movie_right = new QMovie(":/images/arr_right.gif");
    movie_up = new QMovie(":/images/arr_up.gif");
    movie_down = new QMovie(":/images/arr_down.gif");
    movie_up_160 = new QMovie(":/images/arr_up_160.gif");
    movie_down_160 = new QMovie(":/images/arr_down_160.gif");
    movie_left_160 = new QMovie(":/images/arr_left_160.gif");
    ui->label_left_1->setMovie(movie_down);
    ui->label_left_2->setMovie(movie_up);
    ui->label_right_1->setMovie(movie_down);
    ui->label_right_2->setMovie(movie_up);
    ui->label_bottom_left_1->setMovie(movie_right);
    ui->label_bottom_left_2->setMovie(movie_left);
    ui->label_bottom_right_1->setMovie(movie_right);
    ui->label_bottom_right_2->setMovie(movie_left);
    ui->label_left_160->hide();
    ui->label_right_160->hide();
    ui->label_bottom_left->hide();
    ui->label_bottom_right->hide();
    movie_left->start();
    movie_right->start();
    movie_up->start();
    movie_down->start();
}



/*
 * @Leo Pitch Roll Yaw Expo Chart Plot
 */

void UAVConfig::drawChartRC(int id)
{
    QString expo_str;
    QString rate_str;
    //Pitch
    if (id == 1)
    {
        expo_str = ui->RADIO_P_EXPO->text();
        expo = expo_str.toInt();

        rate_str = ui->RADIO_P_RATE->text();
        rate = rate_str.toInt();
    }
    //Roll
    else if (id == 2)
    {
        expo_str = ui->RADIO_R_EXPO->text();
        expo = expo_str.toInt();

        rate_str = ui->RADIO_R_RATE->text();
        rate = rate_str.toInt();
    }
    //Yaw
    else if (id == 3)
    {
        expo_str = ui->RADIO_YAW_EXPO->text();
        expo = expo_str.toInt();

        rate_str = ui->RADIO_YAW_RATE->text();
        rate = rate_str.toInt();
    }

    if(expo < 29 || expo > 100 || rate < 29 || rate > 100)
    {
        MainWindow::instance()->showCriticalMessage(tr("Error"), tr("Please input Expo Pitch and Rate Pitch between 30 and 99"));
    }
    else
    {
        //calculate result1
        for (int i = 0; i <= 7; i++)
        {
            result1[i] = (float)(2500 + expo*(i*i-25))*i*rate/2500;
        }
        //calculate y location
        for (int j = 0; j <= 7; j++)
        {
            y_loca[j] = (float)result1[j] + (x_loca[j] - i_const[j]*100)*(result1[j+1] - result1[j])/100;
        }

        QPolygonF poly;
        for (int i = 0; i<=7; i++)
        {
            poly << QPointF(x_loca[i], y_loca[i]);
        }

        QWidget *widget = new QWidget();
        QwtPlot *plot = new QwtPlot(widget);

        //Point Marker
        for (int i = 0; i<=7; i++)
        {
            QwtPlotMarker *mark = new QwtPlotMarker();
            mark->setValue(QPointF(x_loca[i], y_loca[i]));
            mark->attach(plot);
        }

        //Grid
        QwtPlotGrid *grid = new QwtPlotGrid();
        grid->attach(plot);
        grid->setPen( Qt::gray, 0.0, Qt::DotLine );

        //Curve
        QwtPlotCurve *c = new QwtPlotCurve();
        c->setPen( QPen(QColor(102,153,255), 4));
        c->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        c->setSamples(poly);
        c->attach(plot);

        plot->resize(480,255);
        plot->replot();
        plot->show();

        //Pitch
        if (id == 1)
        {
            ui->scrollArea_ChartsPitchExpo->setWidget(widget);
        }
        //Roll
        else if (id == 2)
        {
            ui->scrollArea_ChartsRollExpo->setWidget(widget);
        }
        //Yaw
        else if (id == 3)
        {
            ui->scrollArea_ChartsYawExpo->setWidget(widget);
        }
    }
}


/*
 * @trung TPA Chart Plot
 */

void UAVConfig::TPAChart()
{
    if (ui->RADIO_DYN_THROT->text() == NULL || ui->RADIO_THROT_TPA->text()==NULL)
    {
        MainWindow::instance()->showCriticalMessage(tr("Error"), tr("TPA or TPA Breakpoint value is empty"));
    }
    TPA = ui->RADIO_DYN_THROT->text().toFloat();

    //    TPA_breakpoint = ui->RADIO_THROT_TPA->text().toInt();

    if(TPA < 0 || TPA > 1)
    {
        MainWindow::instance()->showCriticalMessage(tr("Error"), tr("Please input TPA between 0 and 1"));
    }
    else if (TPA_breakpoint < 1000 || TPA_breakpoint > 2000){
        MainWindow::instance()->showCriticalMessage(tr("Error"), tr("Please input TPA Breakpoint between 1000 and 2000"));
    }
    else
    {
        QPolygonF poly;
        poly << QPointF(0, 100) << QPointF(TPA_breakpoint, 100) << QPointF(2000, ((1-TPA)*100));

        QWidget *widget = new QWidget();
        QwtPlot *plot = new QwtPlot(widget);
        plot->setAxisScale(QwtPlot::yLeft, 0, 100, 10);
        plot->setAxisScale(QwtPlot::xBottom, 1000, 2000, 250);

        //Grid
        QwtPlotGrid *grid = new QwtPlotGrid();
        grid->attach(plot);
        grid->setPen( Qt::gray, 0.0, Qt::DotLine );

        //Curve
        QwtPlotCurve *c = new QwtPlotCurve();
        c->setPen( QPen(QColor(102,153,255), 4));
        c->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        c->setSamples(poly);
        c->attach(plot);

        plot->resize(480,255);
        plot->replot();
        plot->show();

        ui->scrollArea_Charts_TPA->setWidget(widget);
    }
}

/*
 * 3DModel: IMU
 */
void UAVConfig::load3DModel()
{
    //3D Model: load qml and connect between QML & C++
    view.releaseResources();
    QWidget *container = QWidget::createWindowContainer(&view);
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.clearBeforeRendering();
    view.engine()->clearComponentCache();
    view.rootContext()->setContextProperty("drone",&drone); //connect QML & C++
    view.setSource(QUrl("qrc:/src/main.qml")); //load QML file
    ui->scrollArea_3D->setWidget(container);

}

void UAVConfig::radioType_changed(int idx)
{
    //    Q_UNUSED(idx);
    //    emit hardwareInfoUpdated();

    //    bool ok;
    //    int prevRadioValue;
    //    int newRadioValue;

    //    if (useRadioSetupParam) {
    //        prevRadioValue = paramaq->getParaAQ("RADIO_SETUP").toInt(&ok);
    //        newRadioValue = calcRadioSetting();
    //    }
}

void UAVConfig::startCalib()
{
    if (!uas)
        return;
    uas->sendCommmandToAq(MAV_CMD_PREFLIGHT_CALIBRATION, 1,0.0f, 0.0f, 0.0f, 1.0f);
}

void UAVConfig::setupRadioTypes()
{
    //    uint8_t idx = ui->RADIO_SETUP->currentIndex();

    //    //Radio Type Available
    //    QMap<int, QString> radioTypes;
    //    radioTypes.insert(0, tr("No Radio"));
    //    radioTypes.insert(1, tr("Spektrum 11Bit"));
    //    radioTypes.insert(2, tr("Spektrum 10Bit"));
    //    radioTypes.insert(3, tr("S-BUS (Futaba, others)"));
    //    radioTypes.insert(4, tr("PPM"));

    //    ui->RADIO_SETUP->blockSignals(true);

    //    ui->RADIO_SETUP->clear();

    //    QMapIterator<int, QString> i(radioTypes);
    //    while(i.hasNext()) {
    //        i.next();
    //        ui->RADIO_SETUP->addItem(i.value(), i.key());
    //    }

    //    if (idx < ui->RADIO_SETUP->count())
    //        ui->RADIO_SETUP->setCurrentIndex(idx);

    //    ui->RADIO_SETUP->blockSignals(false);

}


void UAVConfig::BLHeliTab()
{
    // Initialize current and default value
    default_beep = ui->slide_beep->value();
    default_beaconstr = ui->slide_beaconstr->value();
    default_delay = ui->slide_delay->value();
    default_demeg = ui->slide_demeg->value();
    default_enable = ui->slide_enable->value();
    default_motor = ui->slide_motor->value();
    default_polarity = ui->slide_polarity->value();
    default_pwm = ui->slide_PWM->value();
    default_startup = ui->slide_startup->value();
    default_tempe = ui->slide_tempe->value();

    current_beep = default_beep;
    current_beaconstr = default_beaconstr;
    current_delay = default_delay;
    current_demeg = default_demeg;
    current_enable = default_enable;
    current_motor = default_motor;
    current_polarity = default_polarity;
    current_pwm = default_pwm;
    current_startup = default_startup;
    current_tempe = default_tempe;

    // Set invisible undo button and add connect
    ui->undo_beep->setVisible(false);
    connect(ui->slide_beep,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelBeep(int)));
    ui->undo_delay->setVisible(false);
    connect(ui->slide_delay,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelDelay(int)));
    ui->undo_demeg->setVisible(false);
    connect(ui->slide_demeg,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelDemeg(int)));
    ui->undo_enable->setVisible(false);
    connect(ui->slide_enable,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelEnable(int)));
    ui->undo_motor->setVisible(false);
    connect(ui->slide_motor,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelMotor(int)));
    ui->undo_polarity->setVisible(false);
    connect(ui->slide_polarity,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelPolarity(int)));
    ui->undo_PWM->setVisible(false);
    connect(ui->slide_PWM,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelPWM(int)));
    ui->undo_startup->setVisible(false);
    connect(ui->slide_startup,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelStartup(int)));
    ui->undo_beaconstr->setVisible(false);
    connect(ui->slide_beaconstr,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelBeaconStr(int)));
    ui->undo_tempe->setVisible(false);
    connect(ui->slide_tempe,SIGNAL(valueChanged(int)), this, SLOT(set_Value_Title_LabelTempe(int)));

    // Invisible default button
    ui->default_beep->setVisible(false);
    ui->default_delay->setVisible(false);
    ui->default_demeg->setVisible(false);
    ui->default_enable->setVisible(false);
    ui->default_motor->setVisible(false);
    ui->default_polarity->setVisible(false);
    ui->default_PWM->setVisible(false);
    ui->default_startup->setVisible(false);
    ui->default_beaconstr->setVisible(false);
    ui->default_tempe->setVisible(false);

    // Set alight left
    ui->horizontalLayout_beep->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_delay->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_demeg->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_enable->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_motor->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_polarity->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_pwm->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_startup->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_beaconstr->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_tempe->setAlignment(Qt::AlignLeft);

    // connect undo and default button
    connect(ui->default_beep, SIGNAL(clicked(bool)), this, SLOT(handle_default_beep(bool)));
    connect(ui->default_beaconstr, SIGNAL(clicked(bool)), this, SLOT(handle_default_beaconstr(bool)));
    connect(ui->default_delay, SIGNAL(clicked(bool)), this, SLOT(handle_default_delay(bool)));
    connect(ui->default_demeg, SIGNAL(clicked(bool)), this, SLOT(handle_default_demeg(bool)));
    connect(ui->default_enable, SIGNAL(clicked(bool)), this, SLOT(handle_default_enable(bool)));
    connect(ui->default_motor, SIGNAL(clicked(bool)), this, SLOT(handle_default_motor(bool)));
    connect(ui->default_polarity, SIGNAL(clicked(bool)), this, SLOT(handle_default_polarity(bool)));
    connect(ui->default_PWM, SIGNAL(clicked(bool)), this, SLOT(handle_default_pwm(bool)));
    connect(ui->default_startup, SIGNAL(clicked(bool)), this, SLOT(handle_default_startup(bool)));
    connect(ui->default_tempe, SIGNAL(clicked(bool)), this, SLOT(handle_default_tempe(bool)));
}

// tab BLHeli
// Set visible when slide's value changed
// Set value and title label

void UAVConfig::set_Value_Title_LabelBeep(int value)
{
    ui->undo_beep->setVisible(true);
    ui->default_beep->setVisible(true);
    ui->label_beepValue->setText(QString::number(value));
    ui->label_beepTitle->setText(QString::number(value));
}

void UAVConfig::set_Value_Title_LabelDelay(int value)
{
    ui->undo_delay->setVisible(true);
    ui->default_delay->setVisible(true);
    ui->label_delayValue->setText(QString::number(value));
    switch (value) {
    case 1:
        ui->label_delayTitle->setText("1m");
        break;
    case 2:
        ui->label_delayTitle->setText("2m");
        break;
    case 3:
        ui->label_delayTitle->setText("5m");
        break;
    case 4:
        ui->label_delayTitle->setText("10m");
        break;
    case 5:
        ui->label_delayTitle->setText("Infinite");
        break;
    }
}

void UAVConfig::set_Value_Title_LabelDemeg(int value)
{
    ui->undo_demeg->setVisible(true);
    ui->default_demeg->setVisible(true);
    ui->label_demegValue->setText(QString::number(value));
    switch (value) {
    case 1:
        ui->label_demegTitle->setText("Off");
        break;
    case 2:
        ui->label_demegTitle->setText("Low");
        break;
    case 3:
        ui->label_demegTitle->setText("High");
        break;
    }
}

void UAVConfig::set_Value_Title_LabelEnable(int value)
{
    ui->undo_enable->setVisible(true);
    ui->default_enable->setVisible(true);
    ui->label_enableValue->setText(QString::number(value));
    if (value == 1)
        ui->label_enableTitle->setText("Off");
    else ui->label_enableTitle->setText("On");
}

void UAVConfig::set_Value_Title_LabelMotor(int value)
{
    ui->undo_motor->setVisible(true);
    ui->default_motor->setVisible(true);
    ui->label_motorValue->setText(QString::number(value));
    if (value == 1)
        ui->label_motorTitle->setText("Normal");
    else ui->label_motorTitle->setText("Reversed");
}

void UAVConfig::set_Value_Title_LabelPolarity(int value)
{
    ui->undo_polarity->setVisible(true);
    ui->default_polarity->setVisible(true);
    ui->label_polarityValue->setText(QString::number(value));
    if (value == 1)
        ui->label_polarityTitle->setText("Positive");
    else ui->label_polarityTitle->setText("Negative");
}

void UAVConfig::set_Value_Title_LabelPWM(int value)
{
    ui->undo_PWM->setVisible(true);
    ui->default_PWM->setVisible(true);
    ui->label_PWMValue->setText(QString::number(value));
    if (value == 1)
        ui->label_PWMTitle->setText("High");
    else ui->label_PWMTitle->setText("Low");
}

void UAVConfig::set_Value_Title_LabelStartup(int value)
{
    ui->undo_startup->setVisible(true);
    ui->default_startup->setVisible(true);
    ui->label_startupValue->setText(QString::number(value));
    switch (value)
    {
    case 1:
        ui->label_startupTitle->setText("x0.031");
        break;
    case 2:
        ui->label_startupTitle->setText("x0.047");
        break;
    case 3:
        ui->label_startupTitle->setText("x0.063");
        break;
    case 4:
        ui->label_startupTitle->setText("x0.094");
        break;
    case 5:
        ui->label_startupTitle->setText("x0.125");
        break;
    case 6:
        ui->label_startupTitle->setText("x0.188");
        break;
    case 7:
        ui->label_startupTitle->setText("x0.25");
        break;
    case 8:
        ui->label_startupTitle->setText("x0.38");
        break;
    case 9:
        ui->label_startupTitle->setText("x0.50");
        break;
    case 10:
        ui->label_startupTitle->setText("x0.75");
        break;
    case 11:
        ui->label_startupTitle->setText("x1.00");
        break;
    case 12:
        ui->label_startupTitle->setText("x1.25");
        break;
    case 13:
        ui->label_startupTitle->setText("x1.50");
        break;
    }
}

void UAVConfig::set_Value_Title_LabelBeaconStr(int value)
{
    ui->undo_beaconstr->setVisible(true);
    ui->default_beaconstr->setVisible(true);
    ui->label_beaconstrValue->setText(QString::number(value));
    ui->label_beaconstrTitle->setText(QString::number(value));
}

void UAVConfig::set_Value_Title_LabelTempe(int value)
{
    ui->undo_tempe->setVisible(true);
    ui->default_tempe->setVisible(true);
    ui->label_tempeValue->setText(QString::number(value));
    if (value == 1)
        ui->label_tempeTitle->setText("On");
    else ui->label_tempeTitle->setText("Off");
}

// Handle event when click default button

void UAVConfig::handle_default_beep(bool b)
{
    Q_UNUSED(b);
    ui->slide_beep->setValue(default_beep);
    ui->default_beep->setVisible(false);
}

void UAVConfig::handle_default_beaconstr(bool b)
{
    Q_UNUSED(b);
    ui->slide_beaconstr->setValue(default_beaconstr);
    ui->default_beaconstr->setVisible(false);
}

void UAVConfig::handle_default_delay(bool b)
{
    Q_UNUSED(b);
    ui->slide_delay->setValue(default_delay);
    ui->default_delay->setVisible(false);
}

void UAVConfig::handle_default_demeg(bool b)
{
    Q_UNUSED(b);
    ui->slide_demeg->setValue(default_demeg);
    ui->default_demeg->setVisible(false);
}

void UAVConfig::handle_default_enable(bool b)
{
    Q_UNUSED(b);
    ui->slide_enable->setValue(default_enable);
    ui->default_enable->setVisible(false);
}

void UAVConfig::handle_default_motor(bool b)
{
    Q_UNUSED(b);
    ui->slide_motor->setValue(default_motor);
    ui->default_motor->setVisible(false);
}

void UAVConfig::handle_default_polarity(bool b)
{
    Q_UNUSED(b);
    ui->slide_polarity->setValue(default_polarity);
    ui->default_polarity->setVisible(false);
}

void UAVConfig::handle_default_pwm(bool b)
{
    Q_UNUSED(b);
    ui->slide_PWM->setValue(default_pwm);
    ui->default_PWM->setVisible(false);
}

void UAVConfig::handle_default_startup(bool b)
{
    Q_UNUSED(b);
    ui->slide_startup->setValue(default_startup);
    ui->default_startup->setVisible(false);
}

void UAVConfig::handle_default_tempe(bool b)
{
    Q_UNUSED(b);
    ui->slide_tempe->setValue(default_tempe);
    ui->default_tempe->setVisible(false);
}
