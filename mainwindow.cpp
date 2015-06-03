#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDebug>
#include <QLayout>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QDockWidget>
#include <QSettings>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDesktopServices>
#include <QVariant>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "uavconfig.h"
#include "qtoolbar.h"
#include "parameterinterface.h"
#include "mavlinkmessagesender.h"
#include "uasmanager.h"
#include "linkmanager.h"
#include "mavlinkprotocol.h"
#include "qgc.h"
#include "seriallink.h"
#include "uasinfowidget.h"


//QFile file("datatype.txt");
//QTextStream out(&file);

MainWindow::MainWindow(QWidget *parent) :
    mav(NULL),
    changed(true),
    batteryPercent(0),
    batteryVoltage(0),
    systemArmed(false),
    QMainWindow(parent),
    sys_mode(MAV_MODE_PREFLIGHT),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    QMainWindow::showFullScreen();

    //    serial = new QSerialPort(this);
    settings = new SettingsDialog();
    config = new UAVConfig();
    setCentralWidget(config);

    link = new SerialLink();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    infoDockWidget = new QDockWidget(tr("Status Details"), this);
    infoDockWidget->setWidget( new UASInfoWidget(this));
    infoDockWidget->setObjectName("UAS_STATUS_DETAILS_DOCKWIDGET");
    addTool(infoDockWidget, tr("Status Details"), Qt::RightDockWidgetArea);

    parametersDockWidget = new QDockWidget(tr("Onboard Parameters"), this);
    parametersDockWidget->setWidget( new ParameterInterface(this) );
    parametersDockWidget->setObjectName("PARAMETER_INTERFACE_DOCKWIDGET");
    addTool(parametersDockWidget, tr("Onboard Parameters"), Qt::RightDockWidgetArea);




    if(setting.contains(getWindowGeometryKey()))
    {
        //REstore the window geometry
        restoreGeometry(setting.value(getWindowGeometryKey()).toByteArray());
        show();
    }
    else
    {
        //Ajust the size
        const int screenWidth = QApplication::desktop()->availableGeometry().width();
        const int screenHeight = QApplication::desktop()->availableGeometry().height();

        if (screenWidth <1200 || screenHeight <800) {
            showMaximized();
        }
        else{
            resize(qMin(screenWidth,1200), qMin(screenHeight,800));
            show();
        }
    }

    initActionsConnections();
connectCommonWidgets();
connectCommonActions();

}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
    serial->close();
    //    file.close();
}

QList<QAction *> MainWindow::listLinkMenuActions()
{
    return ui->menuWidgets->actions();
}

QAction *MainWindow::getActionByLink(LinkInterface *link)
{
    QAction *ret = NULL;
    int linkIndex = LinkManager::instance()->getLinks().indexOf(link);
    int linkID = LinkManager::instance()->getLinks().at(linkIndex)->getId();

    foreach (QAction* act, listLinkMenuActions())
    {
        if (act->data().toInt() == linkID)
            return act;
    }
    return ret;
}

void MainWindow::closeSerialPort()
{
    if(link)
    {
        link->disconnect(); //disconnect port, and also calls terminate() to stop the thread
        link->wait(); // wait() until thread is stoped before deleting
        LinkManager::instance()->removeLink(link); //remove link from LinkManager list
        link->deleteLater();
    }
    link=NULL;
    //    parametersDockWidget->hide();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));

}

