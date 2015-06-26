#include "debugconsole.h"
#include "ui_debugconsole.h"

#include "linkmanager.h"
#include "uasmanager.h"
#include "common/mavlink_types.h"
#include "string.h"
#include "qgc.h"
#include "mainwindow.h"

#include <QPainter>
#include <QSettings>
#include <QDebug>

DebugConsole::DebugConsole(QWidget *parent) :
    QWidget(parent),
    currLink(NULL),
    holdOn(true),
    convertToAscii(true),
    filterMAVLINK(false),
    autoHold(true),
    bytesToIgnore(0),
    lastByte(-1),
    escReceived(false),
    escIndex(0),
    sentBytes(),
    holdBuffer(),
    lineBuffer(""),
    lastLineBuffer(0),
    lineBufferTimer(),
    snapShotTimer(),
    snapShotInterval(500),
    snapShotBytes(0),
    dataRate(0.0f),
    lowpassDataRate(0.0f),
    dataRateThreshold(400),
    commandIndex(0),
    m_ui(new Ui::DebugConsole)
{
    // Setup basic user interface
    m_ui->setupUi(this);
    // Hide sent text field - it is only useful after send has been hit
    m_ui->sentText->setVisible(false);
    // Hide auto-send checkbox
    //m_ui->specialCheckBox->setVisible(false);
    // Make text area not editable
    m_ui->receiveText->setReadOnly(false);
    // Limit to 500 lines
    m_ui->receiveText->setMaximumBlockCount(500);
    // Allow to wrap everywhere
    m_ui->receiveText->setWordWrapMode(QTextOption::WrapAnywhere);
//    // Set monospace font
//    m_ui->receiveText->setFontFamily("Monospace");

    m_ui->connectButton->setProperty("type", "toggle-ok-warn");

    // Enable 10 Hz output
    //connect(&lineBufferTimer, SIGNAL(timeout()), this, SLOT(showData()));
    //lineBufferTimer.setInterval(100); // 100 Hz
    //lineBufferTimer.start();
    loadSettings();

    m_ui->holdButton->setChecked(holdOn);

    on_checkBox_simpleView_toggled(m_ui->checkBox_simpleView->isChecked());

    // Enable traffic measurements
    connect(&snapShotTimer, SIGNAL(timeout()), this, SLOT(updateTrafficMeasurements()));
    snapShotTimer.setInterval(snapShotInterval);
    snapShotTimer.start();
    // Update measurements the first time
    updateTrafficMeasurements();

    // Get a list of all existing links
    links = QList<LinkInterface*>();
    foreach (LinkInterface* link, LinkManager::instance()->getLinks()) {
        addLink(link);
    }

    // Connect to link manager to get notified about new links
    connect(LinkManager::instance(), SIGNAL(newLink(LinkInterface*)), this, SLOT(addLink(LinkInterface*)));
    // Connect link combo box
    connect(m_ui->linkComboBox, SIGNAL(activated(int)), this, SLOT(linkSelected(int)));
    // Connect send button
    connect(m_ui->transmitButton, SIGNAL(clicked()), this, SLOT(sendBytes()));
    // Connect HEX conversion and MAVLINK filter checkboxes
    connect(m_ui->mavlinkCheckBox, SIGNAL(clicked(bool)), this, SLOT(MAVLINKfilterEnabled(bool)));
    connect(m_ui->hexCheckBox, SIGNAL(clicked(bool)), this, SLOT(hexModeEnabled(bool)));
    connect(m_ui->holdCheckBox, SIGNAL(clicked(bool)), this, SLOT(setAutoHold(bool)));
    // Connect hold button
    connect(m_ui->holdButton, SIGNAL(toggled(bool)), this, SLOT(hold(bool)));
    // Connect connect button
    connect(m_ui->connectButton, SIGNAL(clicked()), this, SLOT(handleConnectButton()));
    // Connect the special chars combo box
    connect(m_ui->addSymbolButton, SIGNAL(clicked()), this, SLOT(appendSpecialSymbol()));
    // Connect Checkbox
    connect(m_ui->specialComboBox, SIGNAL(highlighted(QString)), this, SLOT(specialSymbolSelected(QString)));
    // Allow to send via return
    connect(m_ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendBytes()));
}

