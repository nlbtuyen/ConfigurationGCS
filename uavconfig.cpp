#include "uavconfig.h"
#include "ui_uavconfig.h"
#include "seriallinkinterface.h"
#include "seriallink.h"
#include "linkmanager.h"
#include "mainwindow.h"
#include "uas.h"
#include "uasmanager.h"
#include "uasinterface.h"

#include <QDebug>
#include <QWidget>
#include <QFileDialog>
#include <QTextBrowser>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

UAVConfig::UAVConfig(QWidget *parent) :
    QWidget(parent),
    paramaq(NULL),
    uas(NULL),
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

    ui->comboBox_fwType->addItem(tr("AutoQuad Serial"), "aq");
    ui->comboBox_fwType->addItem(tr("AutoQuad M4 USB"), "dfu");
    ui->comboBox_fwType->setCurrentIndex(0);

    ui->comboBox_fwPortSpeed->setCurrentIndex(ui->comboBox_fwPortSpeed->findText(settings.value("FW_FLASH_BAUD_RATE", 115200).toString()));

    QStringList flashBaudRates;
    flashBaudRates << "38400" <<  "57600" << "115200";
    ui->comboBox_fwPortSpeed->addItems(flashBaudRates);

    connect(ui->portName, SIGNAL(currentIndexChanged(QString)), this, SLOT(setPortName(QString)));
    connect(ui->comboBox_fwPortSpeed, SIGNAL(currentIndexChanged(QString)), this, SLOT(setPortName(QString)));
    connect(ui->comboBox_fwType, SIGNAL(currentIndexChanged(int)), this, SLOT(fwTypeChange()));
    connect(ui->flashButton, SIGNAL(clicked()), this, SLOT(flashFW()));
    connect(ui->SelectFirmwareButton, SIGNAL(clicked()), this, SLOT(selectFWToFlash()));
    connect(ui->toolButton_fwReloadPorts, SIGNAL(clicked()), this, SLOT(setupPortList()));

    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(createAQParamWidget(UASInterface*)));

    //connect(ui->btn_save, SIGNAL(clicked()),this,SLOT(saveAQSettings()));

    setupPortList();
    loadSettings();

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

    QImage imageObject8;
    imageObject8.load(str + "graph.png");
    ui->lbl_graph->setPixmap(QPixmap::fromImage(imageObject8));

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
    //    qDebug() << "load Parameters to UI";
    //    qDebug() << paramaq->paramExistsAQ("CTRL_TLT_RTE_P");
    //    QVariant val;
    //    val = paramUIaq->getParaAQ("CTRL_TLT_RTE_P");
    //    QString valstr;
    //    valstr.setNum(val.toFloat(), 'g', 6);
    //    qDebug() << valstr;
    //    paramaq = paramUI;
    useRadioSetupParam = paramaq->paramExistsAQ("RADIO_SETUP");
    //qDebug() << useRadioSetupParam;

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
        } else if (QProgressBar* prb = qobject_cast<QProgressBar *>(w)) {
            //                qDebug() << "QProgressBar" <<prb->objectName();
            //                qDebug() << "QPro Value" <<val.toFloat();
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

    //        ui->RADIO_FLAP_CH->setCurrentIndex(val.toInt());
    //        ui->RADIO_ROLL_CH->setCurrentIndex(2);(w

}

void UAVConfig::createAQParamWidget(UASInterface *uas)
{
    connect(uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this, SLOT(setRadioChannelDisplayValue(int,float)));
    //    qDebug() << "create AQParamWidget";
    paramaq = new AQParamWidget(uas, this);
    connect(paramaq, SIGNAL(requestParameterRefreshed()), this, SLOT(loadParametersToUI()));
    //    paramaq->requestParameterList();
}

void UAVConfig::setRadioChannelDisplayValue(int channelId, float normalized)
{
    int val;
    //    qDebug() << "Channel" <<channelId;

    //    qDebug() << "Value" <<val;
    //@Zyrter Fix here to set fit value on progress Bar

    if (channelId >= allRadioChanProgressBars.size())
        return;
    QProgressBar* bar = allRadioChanProgressBars.at(channelId);
    val = (int)(normalized-1024);

//    if (channelId == 0)
//        qDebug() << "Value" <<val;
    if (val > bar->maximum())
        val = bar->maximum();
    if (val < bar->minimum())
        val = bar->minimum();
    bar->setValue(val);
}