void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::readData()
{
    //qDebug() << "readData() \n";
    const qint64 maxLength = 2048;
    char data[maxLength];
    qint64 numBytes = 0, rBytes = 0;

    dataMutex.lock();
    while ((numBytes = serial->bytesAvailable())) {
        rBytes = numBytes;
        if(maxLength < rBytes) rBytes = maxLength;

        if (serial->read(data, rBytes) == -1) { // -1 result means error

            return;
        }
        QByteArray b(data, rBytes);
        bytesReceived(b);

        bitsReceivedTotal += rBytes * 8;

    }
    dataMutex.unlock();

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::heartbeatTimeout(bool timeout, unsigned int ms)
{
    if (ms>10000)
    {
        if (!link || !link->isConnected())
        {
            toolBarTimeoutLabel->setText(tr("DISCONNECTED"));
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: #B40404; }"));
        }
    }
    if (timeout)
    {
        if ((ms/1000)%2==0)
        {
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: #B40404; }"));
        }
        else
        {
            toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding: 0 3px; background-color: #B40404; }"));
        }
        toolBarTimeoutLabel->setText(tr("CONNECTION LOST: %1 s").arg((ms / 1000.0f), 2, 'f', 1, ' '));
    }
    else
    {

    }
}

void MainWindow::updateBatteryRemaining(UASInterface *uas, double voltage, double percent, int seconds)
{
    Q_UNUSED(uas);
    Q_UNUSED(seconds);
    if (batteryPercent != percent || batteryVoltage != voltage) changed = true;
    batteryPercent = percent;
    batteryVoltage = voltage;
}

void MainWindow::updateArmingState(bool armed)
{
    systemArmed = armed;
    changed = true;
    /* important, immediately update */
    updateView();
}

void MainWindow::UAScreated(UASInterface *uas)
{
    if (!uas)
        return;


}

void MainWindow::addTool(QDockWidget* widget, const QString& title, Qt::DockWidgetArea area)
{
    QAction* tempAction = ui->menuWidgets->addAction(title);

    tempAction->setCheckable(true);
    QVariant var;
    var.setValue((QWidget*)widget);
    tempAction->setData(var);
    connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
    connect(widget, SIGNAL(visibilityChanged(bool)), tempAction, SLOT(setChecked(bool)));
    tempAction->setChecked(widget->isVisible());
    addDockWidget(area, widget);
    widget->setMinimumWidth(250);
    //    widget->hide();
}

void MainWindow::connectCommonWidgets()
{
    if (infoDockWidget && infoDockWidget->widget())
    {
        connect(mavlink, SIGNAL(receiveLossChanged(int,float)),infoDockWidget->widget(), SLOT(updateSendLoss(int, float)));
    }
}

void MainWindow::connectCommonActions()
{
//  QActionGroup* perspectives = new QActionGroup(ui->menuWidgets);
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(addLink()));
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(UAScreated(UASInterface*)));
//    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));
//    connect(UASManager::instance(), SIGNAL(UASDeleted(UASInterface*)), this, SLOT(UASDeleted(UASInterface*)));


}


void MainWindow::showTool(bool show)
{
    QAction* act = qobject_cast<QAction *>(sender());
    QWidget* widget = act->data().value<QWidget *>();
    widget->setVisible(show);
}

