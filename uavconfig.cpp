#include "uavconfig.h"
#include "ui_uavconfig.h"

UAVConfig::UAVConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UAVConfig)
{
    ui->setupUi(this);

    QImage imageObject1;
    imageObject1.load("image uisoft/1.png");
    ui->lbl_show_calib_gyro->setPixmap(QPixmap::fromImage(imageObject1));

    QImage imageObject2;
    imageObject2.load("image uisoft/2.png");
    ui->lbl_show_calib_mag->setPixmap(QPixmap::fromImage(imageObject2));

    QImage imageObject3;
    imageObject3.load("image uisoft/3.png");
    ui->lbl_show_save_ee->setPixmap(QPixmap::fromImage(imageObject3));

    QImage imageObject4;
    imageObject4.load("image uisoft/4.png");
    ui->lbl_show_calib_esc->setPixmap(QPixmap::fromImage(imageObject4));

    QImage imageObject5;
    imageObject5.load("image uisoft/4.png");
    ui->lbl_show_enable_pid->setPixmap(QPixmap::fromImage(imageObject5));

    QImage imageObject7;
    imageObject7.load("image uisoft/display.jpg");
    ui->lbl_show_primary->setPixmap(QPixmap::fromImage(imageObject7));

    QImage imageObject8;
    imageObject8.load("image uisoft/graph.png");
    ui->lbl_graph->setPixmap(QPixmap::fromImage(imageObject8));
}

UAVConfig::~UAVConfig()
{
    delete ui;
}

void UAVConfig::on_btn_quadx_clicked()
{
    QImage imageObject6;
    imageObject6.load("image uisoft/quadX.png");
    ui->lbl_show_ac->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->btn_quadx->setStyleSheet("background-color : yellow");
        isSelected = 1;
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
    imageObject6.load("image uisoft/quadplus.png");
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
    imageObject6.load("image uisoft/hex.png");
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
    imageObject6.load("image uisoft/hexY.png");
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