//========= Update FW ==========


bool UAVConfig::checkAqConnected(bool interactive)
{
    if ( !paramaq || !uas || uas->getCommunicationStatus() != uas->COMM_CONNECTED ) {
        if (interactive)
            MainWindow::instance()->showCriticalMessage("Error", "No AutoQuad connected!");
        return false;
    } else
        return true;
}



void UAVConfig::flashFW()
{
    if (ui->comboBox_fwType->currentIndex() == -1) {
        MainWindow::instance()->showCriticalMessage(tr("Error!"), tr("Please select the firwmare type."));
        return;
    }

    if (checkProcRunning())
        return;

    QString fwtype = ui->comboBox_fwType->itemData(ui->comboBox_fwType->currentIndex()).toString();
    QString msg = "";

    if (fwtype == "dfu") {
        msg += tr("Make sure your AQ is connected via USB and is already in bootloader mode.  To enter bootloader mode,"
                  "first connect the BOOT pins (or hold the BOOT button) and then turn the AQ on.\n\n");
    }
    else
    {
        if (!portName.length()) {
            MainWindow::instance()->showCriticalMessage(tr("Error!"), tr("Please select an available COM port."));
            return;
        }

        if ( checkAqSerialConnection(portName) )
            msg = tr("WARNING: You are already connected to AutoQuad. If you continue, you will be disconnected and then re-connected afterwards.\n\n");

        msg += tr("WARNING: Flashing firmware will reset all AutoQuad settings back to default values. Make sure you have your generated parameters and custom settings saved.\n\n");

        msg += tr("Make sure you are using the %1 port.\n").arg(portName);
        msg += tr("There is a delay before the flashing process shows any progress. Please wait at least 20sec. before you retry!\n\n");
    }
    msg += "Do you wish to continue flashing?";

    QMessageBox::StandardButton qrply = QMessageBox::warning(this, tr("Confirm Firmware Flashing"), msg, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
    if (qrply == QMessageBox::Cancel)
        return;

    if (connectedLink)
        connectedLink->disconnect();

    activeProcessStatusWdgt = ui->textFlashOutput;
    fwFlashActive = true;

    if (fwtype == "aq")
        flashFwStart();
    else
        flashFwDfu();

}

void UAVConfig::flashFwStart()
{
    QString AppPath = QDir::toNativeSeparators(aqBinFolderPath + "stm32flash" + platformExeExt);
    QStringList Arguments;
    Arguments.append(QString("-b"));
    Arguments.append(ui->comboBox_fwPortSpeed->currentText());
    Arguments.append(QString("-w"));
    Arguments.append(QDir::toNativeSeparators(ui->fileLabel->text()));
    if (ui->fileLabel->text().endsWith(".bin", Qt::CaseInsensitive))
        Arguments.append("-s 0x08000000");
    if (ui->checkBox_verifyFwFlash->isChecked())
        Arguments.append("-v");
    Arguments.append(portName);

    QString cmdLine = AppPath;
    foreach (const QString arg, Arguments)
        cmdLine += " " + arg;
    ui->textFlashOutput->append(cmdLine + "\n\n");

    ps_master.start(AppPath , Arguments, QProcess::Unbuffered | QProcess::ReadWrite);
}

void UAVConfig::flashFwDfu()
{
    QString AppPath = QDir::toNativeSeparators(aqBinFolderPath + "dfu-util" + platformExeExt);
    QStringList Arguments;
    Arguments.append("-a 0");                   // alt 0 is start of internal flash
    Arguments.append("-d 0483:df11" );          // device ident stm32
    Arguments.append("-s 0x08000000:leave");    // start address (:leave to exit DFU mode after flash)
    //Arguments.append("-v");                   // verbose
    Arguments.append("-R");                     // reset after upload
    Arguments.append("-D");                     // firmware file
    Arguments.append(QDir::toNativeSeparators(ui->fileLabel->text()));

    QString cmdLine = AppPath;
    foreach (const QString arg, Arguments)
        cmdLine += " " + arg;
    ui->textFlashOutput->append(cmdLine + "\n\n");

    ps_master.start(AppPath , Arguments, QProcess::Unbuffered | QProcess::ReadWrite);
}

void UAVConfig::setPortName(QString str)
{

    //    if (ui->portName->currentText() == ui->portName->itemText(ui->portName->currentIndex()))
    //    {
    //        if (ui->portName->itemData(ui->portName->currentIndex()).toString() == "[no port]")
    //            return;
    //        str = ui->portName->itemData(ui->portName->currentIndex()).toString();
    //    }
    //    else
    //        str = str.split(" - ").first().remove(" ");


    portName = ui->portName->itemData(ui->portName->currentIndex()).toString();
    ui->portName->setToolTip(ui->portName->currentText());
}

void UAVConfig::fwTypeChange()
{
    bool en = ui->comboBox_fwType->itemData(ui->comboBox_fwType->currentIndex()).toString() != "dfu";
    ui->comboBox_fwPortSpeed->setEnabled(en);
    ui->portName->setEnabled(en);
    ui->label_fwPort->setEnabled(en);
    ui->label_fwPortSpeed->setEnabled(en);
    ui->toolButton_fwReloadPorts->setEnabled(en);
    ui->checkBox_verifyFwFlash->setEnabled(en);
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
                                                    tr("AQ or ESC32 firmware") + " (*.hex *.bin)");

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

        setFwType();
    }
}

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

    ui->portName->setCurrentIndex(ui->portName->findText(settings.value("FW_FLASH_PORT_NAME", "").toString()));
    ui->comboBox_fwPortSpeed->setCurrentIndex(ui->comboBox_fwPortSpeed->findText(settings.value("FW_FLASH_BAUD_RATE", 115200).toString()));

    if (settings.contains("AUTOQUAD_FW_FILE") && settings.value("AUTOQUAD_FW_FILE").toString().length()) {
        ui->fileLabel->setText(settings.value("AUTOQUAD_FW_FILE").toString());
        ui->fileLabel->setToolTip(settings.value("AUTOQUAD_FW_FILE").toString());
        ui->checkBox_verifyFwFlash->setChecked(settings.value("AUTOQUAD_FW_VERIFY", true).toBool());
        setFwType();
    }

    LastFilePath = settings.value("AUTOQUAD_LAST_PATH").toString();


    settings.endGroup();
    settings.sync();
}