void MainWindow::initActionsConnections()
{
    //TODO:  move protocol outside UI
    mavlink     = new MAVLinkProtocol();
    connect(mavlink, SIGNAL(protocolStatusMessage(QString,QString)), this, SLOT(showCriticalMessage(QString,QString)), Qt::QueuedConnection);

    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(addLink()));
    //    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));



    //===== Toolbar Status =====

    toolBarTimeoutLabel = new QLabel(tr("NOT CONNECTED"), this);
    toolBarTimeoutLabel->setToolTip(tr("System connection status, interval since last message if timed out."));
    toolBarTimeoutLabel->setObjectName("toolBarTimeoutLabel");
    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { margin: 0px 2px; font: 18px; color: #3C7B9E; }"));
    ui->mainToolBar->addWidget(toolBarTimeoutLabel);

    toolBarSafetyLabel = new QLabel(tr("SAFE"), this);
    toolBarSafetyLabel->setStyleSheet(QString("QLabel { margin: 0px 2px; font: 18px; color: #008000; }"));
    toolBarSafetyLabel->setToolTip(tr("Vehicle safety state"));
    toolBarSafetyLabel->setObjectName("toolBarSafetyLabel");
    ui->mainToolBar->addWidget(toolBarSafetyLabel);

    //    toolBarStateLabel = new QLabel("------", this);
    //    toolBarStateLabel->setStyleSheet(QString("QLabel { margin: 0px 2px; font: 18px; color: #FFFF00; }"));
    //    toolBarStateLabel->setToolTip(tr("Vehicle state"));
    //    toolBarStateLabel->setObjectName("toolBarStateLabel");
    //    ui->mainToolBar->addWidget(toolBarStateLabel);

    toolBarBatteryBar = new QProgressBar(this);
    toolBarBatteryBar->setStyleSheet("QProgressBar:horizontal { margin: 0px 4px 0px 0px; border: 1px solid #4A4A4F; border-radius: 4px; text-align: center; padding: 2px; color: #111111; background-color: #111118; height: 10px; } QProgressBar:horizontal QLabel { font-size: 9px; color: #111111; } QProgressBar::chunk { background-color: green; }");
    toolBarBatteryBar->setMinimum(0);
    toolBarBatteryBar->setMaximum(100);
    toolBarBatteryBar->setMinimumWidth(20);
    toolBarBatteryBar->setMaximumWidth(100);
    toolBarBatteryBar->setToolTip(tr("Battery charge level"));
    toolBarBatteryBar->setObjectName("toolBarBatteryBar");
    ui->mainToolBar->addWidget(toolBarBatteryBar);

    toolBarBatteryVoltageLabel = new QLabel("xx.x V");
    toolBarBatteryVoltageLabel->setStyleSheet(QString("QLabel {  margin: 0px 2px; font: 18px; color: %1; }").arg(QColor(Qt::green).name()));
    toolBarBatteryVoltageLabel->setToolTip(tr("Battery voltage"));
    toolBarBatteryVoltageLabel->setObjectName("toolBarBatteryVoltageLabel");
    ui->mainToolBar->addWidget(toolBarBatteryVoltageLabel);

    //    toolBarMessageLabel = new QLabel(tr("No system messages."), this);
    //    toolBarMessageLabel->setToolTip(tr("Most recent system message"));
    //    toolBarMessageLabel->setObjectName("toolBarMessageLabel");
    //    ui->mainToolBar->addWidget(toolBarMessageLabel);

    updateView();

}

QString MainWindow::getWindowGeometryKey()
{
    return "_geometry";
}
void MainWindow::bytesReceived(QByteArray data)
{
    mavlink_message_t message;
    mavlink_status_t status;

    for (int position = 0; position < data.size(); position++)
    {
        unsigned int decodeState = mavlink_parse_char(0, (uint8_t)(data[position]), &message, &status);

        if (decodeState == 1)
        {
            receiveMessage(message);
        }
    }

}

