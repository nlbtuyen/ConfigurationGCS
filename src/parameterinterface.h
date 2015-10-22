
#ifndef PARAMETERINTERFACE_H
#define PARAMETERINTERFACE_H

#include <QWidget>

#include "ui_parameterinterface.h"
#include "uasinterface.h"
#include "aqpramwidget.h"

namespace Ui
{
class ParameterInterface;
}

/**
 *  Container class for onboard parameter widgets
 *
 * @see QGCParamWidget
 */
class ParameterInterface : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterInterface(QWidget *parent = 0);
    virtual ~ParameterInterface();

public slots:
    void addUAS(UASInterface* uas);
    void selectUAS(int index);

protected:
    virtual void changeEvent(QEvent *e);
    QMap<int, AQParamWidget*>* paramWidgets;
    int curr;

private:
    Ui::parameterWidget *m_ui;
};

#endif // PARAMETERINTERFACE_H