void DebugConsole::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    storeSettings();
}

DebugConsole::~DebugConsole()
{
    storeSettings();
    delete m_ui;
}

void DebugConsole::dockEvent(Qt::DockWidgetArea area)
{
    if ((area & (Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea)))
        m_ui->checkBox_simpleView->setChecked(true);
}

void DebugConsole::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    settings.beginGroup("QGC_DEBUG_CONSOLE");
    m_ui->specialComboBox->setCurrentIndex(settings.value("SPECIAL_SYMBOL", m_ui->specialComboBox->currentIndex()).toInt());
    m_ui->specialCheckBox->setChecked(settings.value("SPECIAL_SYMBOL_CHECKBOX_STATE", m_ui->specialCheckBox->isChecked()).toBool());
    m_ui->checkBox_simpleView->setChecked(settings.value("VIEW_MODE_SIMPLE", m_ui->checkBox_simpleView->isChecked()).toBool());
    hexModeEnabled(settings.value("HEX_MODE_ENABLED", m_ui->hexCheckBox->isChecked()).toBool());
    MAVLINKfilterEnabled(settings.value("MAVLINK_FILTER_ENABLED", filterMAVLINK).toBool());
    holdOn = settings.value("HOLD_ENABLED", holdOn).toBool();
    setAutoHold(settings.value("AUTO_HOLD_ENABLED", autoHold).toBool());
    settings.endGroup();

//    // Update visibility settings
//    if (m_ui->specialCheckBox->isChecked())
//    {
//        m_ui->specialCheckBox->setVisible(true);
//        m_ui->addSymbolButton->setVisible(false);
//    }
}

void DebugConsole::storeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("QGC_DEBUG_CONSOLE");
    settings.setValue("SPECIAL_SYMBOL", m_ui->specialComboBox->currentIndex());
    settings.setValue("SPECIAL_SYMBOL_CHECKBOX_STATE", m_ui->specialCheckBox->isChecked());
    settings.setValue("HEX_MODE_ENABLED", m_ui->hexCheckBox->isChecked());
    settings.setValue("VIEW_MODE_SIMPLE", m_ui->checkBox_simpleView->isChecked());
    settings.setValue("MAVLINK_FILTER_ENABLED", filterMAVLINK);
    settings.setValue("AUTO_HOLD_ENABLED", autoHold);
    settings.setValue("HOLD_ENABLED", holdOn);
    settings.endGroup();
    settings.sync();
    //qDebug() << "Storing settings!";
}

/**
 * Add a link to the debug console output
 */
void DebugConsole::addLink(LinkInterface* link)
{
    if (!link)
        return;

    // Add link to link list
    links.insert(link->getId(), link);

    m_ui->linkComboBox->insertItem(link->getId(), link->getName());
    // Set new item as current
    m_ui->linkComboBox->setCurrentIndex(qMax(0, links.size() - 1));
    linkSelected(m_ui->linkComboBox->currentIndex());

    // Register for name changes
    connect(link, SIGNAL(nameChanged(QString)), this, SLOT(updateLinkName(QString)));
    connect(link, SIGNAL(deleteLink(LinkInterface* const)), this, SLOT(removeLink(LinkInterface* const)));
    connect(link, SIGNAL(connected(bool)), this, SLOT(linkStatusChanged(bool)));
}

void DebugConsole::removeLink(LinkInterface* const linkInterface)
{
    int linkIndex = links.indexOf(linkInterface);
    if (linkIndex != -1) {
        disconnect(linkInterface, 0, this, 0);
        links.removeAt(linkIndex);

        if (linkInterface == currLink)
            currLink = NULL;

        m_ui->linkComboBox->removeItem(linkIndex);
        linkSelected(m_ui->linkComboBox->currentIndex());
    }
}

