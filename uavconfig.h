#ifndef UAVCONFIG_H
#define UAVCONFIG_H

#include <QDialog>

namespace Ui {
class UAVConfig;
}

class UAVConfig : public QDialog
{
    Q_OBJECT

public:
    explicit UAVConfig(QWidget *parent = 0);
    ~UAVConfig();

private slots:
    void on_btn_quadx_clicked();

    void on_btn_quadplus_clicked();

    void on_btn_hex6_clicked();

    void on_btn_hexy_clicked();

private:
    Ui::UAVConfig *ui;
    int isSelected = 0;
};

#endif // UAVCONFIG_H
