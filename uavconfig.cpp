#include "uavconfig.h"
#include "ui_uavconfig.h"

UAVConfig::UAVConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVConfig)
{
    ui->setupUi(this);
    QImage imageObject1;
    imageObject1.load("image uisoft/1-1.png");
    ui->label_21->setPixmap(QPixmap::fromImage(imageObject1));

    QImage imageObject2;
    imageObject2.load("image uisoft/2-2.png");
    ui->label_22->setPixmap(QPixmap::fromImage(imageObject2));

    QImage imageObject3;
    imageObject3.load("image uisoft/3-3.png");
    ui->label_23->setPixmap(QPixmap::fromImage(imageObject3));

    QImage imageObject4;
    imageObject4.load("image uisoft/4-4.png");
    ui->label_24->setPixmap(QPixmap::fromImage(imageObject4));

    QImage imageObject5;
    imageObject5.load("image uisoft/4-4.png");
    ui->label_25->setPixmap(QPixmap::fromImage(imageObject5));

    QImage imageObject7;
    imageObject7.load("image uisoft/display.png");
    ui->label_63->setPixmap(QPixmap::fromImage(imageObject7));
}

UAVConfig::~UAVConfig()
{
    delete ui;
}

void UAVConfig::on_pushButton_5_clicked()
{
    QImage imageObject6;
    imageObject6.load("image uisoft/quadX.png");
    ui->label_61->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->pushButton_5->setStyleSheet("background-color : yellow");
        isSelected = 1;
    }else if (isSelected == 2){
        ui->pushButton_5->setStyleSheet("background-color : yellow");
        ui->pushButton_6->setStyleSheet("");
        isSelected = 1;
    }else if (isSelected == 3){
        ui->pushButton_5->setStyleSheet("background-color : yellow");
        ui->pushButton_7->setStyleSheet("");
        isSelected = 1;
    }else if (isSelected == 4){
        ui->pushButton_5->setStyleSheet("background-color : yellow");
        ui->pushButton_8->setStyleSheet("");
        isSelected = 1;
    }
}

void UAVConfig::on_pushButton_6_clicked()
{
    QImage imageObject6;
    imageObject6.load("image uisoft/quad_plus.jpg");
    ui->label_61->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->pushButton_6->setStyleSheet("background-color : yellow");
        isSelected = 2;
    }else if (isSelected == 1){
        ui->pushButton_6->setStyleSheet("background-color : yellow");
        ui->pushButton_5->setStyleSheet("");
        isSelected = 2;
    }else if (isSelected == 3){
        ui->pushButton_6->setStyleSheet("background-color : yellow");
        ui->pushButton_7->setStyleSheet("");
        isSelected = 2;
    }else if (isSelected == 4){
        ui->pushButton_6->setStyleSheet("background-color : yellow");
        ui->pushButton_8->setStyleSheet("");
        isSelected = 2;
    }
}

void UAVConfig::on_pushButton_7_clicked()
{
    QImage imageObject6;
    imageObject6.load("image uisoft/hex.png");
    ui->label_61->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->pushButton_7->setStyleSheet("background-color : yellow");
        isSelected = 3;
    }else if (isSelected == 1){
        ui->pushButton_7->setStyleSheet("background-color : yellow");
        ui->pushButton_5->setStyleSheet("");
        isSelected = 3;
    }else if (isSelected == 2){
        ui->pushButton_7->setStyleSheet("background-color : yellow");
        ui->pushButton_6->setStyleSheet("");
        isSelected = 3;
    }else if (isSelected == 4){
        ui->pushButton_7->setStyleSheet("background-color : yellow");
        ui->pushButton_8->setStyleSheet("");
        isSelected = 3;
    }
}

void UAVConfig::on_pushButton_8_clicked()
{
    QImage imageObject6;
    imageObject6.load("image uisoft/hex_Y.png");
    ui->label_61->setPixmap(QPixmap::fromImage(imageObject6));
    if (isSelected == 0){
        ui->pushButton_8->setStyleSheet("background-color : yellow");
        isSelected = 4;
    }else if (isSelected == 1){
        ui->pushButton_8->setStyleSheet("background-color : yellow");
        ui->pushButton_5->setStyleSheet("");
        isSelected = 4;
    }else if (isSelected == 2){
        ui->pushButton_8->setStyleSheet("background-color : yellow");
        ui->pushButton_6->setStyleSheet("");
        isSelected = 4;
    }else if (isSelected == 3){
        ui->pushButton_8->setStyleSheet("background-color : yellow");
        ui->pushButton_7->setStyleSheet("");
        isSelected = 4;
    }
}