void UAVConfig::setFwType()
{
    QString typ = "aq";
    // test for aq M4 or v7/8 hardware in fw file name
    if (ui->fileLabel->text().contains(QRegExp("(aq|autoquad).+(hwv[78]\\.[\\d]|m4).+\\.bin$", Qt::CaseInsensitive)))
        typ = "dfu";

    ui->comboBox_fwType->setCurrentIndex(ui->comboBox_fwType->findData(typ));
}

void UAVConfig::setupPortList()
{
    QString pdispname;
    QString cidxfw = ui->portName->currentText();
    ui->portName->clear();
    // Get the ports available on this system
    foreach (const QextPortInfo &p, QextSerialEnumerator::getPorts()) {
        if (!p.portName.length())
            continue;
        pdispname = p.portName;
        if (p.friendName.length())
            pdispname += " - " + p.friendName.split(QRegExp(" ?\\(")).first();
        ui->portName->addItem(pdispname, p.portName);
    }
    ui->portName->setCurrentIndex(ui->portName->findText(cidxfw));
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

bool UAVConfig::checkAqSerialConnection(QString port)
{
    bool IsConnected = false;
    connectedLink = NULL;

    if (!checkAqConnected(false))
        return false;

    if ( uas != NULL ) {
        for ( int i=0; i < uas->getLinks()->count(); i++) {
            connectedLink = uas->getLinks()->at(i);
            //qDebug() << connectedLink->getName();
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