mavlink_message_info_t message_info[256] = MAVLINK_MESSAGE_INFO;
void MainWindow::print_one_field(const mavlink_message_t *msg, const mavlink_field_info_t *f, int idx)
{
#define PRINT_FORMAT(f, def) (f->print_format?f->print_format:def)
    switch (f->type) {
    case MAVLINK_TYPE_CHAR:
        qDebug(PRINT_FORMAT(f, "%c"), _MAV_RETURN_char(msg, f->wire_offset+idx*1));
        break;
    case MAVLINK_TYPE_UINT8_T:
        qDebug(PRINT_FORMAT(f, "%u"), _MAV_RETURN_uint8_t(msg, f->wire_offset+idx*1));
        break;
    case MAVLINK_TYPE_INT8_T:
        qDebug(PRINT_FORMAT(f, "%d"), _MAV_RETURN_int8_t(msg, f->wire_offset+idx*1));
        break;
    case MAVLINK_TYPE_UINT16_T:
        qDebug(PRINT_FORMAT(f, "%u"), _MAV_RETURN_uint16_t(msg, f->wire_offset+idx*2));
        break;
    case MAVLINK_TYPE_INT16_T:
        qDebug(PRINT_FORMAT(f, "%d"), _MAV_RETURN_int16_t(msg, f->wire_offset+idx*2));
        break;
    case MAVLINK_TYPE_UINT32_T:
        qDebug(PRINT_FORMAT(f, "%lu"), (unsigned long)_MAV_RETURN_uint32_t(msg, f->wire_offset+idx*4));
        break;
    case MAVLINK_TYPE_INT32_T:
        qDebug(PRINT_FORMAT(f, "%ld"), (long)_MAV_RETURN_int32_t(msg, f->wire_offset+idx*4));
        break;
    case MAVLINK_TYPE_UINT64_T:
        qDebug(PRINT_FORMAT(f, "%llu"), (unsigned long long)_MAV_RETURN_uint64_t(msg, f->wire_offset+idx*8));
        break;
    case MAVLINK_TYPE_INT64_T:
        qDebug(PRINT_FORMAT(f, "%lld"), (long long)_MAV_RETURN_int64_t(msg, f->wire_offset+idx*8));
        break;
    case MAVLINK_TYPE_FLOAT:
        qDebug(PRINT_FORMAT(f, "%f"), (double)_MAV_RETURN_float(msg, f->wire_offset+idx*4));
        break;
    case MAVLINK_TYPE_DOUBLE:
        qDebug(PRINT_FORMAT(f, "%f"), _MAV_RETURN_double(msg, f->wire_offset+idx*8));
        break;
    }
}

void MainWindow::print_field(const mavlink_message_t *msg, const mavlink_field_info_t *f)
{
    qDebug("%s: ", f->name);
    //    out << (f->name) << "\r\n";
    if (f->array_length == 0) {
        print_one_field(msg, f, 0);
        qDebug(" ");
    } else {
        unsigned i;
        /* print an array */
        if (f->type == MAVLINK_TYPE_CHAR) {
            qDebug("'%.*s'", f->array_length,
                   f->wire_offset+(const char *)_MAV_PAYLOAD(msg));

        } else {
            qDebug("[ ");
            for (i=0; i<f->array_length; i++) {
                print_one_field(msg, f, i);
                if (i < f->array_length) {
                    qDebug(", ");
                }
            }
            qDebug("]");
        }
    }
    qDebug(" ");
}

void MainWindow::print_message(const mavlink_message_t *msg)
{
    const mavlink_message_info_t *m = &message_info[msg->msgid];
    const mavlink_field_info_t *f = m->fields;
    unsigned i;

    qDebug("%s { ", m->name);
    //    out << (m->name) << " { \r\n";

    for (i=0; i< m->num_fields; i++) {
        print_field(msg, &f[i]);
    }
    qDebug("}\n");
    //    out << "} \r\n";
}

void MainWindow::receiveMessage(mavlink_message_t message)
{
    memcpy(receivedMessages+message.msgid, &message, sizeof(mavlink_message_t));

    //qDebug() << message.payload64;
    print_message(&message);

    switch(message.msgid)
    {
    case MAVLINK_MSG_ID_SYS_STATUS:
        break;
    case MAVLINK_MSG_ID_ATTITUDE:

        break;
    case MAVLINK_MSG_ID_SET_MODE:
    {
        mavlink_set_mode_t mode;
        mavlink_msg_set_mode_decode(&message,&mode);
        if (message.sysid == mode.target_system)
            sys_mode = mode.base_mode;
    }
        break;
    case MAVLINK_MSG_ID_HIL_STATE:
    {
        qDebug() << "==================================================================hil state \n";
        mavlink_hil_state_t state;
        mavlink_msg_hil_state_decode(&message,&state);
        roll = state.roll;
        pitch = state.pitch;
        yaw = state.yaw;
        rollspeed = state.rollspeed;
        pitchspeed = state.pitchspeed;
        yawspeed = state.yawspeed;
        latitude = state.lat;
        longitude = state.lon;
        altitude = state.alt;

        qDebug() << "Roll: " << roll << "\n";
        qDebug() << "Pitch: " << pitch << "\n";
        qDebug() << "Yaw: " << yaw << "\n";
        qDebug() << "RollSpeed: " << rollspeed << "\n";
        qDebug() << "PitchSpeed: " << pitchspeed << "\n";
        qDebug() << "YawSpeed: " << yawspeed << "\n";
    }
        break;
    }
}
void MainWindow::addLink()
{
    SerialLink* tmplink = new SerialLink();
    link = tmplink;
    LinkManager::instance()->add(link);
    LinkManager::instance()->addProtocol(link, mavlink);
    qDebug() << link->getBitsReceived();
    link->connect();
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);
    ui->statusBar->showMessage(tr("Connected"));
    //connect(active, SIGNAL(heartbeatTimeout(bool, unsigned int)), this, SLOT(heartbeatTimeout(bool,unsigned int)));

    toolBarTimeoutLabel->setStyleSheet(QString("QLabel { padding:0 3px; background-color: #FF0000; }"));
    toolBarTimeoutLabel->setText(tr("CONNECTION"));

    QAction* act = getActionByLink(link);
    if (act)
        act->trigger();


}

