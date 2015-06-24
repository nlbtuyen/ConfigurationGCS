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

UAVConfig::UAVConfig(QWidget *parent) :
    QWidget(parent),
    paramaq(NULL),
    uas(NULL),
    connectedLink(NULL),
    ui(new Ui::UAVConfig)
{
    fldnameRx.setPattern("^(COMM|CTRL|DOWNLINK|GMBL|GPS|IMU|L1|MOT|NAV|PPM|RADIO|SIG|SPVR|UKF|VN100|QUATOS|LIC)_[A-Z0-9_]+$"); // strict field name matching
    dupeFldnameRx.setPattern("___N[0-9]"); // for having duplicate field names, append ___N# after the field name (three underscores, "N", and a unique number)
    ui->setupUi(this);


    updateCommonImages();

    aqBinFolderPath = QCoreApplication::applicationDirPath() + "/aq/bin/";
    platformExeExt = ".exe";
    LastFilePath = settings.value("AUTOQUAD_LAST_PATH").toString();

    for(int i=0; i < 6; i++ )
    {
        allRadioChanProgressBars << ui->widget_2->findChild<QProgressBar *>(QString("progressBar_chan_%1").arg(i));
    }

    connect(ui->flashButton, SIGNAL(clicked()), this, SLOT(flashFW()));
    connect(ui->SelectFirmwareButton, SIGNAL(clicked()), this, SLOT(selectFWToFlash()));

    //Process Slots
    ps_master.setProcessChannelMode(QProcess::MergedChannels);
    connect(&ps_master, SIGNAL(finished(int)), this, SLOT(prtstexit(int)));
    connect(&ps_master, SIGNAL(readyReadStandardOutput()), this, SLOT(prtstdout()));
    connect(&ps_master, SIGNAL(error(QProcess::ProcessError)), this, SLOT(extProcessError(QProcess::ProcessError)));


    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(createAQParamWidget(UASInterface*)));

    connect (ui->btn_save, SIGNAL(clicked()), this, SLOT(saveAQSetting()));
    loadSettings();

//    aqtelemetry = new AQTelemetryView();
//    ui->scrollArea_logviewer->setWidget(aqtelemetry);

}

UAVConfig::~UAVConfig()
{
    delete ui;
}

void UAVConfig::on_btn_quadx_clicked()
{
    QImage imageObject6;
    imageObject6.load(str + "quadx.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 1){
        ui->btn_quadx->setStyleSheet("background-color : yellow");
        //        isSelected = 1;
    }else if (isSelected == 2){
        ui->btn_quadx->setStyleSheet("background-color : yellow");
        ui->btn_quadplus->setStyleSheet("");
        isSelected = 1;
    }else if (isSelected == 3){
        ui->btn_quadx->setStyleSheet("background-color : yellow");
        ui->btn_hex6->setStyleSheet("");
        isSelected = 1;
    }else if (isSelected == 4){
        ui->btn_quadx->setStyleSheet("background-color : yellow");
        ui->btn_hexy->setStyleSheet("");
        isSelected = 1;
    }
}

void UAVConfig::on_btn_quadplus_clicked()
{
    QImage imageObject6;
    imageObject6.load(str + "quadplus.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->btn_quadplus->setStyleSheet("background-color : yellow");
        isSelected = 2;
    }else if (isSelected == 1){
        ui->btn_quadplus->setStyleSheet("background-color : yellow");
        ui->btn_quadx->setStyleSheet("");
        isSelected = 2;
    }else if (isSelected == 3){
        ui->btn_quadplus->setStyleSheet("background-color : yellow");
        ui->btn_hex6->setStyleSheet("");
        isSelected = 2;
    }else if (isSelected == 4){
        ui->btn_quadplus->setStyleSheet("background-color : yellow");
        ui->btn_hexy->setStyleSheet("");
        isSelected = 2;
    }
}

void UAVConfig::on_btn_hex6_clicked()
{
    QImage imageObject6;
    imageObject6.load(str + "hex.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->btn_hex6->setStyleSheet("background-color : yellow");
        isSelected = 3;
    }else if (isSelected == 1){
        ui->btn_hex6->setStyleSheet("background-color : yellow");
        ui->btn_quadx->setStyleSheet("");
        isSelected = 3;
    }else if (isSelected == 2){
        ui->btn_hex6->setStyleSheet("background-color : yellow");
        ui->btn_quadplus->setStyleSheet("");
        isSelected = 3;
    }else if (isSelected == 4){
        ui->btn_hex6->setStyleSheet("background-color : yellow");
        ui->btn_hexy->setStyleSheet("");
        isSelected = 3;
    }
}