void DebugConsole::linkSelected(int linkIdx)
{
    if (currLink && links.indexOf(currLink) == linkIdx)
        return;

    // Disconnect
    if (currLink) {
        disconnect(currLink, SIGNAL(bytesReceived(LinkInterface*,QByteArray)), this, SLOT(receiveBytes(LinkInterface*, QByteArray)));
    }

    // Clear data
    //m_ui->receiveText->clear();

    // Connect new link
    if (linkIdx > -1 && links.size() > linkIdx) {
        currLink = links[linkIdx];
        connect(currLink, SIGNAL(bytesReceived(LinkInterface*,QByteArray)), this, SLOT(receiveBytes(LinkInterface*, QByteArray)));
        setConnectionState(currLink->isConnected());
    }
}

/**
 * @param name new name for this link - the link is determined to the sender to this slot by QObject::sender()
 */
void DebugConsole::updateLinkName(QString name)
{
    // Set name if signal came from a link
    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    //if (link != NULL) m_ui->linkComboBox->setItemText(link->getId(), name);
    if((link != NULL) && (links.contains(link)))
    {
        const qint16 &linkIndex(links.indexOf(link));
        m_ui->linkComboBox->setItemText(linkIndex,name);
    }
}

void DebugConsole::linkStatusChanged(bool connected)
{
    Q_UNUSED(connected)
    if (!sender())
        return;
    LinkInterface *link = qobject_cast<LinkInterface *>(sender());
    if (!link)
        return;
    setLinkState(link);
}

void DebugConsole::setLinkState(LinkInterface *link)
{
    QString state;
    QString color;
    bool connected = link->isConnected();
    if(connected) {
        state = tr("connected");
        color = QGC::colorGreen.name();
    } else {
        state = tr("disconnected");
        color = QGC::colorOrange.name();
    }
    m_ui->receiveText->appendHtml(QString("<font color=\"%1\">Link %2 %3.</font>\n").arg(color, link->getName(), state));

    if (currLink && link->getId() == currLink->getId()){
        setConnectionState(connected);
    }
}

void DebugConsole::setConnectionState(bool connected)
{
    m_ui->connectButton->setChecked(connected);
    m_ui->connectButton->setText(connected ? tr("Disconn.") : tr("Connect"));
}

void DebugConsole::setAutoHold(bool hold)
{
    // Disable current hold if hold had been enabled
    if (autoHold && holdOn && !hold) {
        this->hold(false);
        m_ui->holdButton->setChecked(false);
    }
    // Set auto hold checkbox
    if (m_ui->holdCheckBox->isChecked() != hold) {
        m_ui->holdCheckBox->setChecked(hold);
    }

    if (!hold)
    {
        // Warn user about not activated hold
        m_ui->receiveText->appendHtml(QString("<font color=\"%1\">%2</font>\n").arg(QColor(Qt::red).name(), tr("WARNING: You have NOT enabled auto-hold (stops updating the console if huge amounts of serial data arrive). Updating the console consumes significant CPU load, so if you receive more than about 5 KB/s of serial data, make sure to enable auto-hold if not using the console.")));
    }
    else
    {
        m_ui->receiveText->clear();
    }

    // Set new state
    autoHold = hold;
}

/**
 * Prints the message in the UAS color
 */
