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

    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(createAQParamWidget(UASInterface*)));

    connect(ui->btn_save, SIGNAL(clicked()),this,SLOT(saveAQSettings()));
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
//    qDebug() << "create AQParamWidget";
    paramaq = new AQParamWidget(uas, this);
    connect(paramaq, SIGNAL(requestParameterRefreshed()), this, SLOT(loadParametersToUI()));
//    paramaq->requestParameterList();
}