void UAVConfig::on_btn_hexy_clicked()
{
    QImage imageObject6;
    imageObject6.load(str + "hexY.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->btn_hexy->setStyleSheet("background-color : yellow");
        isSelected = 4;
    }else if (isSelected == 1){
        ui->btn_hexy->setStyleSheet("background-color : yellow");
        ui->btn_quadx->setStyleSheet("");
        isSelected = 4;
    }else if (isSelected == 2){
        ui->btn_hexy->setStyleSheet("background-color : yellow");
        ui->btn_quadplus->setStyleSheet("");
        isSelected = 4;
    }else if (isSelected == 3){
        ui->btn_hexy->setStyleSheet("background-color : yellow");
        ui->btn_hex6->setStyleSheet("");
        isSelected = 4;
    }
}

void UAVConfig::updateCommonImages()
{
    QImage imageObject1;
    imageObject1.load(str + "1.png");
    ui->lbl_show_calib_gyro->setPixmap(QPixmap::fromImage(imageObject1));

    QImage imageObject2;
    imageObject2.load(str + "2.png");
    ui->lbl_show_calib_mag->setPixmap(QPixmap::fromImage(imageObject2));

    QImage imageObject3;
    imageObject3.load(str + "3.png");
    ui->lbl_show_save_ee->setPixmap(QPixmap::fromImage(imageObject3));

    QImage imageObject4;
    imageObject4.load(str + "4.png");
    ui->lbl_show_calib_esc->setPixmap(QPixmap::fromImage(imageObject4));

    QImage imageObject5;
    imageObject5.load(str + "4.png");
    ui->lbl_show_enable_pid->setPixmap(QPixmap::fromImage(imageObject5));

    QImage imageObject7;
    imageObject7.load(str + "display.jpg");
    ui->lbl_show_primary->setPixmap(QPixmap::fromImage(imageObject7));

    ui->btn_quadx->setStyleSheet("background-color : yellow");
    QImage imageObject6;
    ui->lbl_show_ac->setAlignment(Qt::AlignCenter);
    imageObject6.load(str + "quadx.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
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

    QVariant val;
    getGUIPara(ui->tab_aq_setting);
    ui->groupBox_roll_angle->setDisabled(1);
    // convert old radio type value if switching to new system

    //    if (useRadioSetupParam && paramaq->getParaAQ("RADIO_SETUP").toInt() == 0 && paramaq->paramExistsAQ("RADIO_TYPE")) {
    //        int idx = ui->RADIO_SETUP->findData(paramaq->getParaAQ("RADIO_TYPE").toInt() + 1);
    //        ui->RADIO_SETUP->setCurrentIndex(idx);
    //        //radioType_changed(idx);
    //    }
    val = paramaq->getParaAQ("RADIO_SETUP");
    uint8_t idx = val.toInt();
    //    qDebug() << "RADIO SETUP " <<val;
    QMap<int, QString> radioTypes;
    radioTypes.insert(0, tr("No Radio"));
    radioTypes.insert(1, tr("Spektrum 11Bit"));
    radioTypes.insert(2, tr("Spektrum 10Bit"));
    radioTypes.insert(3, tr("S-BUS (Futaba, others)"));
    radioTypes.insert(4, tr("PPM"));

    ui->RADIO_SETUP->blockSignals(true);
    ui->RADIO_SETUP->clear();
    QMapIterator<int, QString> i(radioTypes);
    while (i.hasNext()) {
        i.next();
        ui->RADIO_SETUP->addItem(i.value(), i.key());
    }
    ui->RADIO_SETUP->setCurrentIndex(idx);
    ui->RADIO_SETUP->blockSignals(false);

}

void UAVConfig::createAQParamWidget(UASInterface *uastmp)
{
    uas = uastmp;

    connect(uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this, SLOT(setRadioChannelDisplayValue(int,float)));
    //    qDebug() << "create AQParamWidget";
    paramaq = new AQParamWidget(uas, this);
    connect(paramaq, SIGNAL(requestParameterRefreshed()), this, SLOT(loadParametersToUI()));
}

void UAVConfig::setRadioChannelDisplayValue(int channelId, float normalized)
{
    int val;
    if (channelId >= allRadioChanProgressBars.size())
        return;
    QProgressBar* bar = allRadioChanProgressBars.at(channelId);
    val = (int)(normalized-1024);

    if (val > bar->maximum())
        val = bar->maximum();
    if (val < bar->minimum())
        val = bar->minimum();
    bar->setValue(val);
}

void UAVConfig::getGUIPara(QWidget *parent)
{
    useRadioSetupParam = paramaq->paramExistsAQ("RADIO_SETUP");

    bool ok;
    int precision, tmp;
    QString paraName, valstr;
    QVariant val;
    QLabel *paraLabel;
    QWidget *paraContainer;
    QList<QWidget*> wdgtList = ui->tab_aq_setting->findChildren<QWidget *>(fldnameRx);
    foreach (QWidget* w, wdgtList) {
        //            qDebug() << "QWidget Name" <<w->objectName();
        paraName = paramNameGuiToOnboard(w->objectName());
        paraLabel = ui->tab_aq_setting->findChild<QLabel *>(QString("label_%1").arg(w->objectName()));
        paraContainer = ui->tab_aq_setting->findChild<QWidget *>(QString("container_%1").arg(w->objectName()));

        //            qDebug() << paraName;
        //            qDebug() << paraLabel;
        //            qDebug() << paraContainer;

        val = paramaq->getParaAQ(paraName);
        if (paraName == "GMBL_SCAL_PITCH" || paraName == "GMBL_SCAL_ROLL")
            val = fabs(val.toFloat());
        else if (paraName == "RADIO_SETUP")
            val = val.toInt() & 0x0f;

        //            qDebug() << val;

        if (QComboBox* cb = qobject_cast<QComboBox *>(w)) {
            //                qDebug() << "QComboBox" <<cb->objectName();
            if (cb->isEditable()) {
                //                    qDebug() << "Editable";
                if ((tmp = cb->findText(val.toString())) > -1)
                {
                    cb->setCurrentIndex(tmp);
                    //                        qDebug() << "Set Current Index";
                }
                else {
                    cb->insertItem(0, val.toString());
                    cb->setCurrentIndex(0);
                }
            }
            else if ((tmp = cb->findData(val)) > -1)
                cb->setCurrentIndex(tmp);
            else
                cb->setCurrentIndex(abs(val.toInt(&ok)));
        } else if (QSlider* prb = qobject_cast<QSlider *>(w)) {
            //@Zyrter Fix here to change display value
            if(val.toFloat() < 0.01){
                prb->setValue((int)(val.toFloat()*100/0.01));
            } else if (val.toFloat() < 1) {
                prb->setValue((int)(val.toFloat()*100));
            }  else if (val.toFloat() < 10) {
                prb->setValue((int)(val.toFloat()*100/10));
            } else if (val.toFloat() < 100) {
                prb->setValue((int)(val.toFloat()));
            } else if (val.toFloat() < 1000) {
                prb->setValue((int)(val.toFloat()*100/1000));
            } else if (val.toFloat() < 10000) {
                prb->setValue((int)(val.toFloat()*100/10000));
            }
        }
        else continue;
    }
}


//========= Update FW ==========


bool UAVConfig::checkAqConnected(bool interactive)
{
    if (!uas || !paramaq || uas->getCommunicationStatus() != uas->COMM_CONNECTED ) {
        if (interactive)
            MainWindow::instance()->showCriticalMessage("Error", "No AutoQuad connected!");
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
            MainWindow::instance()->showCriticalMessage(tr("Warning!"), tr("Could not open firmware file. %1").arg(file.errorString()));
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

    QString msg = "";

        msg += tr("Make sure your AQ is connected via USB and is already in bootloader mode.  To enter bootloader mode,"
                  "first connect the BOOT pins (or hold the BOOT button) and then turn the AQ on.\n\n");

    msg += "Do you wish to continue flashing?";

    QMessageBox::StandardButton qrply = QMessageBox::warning(this, tr("Confirm Firmware Flashing"), msg, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
    if (qrply == QMessageBox::Cancel)
        return;

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

//Print to output
void UAVConfig::prtstexit(int stat)
{
    //prtstdout();
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



//Save AQSetting

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
        qDebug() << "Copied settings from Aq.ini to QGC shared config storage.";
    }

    if (settings.contains("AUTOQUAD_FW_FILE") && settings.value("AUTOQUAD_FW_FILE").toString().length()) {
        ui->fileLabel->setText(settings.value("AUTOQUAD_FW_FILE").toString());
        ui->fileLabel->setToolTip(settings.value("AUTOQUAD_FW_FILE").toString());
        ui->checkBox_verifyFwFlash->setChecked(settings.value("AUTOQUAD_FW_VERIFY", true).toBool());
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

                msg += QString("<tr><td style=\"padding: 1px 7px 0 1px;\"><span style=\"color: rgba(255, 0, 0, 200); font-weight: bold;\">%1</span>%2</td><td>%3 </td><td>%4</td></tr>\n").arg(restartFlag, i.key(), val1, val2);
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
        if (interactive)
            MainWindow::instance()->showCriticalMessage(tr("Warning"), tr("No changed parameters detected.  Nothing saved."));
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