void DebugConsole::receiveTextMessage(int id, int component, int severity, QString text)
{
    Q_UNUSED(component);
    Q_UNUSED(severity);
    if (isVisible())
    {
        QString name = UASManager::instance()->getUASForId(id)->getUASName();
//        QString comp;
//        // Get a human readable name if possible
//        switch (component) {
//            // TODO: To be completed
//        case MAV_COMP_ID_IMU:
//            comp = tr("IMU");
//            break;
//        case MAV_COMP_ID_MAPPER:
//            comp = tr("MAPPER");
//            break;
//        case MAV_COMP_ID_MISSIONPLANNER:
//            comp = tr("MISSION");
//            break;
//        case MAV_COMP_ID_SYSTEM_CONTROL:
//            comp = tr("SYS-CONTROL");
//            break;
//        default:
//            comp = QString::number(component);
//            break;
//        }

//        m_ui->receiveText->appendHtml(QString("<font color=\"%1\">(%2:%3) %4</font>\n").arg(UASManager::instance()->getUASForId(id)->getColor().name(), name, comp, text));
        m_ui->receiveText->appendHtml(QString("<font color=\"%1\">(%2) %4</font>\n").arg(UASManager::instance()->getUASForId(id)->getColor().name(), name, text));
        // Ensure text area scrolls correctly
        m_ui->receiveText->ensureCursorVisible();
    }
}

void DebugConsole::updateTrafficMeasurements()
{
    lowpassDataRate = lowpassDataRate * 0.9f + (0.1f * ((float)snapShotBytes / (float)snapShotInterval) * 1000.0f);
    dataRate = ((float)snapShotBytes / (float)snapShotInterval) * 1000.0f;
    snapShotBytes = 0;

    // Check if limit has been exceeded
    if ((lowpassDataRate > dataRateThreshold) && autoHold) {
        // Enable auto-old
        m_ui->holdButton->setChecked(true);
        hold(true);
    }

    QString speed;
    speed = speed.sprintf("%04.1f kB/s", dataRate/1000.0f);
    m_ui->speedLabel->setText(speed);

    if (holdOn) {
        //repaint();
    }
}

//QPainter painter(m_ui->receiveText);
//painter.setRenderHint(QPainter::HighQualityAntialiasing);
//painter.translate((this->vwidth/2.0+xCenterOffset)*scalingFactor, (this->vheight/2.0+yCenterOffset)*scalingFactor);

void DebugConsole::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // Update bandwidth
//    if (holdOn)
//    {
//        //qDebug() << "Data rate:" << dataRate/1000.0f << "kB/s";
//        QString rate("data rate: %1");
//        rate.arg(dataRate);
//        QPainter painter(this);
//        painter.setRenderHint(QPainter::HighQualityAntialiasing);
//        painter.translate(width()/5.0f, height()/5.0f);



//        //QFont font("Bitstream Vera Sans");
//        QFont font = painter.font();
//        font.setPixelSize((int)(60.0f));

//        QFontMetrics metrics = QFontMetrics(font);
//        int border = qMax(4, metrics.leading());
//        QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
//                                          Qt::AlignLeft | Qt::TextWordWrap, rate);
//        painter.setPen(QColor(255, 50, 50));
//        painter.setRenderHint(QPainter::TextAntialiasing);
//        painter.drawText(QRect(QPoint(static_cast<int>(width()/5.0f), static_cast<int>(height()/5.0f)), QPoint(static_cast<int>(width() - width()/5.0f), static_cast<int>(height() - height()/5.0f))), rate);
//        //Qt::AlignRight | Qt::TextWordWrap
//    }
}

