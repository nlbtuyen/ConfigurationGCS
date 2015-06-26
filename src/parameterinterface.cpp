#include "parameterinterface.h"
#include "ui_parameterinterface.h"
#include "uasmanager.h"
#include <QTreeWidget>
#include <QDebug>
#include <QMetaObject>

ParameterInterface::ParameterInterface(QWidget *parent) :
    QWidget(parent),
    paramWidgets(new QMap<int, AQParamWidget*>()),
    curr(-1),
    m_ui(new Ui::parameterWidget)
{
    m_ui->setupUi(this);

//    m_ui->sensorSettings->hide();

    // Get current MAV list
    QList<UASInterface*> systems = UASManager::instance()->getUASList();

    // Add each of them
    foreach (UASInterface* sys, systems) {
        addUAS(sys);
    }

    // Setup MAV connections
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(addUAS(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(activeUASSetListIndex(int)), this, SLOT(selectUAS(int)));
    this->setVisible(false);
}

ParameterInterface::~ParameterInterface()
{
    delete m_ui;
}

void ParameterInterface::selectUAS(int index)
{
    m_ui->stackedWidget->setCurrentIndex(index);
//    m_ui->sensorSettings->setCurrentIndex(index);
    curr = index;
}

/**
 *
 * @param uas System to add to list
 */
void ParameterInterface::addUAS(UASInterface* uas)
{
    AQParamWidget* param = new AQParamWidget(uas, this);
    paramWidgets->insert(uas->getUASID(), param);
    m_ui->stackedWidget->addWidget(param);

    param->requestParameterList();;


//    QGCSensorSettingsWidget* sensor = new QGCSensorSettingsWidget(uas, this);
//    m_ui->sensorSettings->addWidget(sensor);

    // Set widgets as default
    if (curr == -1) {
        // Clear
//        m_ui->sensorSettings->setCurrentWidget(sensor);
        m_ui->stackedWidget->setCurrentWidget(param);
        curr = 0;
    }
}

void ParameterInterface::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