void MainWindow::addLink(LinkInterface *link)
{
    LinkManager::instance()->add(link);
    LinkManager::instance()->addProtocol(link, mavlink);
    link->connect();

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);
    ui->statusBar->showMessage(tr("Connected"));
}

void MainWindow::showMessage(const QString &title, const QString &message, const QString &details, const QString severity)
{
    QMessageBox msgBox(this);
    //msgBox.setWindowFlags(Qt::Dialog);
    if (severity == "critical")
        msgBox.setIcon(QMessageBox::Critical);
    else if  (severity == "warning")
        msgBox.setIcon(QMessageBox::Warning);
    else
        msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(title.leftJustified(300, ' '));
    msgBox.setInformativeText(message);
    if (!details.isEmpty())
        msgBox.setDetailedText(details);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}


void MainWindow::showCriticalMessage(const QString& title, const QString& message)
{
    showMessage(title, message, "", "critical");
}

void MainWindow::setActiveUAS(UASInterface *active)
{
    // Do nothing if system is the same or NULL
    if ((active == NULL) || mav == active) return;

    if (mav)
    {
        // Disconnect old system
        disconnect(mav, 0, this, 0);
    }

    // Connect new system
    mav = active;
    //connected or not
    connect(active,SIGNAL(heartbeatTimeout(bool,uint)),this,SLOT(heartbeatTimeout(bool,uint)));
    //update battery
    connect(active, SIGNAL(batteryChanged(UASInterface*,double,double,int)), this, SLOT(updateBatteryRemaining(UASInterface*,double,double,int)));
    //update arm or not
    connect(active, SIGNAL(armingChanged(bool)), this, SLOT(updateArmingState(bool)));

    systemArmed = mav->isArmed();

    //    toolBarTimeoutLabel->setStyleSheet(QString(" padding: 0;"));
    //    toolBarTimeoutLabel->setText("");
}

void MainWindow::updateView()
{
    if (!changed) return;

    setActiveUAS(UASManager::instance()->getActiveUAS());
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    toolBarBatteryBar->setValue(batteryPercent);
    toolBarBatteryVoltageLabel->setText(tr("%1 V").arg(batteryVoltage, 4, 'f', 1, ' '));

    if (systemArmed)
    {
        toolBarSafetyLabel->setStyleSheet(QString("QLabel {margin: 0px 2px; font: 18px; color: %1; background-color: %2; }").arg(QGC::colorRed.name()).arg(QGC::colorYellow.name()));
        toolBarSafetyLabel->setText(tr("ARMED"));
    }
    else
    {
        toolBarSafetyLabel->setStyleSheet("QLabel {margin: 0px 2px; font: 18px; color: #14C814; }");
        toolBarSafetyLabel->setText(tr("SAFE"));
    }

    changed = false;

}