void DebugConsole::receiveBytes(LinkInterface* link, QByteArray bytes)
{
    snapShotBytes += bytes.size();
    int len = bytes.size();
    int lastSpace = 0;
    if ((this->bytesToIgnore > 260) || (this->bytesToIgnore < -2)) this->bytesToIgnore = 0;
    // Only add data from current link
    if (link == currLink && !holdOn)
    {
        // Parse all bytes
        for (int j = 0; j < len; j++)
        {
            unsigned char byte = bytes.at(j);
            // Filter MAVLink (http://qgroundcontrol.org/mavlink/) messages out of the stream.
            if (filterMAVLINK)
            {
                if (this->bytesToIgnore > 0)
                {
                    if ( (j + this->bytesToIgnore) < len )
                        j += this->bytesToIgnore - 1, this->bytesToIgnore = 1;
                    else
                        this->bytesToIgnore -= (len - j - 1), j = len - 1;
                } else
                if (this->bytesToIgnore == -2)
                {   // Payload plus header - but we got STX already
                    this->bytesToIgnore = static_cast<unsigned int>(byte) + MAVLINK_NUM_NON_PAYLOAD_BYTES - 1;
                    if ( (j + this->bytesToIgnore) < len )
                        j += this->bytesToIgnore - 1, this->bytesToIgnore = 1;
                    else
                        this->bytesToIgnore -= (len - j - 1), j = len - 1;
                } else
                // Filtering is done by setting an ignore counter based on the MAVLINK packet length
                if (static_cast<unsigned char>(byte) == MAVLINK_STX)
                {
                    this->bytesToIgnore = -1;
                } else
                    this->bytesToIgnore = 0;
            } else this->bytesToIgnore = 0;

            if ( (this->bytesToIgnore <= 0) && (this->bytesToIgnore != -1) )
            {
                QString str;
                // Convert to ASCII for readability
                if (convertToAscii)
                {
                    if (escReceived)
                    {
                        if (escIndex < static_cast<int>(sizeof(escBytes)))
                        {
                            escBytes[escIndex] = byte;
                            //qDebug() << "GOT BYTE ESC:" << byte;
                            if (/*escIndex == 1 && */escBytes[escIndex] == 0x48)
                            {
                                // Handle sequence
                                // for this one, clear all text
                                m_ui->receiveText->clear();
                                escReceived = false;
                            }
                            else if (/*escIndex == 1 && */escBytes[escIndex] == 0x4b)
                            {
                                // Handle sequence
                                // for this one, do nothing
                                escReceived = false;
                            }
                            else if (byte == 0x5b)
                            {
                                // Do nothing, this is still a valid escape sequence
                            }
                            else
                            {
                                escReceived = false;
                            }
                         }
                        else
                        {
                            // Obviously something went wrong, reset
                            escReceived = false;
                            escIndex = 0;
                        }
                    }
                    else if ((byte <= 32) || (byte > 126))
                    {
                        switch (byte)
                        {
                            case (unsigned char)'\n':   // Accept line feed
                                if (lastByte != '\r')   // Do not break line again for LF+CR
                                    str.append(byte);   // only break line for single LF or CR bytes
                            break;
                            case (unsigned char)' ':    // space of any type means don't add another on hex output
                            case (unsigned char)'\t':   // Accept tab
                            case (unsigned char)'\r':   // Catch and carriage return
                                if (lastByte != '\n')   // Do not break line again for CR+LF
                                str.append(byte);       // only break line for single LF or CR bytes
                                lastSpace = 1;
                            break;
                            /* VT100 emulation (partially */
                            case 0x1b:                  // ESC received
                                escReceived = true;
                                escIndex = 0;
                                //qDebug() << "GOT ESC";
                                break;
                            case 0x08:                  // BS (backspace) received
                                // Do nothing for now
                                break;
                            default:                    // Append replacement character (box) if char is not ASCII
//                                str.append(QChar(QChar::ReplacementCharacter));
                                QString str2;
                                if ( lastSpace == 1)
                                    str2.sprintf("0x%02x ", byte);
                                else str2.sprintf(" 0x%02x ", byte);
                                str.append(str2);
                                lastSpace = 1;
                                escReceived = false;
                            break;
                        }
                    }
                    else
                    {
                        // Ignore carriage return, because that
                        // is auto-added with '\n'
                        if (byte != '\r') str.append(byte);           // Append original character
                        lastSpace = 0;
                    }
                }
                else
                {
                    QString str2;
                    str2.sprintf("%02x ", byte);
                    str.append(str2);
                }
                lineBuffer.append(str);
                lastByte = byte;
            }
            else
            {
                if (filterMAVLINK) this->bytesToIgnore--;
                // Constrain bytes to positive range
//                bytesToIgnore = qMax(0, bytesToIgnore);
            }

        }
        // Plot every 200 ms if windows is visible
        if (lineBuffer.length() > 0 && (QGC::groundTimeMilliseconds() - lastLineBuffer) > 200) {
            if (isVisible())
            {
                m_ui->receiveText->appendPlainText(lineBuffer);
                lineBuffer.clear();
                lastLineBuffer = QGC::groundTimeMilliseconds();
                // Ensure text area scrolls correctly
                m_ui->receiveText->ensureCursorVisible();
            }
            if (lineBuffer.size() > 8192)
            {
                lineBuffer.remove(0, 4096);
            }
        }
    }
    else if (link == currLink && holdOn)
    {
        holdBuffer.append(bytes);
        if (holdBuffer.size() > 8192)
            holdBuffer.remove(0, 4096); // drop old stuff
    }
}

