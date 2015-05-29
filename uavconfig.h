#ifndef UAVCONFIG_H
#define UAVCONFIG_H

#include <QWidget>

namespace Ui {
class UAVConfig;
}

class UAVConfig : public QWidget
{
    Q_OBJECT

public:
    explicit UAVConfig(QWidget *parent = 0);
    ~UAVConfig();

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();


private:
    Ui::UAVConfig *ui;
    int isSelected =0;
};

#endif // UAVCONFIG_H