QByteArray DebugConsole::symbolNameToBytes(const QString& text)
{
    QByteArray b;
    if (text.contains("CR+LF")) {
        b.append(static_cast<char>(0x0D));
        b.append(static_cast<char>(0x0A));
    } else if (text.contains("LF")) {
        b.append(static_cast<char>(0x0A));
    } else if (text.contains("FF")) {
        b.append(static_cast<char>(0x0C));
    } else if (text.contains("CR")) {
        b.append(static_cast<char>(0x0D));
    } else if (text.contains("TAB")) {
        b.append(static_cast<char>(0x09));
    } else if (text.contains("NUL")) {
        b.append(static_cast<char>(0x00));
    } else if (text.contains("ESC")) {
        b.append(static_cast<char>(0x1B));
    } else if (text.contains("~")) {
        b.append(static_cast<char>(0x7E));
    } else if (text.contains("<Space>")) {
        b.append(static_cast<char>(0x20));
    }
    return b;
}

QString DebugConsole::bytesToSymbolNames(const QByteArray& b)
{
    QString text;
    if (b.size() > 1 && b.contains(0x0D) && b.contains(0x0A)) {
        text = "<CR+LF>";
    } else if (b.contains(0x0A)) {
        text = "<LF>";
    } else if (b.contains(0x0C)) {
        text = "<FF>";
    } else if (b.contains(0x0D)) {
        text = "<CR>";
    } else if (b.contains(0x09)) {
        text = "<TAB>";
    } else if (b.contains((char)0x00)) {
        text = "<NUL>";
    } else if (b.contains(0x1B)) {
        text = "<ESC>";
    } else if (b.contains(0x7E)) {
        text = "<~>";
    } else if (b.contains(0x20)) {
        text = "<Space>";
    } else {
        text.append(b);
    }
    return text;
}

void DebugConsole::specialSymbolSelected(const QString& text)
{
    Q_UNUSED(text);
    //m_ui->specialCheckBox->setVisible(true);
}

void DebugConsole::appendSpecialSymbol(const QString& text)
{
    QString line = m_ui->sendText->text();
    QByteArray symbols = symbolNameToBytes(text);
    // The text is appended to the enter field
    if (convertToAscii) {
        line.append(symbols);
    } else {

        for (int i = 0; i < symbols.size(); i++) {
            QString add(" 0x%1");
            line.append(add.arg(static_cast<char>(symbols.at(i)), 2, 16, QChar('0')));
        }
    }
    m_ui->sendText->setText(line);
}

void DebugConsole::appendSpecialSymbol()
{
    appendSpecialSymbol(m_ui->specialComboBox->currentText());
}

void DebugConsole::sendBytes()
{
    // FIXME This store settings should be removed
    // once all threading issues have been resolved
    // since its called in the destructor, which
    // is absolutely sufficient
    storeSettings();

    // Store command history
    commandHistory.append(m_ui->sendText->text());
    // Since text was just sent, we're at position commandHistory.length()
    // which is the current text
    commandIndex = commandHistory.length();

    if (!m_ui->sentText->isVisible()) {
        m_ui->sentText->setVisible(true);
    }

    if (!currLink->isConnected()) {
        m_ui->sentText->setText(tr("Nothing sent. The link %1 is unconnected. Please connect first.").arg(currLink->getName()));
        return;
    }

    QString transmitUnconverted = m_ui->sendText->text();
    QByteArray specialSymbol;

    // Append special symbol if checkbox is checked
    if (m_ui->specialCheckBox->isChecked()) {
        // Get auto-add special symbols
        specialSymbol = symbolNameToBytes(m_ui->specialComboBox->currentText());

        // Convert them if needed
        if (!convertToAscii) {
            QString specialSymbolConverted;
            for (int i = 0; i < specialSymbol.length(); i++) {
                QString add(" 0x%1");
                specialSymbolConverted.append(add.arg(static_cast<char>(specialSymbol.at(i)), 2, 16, QChar('0')));
            }
            specialSymbol.clear();
            specialSymbol.append(specialSymbolConverted);
        }
    }

    QByteArray transmit;
    QString feedback;
    bool ok = true;
    if (convertToAscii) {
        // ASCII text is not converted
        transmit = transmitUnconverted.toLatin1();
        // Auto-add special symbol handling
        transmit.append(specialSymbol);

        QString translated;

        // Replace every occurence of a special symbol with its text name
        for (int i = 0; i < transmit.size(); ++i) {
            QByteArray specialChar;
            specialChar.append(transmit.at(i));
            translated.append(bytesToSymbolNames(specialChar));
        }

        feedback.append(translated);
    } else {
        // HEX symbols are converted to bytes
        QString str = transmitUnconverted.toLatin1();
        str.append(specialSymbol);
        str.remove(' ');
        str.remove("0x");
        str = str.simplified();
        int bufferIndex = 0;
        if ((str.size() % 2) == 0) {
            for (int i = 0; i < str.size(); i=i+2) {
                bool okByte;
                QString strBuf = QString(str.at(i));
                strBuf.append(str.at(i+1));
                unsigned char hex = strBuf.toInt(&okByte, 16);
                ok = (ok && okByte);
                transmit[bufferIndex++] = hex;

                if (okByte) {
                    // Feedback
                    //feedback.append("0x");
                    feedback.append(str.at(i).toUpper());
                    feedback.append(str.at(i+1).toUpper());
                    feedback.append(" ");
                } else {
                    feedback = tr("HEX format error near \"") + strBuf + "\"";
                }
            }
        } else {
            ok = false;
            feedback = tr("HEX values have to be in pairs, e.g. AA or AA 05");
        }
    }

    // Transmit ASCII or HEX formatted text, only if more than one symbol
    if (ok && m_ui->sendText->text().toLatin1().size() > 0) {
        // Transmit only if conversion succeeded
        //        int transmitted =
        currLink->writeBytes(transmit, transmit.size());
        //        if (transmit.size() == transmitted)
        //        {
        m_ui->sentText->setText(tr("Sent: ") + feedback);
        //        }
        //        else
        //        {
        //            m_ui->sentText->setText(tr("Error during sending: Transmitted only %1 bytes instead of %2.").arg(transmitted, transmit.size()));
        //        }
    } else if (m_ui->sendText->text().toLatin1().size() > 0) {
        // Conversion failed, display error message
        m_ui->sentText->setText(tr("Not sent: ") + feedback);
    }

    // Select text to easy follow-up input from user
    m_ui->sendText->selectAll();
    m_ui->sendText->setFocus(Qt::OtherFocusReason);
}

/**
 * @param mode true to convert all in and output to/from HEX, false to send and receive ASCII values
 */
void DebugConsole::hexModeEnabled(bool mode)
{
    if (convertToAscii == mode) {
        convertToAscii = !mode;
        if (m_ui->hexCheckBox->isChecked() != mode) {
            m_ui->hexCheckBox->setChecked(mode);
        }
        m_ui->receiveText->clear();
        m_ui->sendText->clear();
        m_ui->sentText->clear();
        commandHistory.clear();
    }
}

/**
 * @param filter true to ignore all MAVLINK raw data in output, false, to display all incoming data
 */
void DebugConsole::MAVLINKfilterEnabled(bool filter)
{
    if (filterMAVLINK != filter) {
        filterMAVLINK = filter;
        this->bytesToIgnore = 0;
        if (m_ui->mavlinkCheckBox->isChecked() != filter) {
            m_ui->mavlinkCheckBox->setChecked(filter);
        }
    }
}
/**
 * @param hold Freeze the input and thus any scrolling
 */
void DebugConsole::hold(bool hold)
{
    if (holdOn != hold) {
        // Check if we need to append bytes from the hold buffer
        if (this->holdOn && !hold) {
            // TODO No conversion is done to the bytes in the hold buffer
            m_ui->receiveText->appendPlainText(QString(holdBuffer));
            holdBuffer.clear();
            lowpassDataRate = 0.0f;
        }

        this->holdOn = hold;

        // Change text interaction mode
        if (hold) {
            m_ui->receiveText->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse | Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
        } else {
            m_ui->receiveText->setTextInteractionFlags(Qt::NoTextInteraction);
        }
        if (m_ui->holdCheckBox->isChecked() != hold) {
            m_ui->holdCheckBox->setChecked(hold);
        }
    }
}

/** @brief Handle the connect button */
void DebugConsole::handleConnectButton()
{
    if (currLink) {
        if (currLink->isConnected()) {
            m_ui->connectButton->setChecked(true);  // this will be set once actually connected
            currLink->disconnect();
        } else {
            m_ui->connectButton->setChecked(false);  // this will be set once actually connected
            currLink->connect();
        }
    } else
        MainWindow::instance()->addLink();
}

void DebugConsole::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Up) {
        cycleCommandHistory(true);
    } else if (event->key() == Qt::Key_Down) {
        cycleCommandHistory(false);
    } else {
        QWidget::keyPressEvent(event);
    }
}

void DebugConsole::cycleCommandHistory(bool up)
{
    // Only cycle if there is a history
    if (commandHistory.length() > 0) {
        // Store current command if we're not in history yet
        if (commandIndex == commandHistory.length() && up) {
            currCommand = m_ui->sendText->text();
        }

        if (up) {
            // UP
            commandIndex--;
            if (commandIndex >= 0) {
                m_ui->sendText->setText(commandHistory.at(commandIndex));
            }

            // If the index
        } else {
            // DOWN
            commandIndex++;
            if (commandIndex < commandHistory.length()) {
                m_ui->sendText->setText(commandHistory.at(commandIndex));
            }
            // If the index is at history length, load the last current command

        }

        // Restore current command if we went out of history
        if (commandIndex == commandHistory.length()) {
            m_ui->sendText->setText(currCommand);
        }

        // If we are too far down or too far up, wrap around to current command
        if (commandIndex < 0 || commandIndex > commandHistory.length()) {
            commandIndex = commandHistory.length();
            m_ui->sendText->setText(currCommand);
        }

        // Bound the index
        if (commandIndex < 0) commandIndex = 0;
        if (commandIndex > commandHistory.length()) commandIndex = commandHistory.length();
    }
}

void DebugConsole::on_checkBox_simpleView_toggled(bool checked)
{
    m_ui->addSymbolButton->setVisible(!checked);
    m_ui->hexCheckBox->setVisible(!checked);
    m_ui->holdButton->setVisible(!checked);
    m_ui->holdCheckBox->setVisible(!checked);
    m_ui->mavlinkCheckBox->setVisible(!checked);
    m_ui->sendText->setVisible(!checked);
    m_ui->specialCheckBox->setVisible(!checked);
    m_ui->specialComboBox->setVisible(!checked);
    m_ui->transmitButton->setVisible(!checked);
    if (!checked)
        m_ui->sentText->setVisible(false); // this would be re-shown if needed automatically
}

void DebugConsole::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
