#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QSizePolicy>
#include <QScrollBar>
#include <QLabel>
#include <QMenu>
#include <QSpinBox>
#include <QColor>
#include <QPalette>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

#include "aqlinechartwidget.h"
#include "mainwindow.h"
#include "qgc.h"
#include "mg.h"

AQLinechartWidget::AQLinechartWidget(int systemid, QWidget *parent) : QWidget(parent),
    sysid(systemid),
    activePlot(NULL),
    curvesLock(new QReadWriteLock()),
    plotWindowLock(),
    curveListIndex(0),
    curveListCounter(0),
    listedCurves(new QList<QString>()),
    curveLabels(new QMap<QString, QLabel*>()),
    curveMenu(new QMenu(this)),
    updateTimer(new QTimer()),
    selectedMAV(-1)
{
    ui.setupUi(this);
    this->setMinimumSize(500, 210);

    maxValue = 5;
    minValue = -5;
    ui.maxValue->setText(QString("5"));
    ui.minValue->setText(QString("-5"));

    // Add and customize curve list elements (left side)
    curvesWidget = new QWidget(ui.curveListWidget);
    ui.curveListWidget->setWidget(curvesWidget);
    curvesWidgetLayout = new QGridLayout(curvesWidget);
    curvesWidgetLayout->setMargin(0);
    curvesWidgetLayout->setSpacing(2);
    curvesWidgetLayout->setAlignment(Qt::AlignTop);

    curvesWidgetLayout->setColumnStretch(0, 0);
    curvesWidgetLayout->setColumnStretch(1, 80);
    curvesWidgetLayout->setColumnStretch(2, 80);
    curvesWidgetLayout->setColumnStretch(3, 0);
    curvesWidgetLayout->setColumnStretch(4, 0);
    curvesWidgetLayout->setColumnStretch(5, 0);
    curvesWidgetLayout->setColumnStretch(6, 0);

    curvesWidget->setLayout(curvesWidgetLayout);
    ListItems = new QList<QString>;

    // Create the layout of chart
    createLayout();

    connect(updateTimer, SIGNAL(timeout()), this, SLOT(refresh()));

    updateTimer->setInterval(UPDATE_INTERVAL);
    readSettings();
    connect(ui.maxValue, SIGNAL(textChanged(QString)), this, SLOT(checkMaxMin(QString)));
}

AQLinechartWidget::~AQLinechartWidget()
{
    writeSettings();
    if (activePlot) delete activePlot;
    activePlot = NULL;
    delete listedCurves;
    listedCurves = NULL;
}

void AQLinechartWidget::selectActiveSystem(int mav)
{
    // -1: Unitialized, 0: all
    if (mav != selectedMAV && (selectedMAV != -1))
    {
        // Delete all curves
        // FIXME
    }
    selectedMAV = mav;
}

void AQLinechartWidget::writeSettings()
{
    QSettings settings;
    settings.beginGroup("AQ_TELEMETRY_VIEW");
    settings.setValue("SPLITTER_SIZES", ui.splitter->saveState());
    settings.endGroup();
    settings.sync();
}

void AQLinechartWidget::readSettings()
{
    QSettings settings;
    settings.sync();
    settings.beginGroup("AQ_TELEMETRY_VIEW");
    if (settings.contains("SPLITTER_SIZES"))
        ui.splitter->restoreState(settings.value("SPLITTER_SIZES").toByteArray());
    settings.endGroup();
}

// create chart
void AQLinechartWidget::createLayout()
{
    int colIdx = 0;

    // Setup the plot group box area layout
    QGridLayout* layout = new QGridLayout(ui.AQdiagramGroupBox);
    mainLayout = layout;
    layout->setHorizontalSpacing(8);
    layout->setMargin(2);

    // Create plot container widget
    activePlot = new LinechartPlot(this, sysid);
    // Activate automatic scrolling
    activePlot->setAutoScroll(true);

    layout->addWidget(activePlot, 0, 0, 1, 8);
    layout->setRowStretch(0, 10);
    layout->setRowStretch(1, 1);

    activePlot->enforceGroundTime(true);

    // spacer
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding), 1, colIdx);
    layout->setColumnStretch(colIdx++, 1);

    ui.AQdiagramGroupBox->setLayout(layout);

    // Connect notifications from the user interface to the plot
    connect(this, SIGNAL(curveRemoved(QString)), activePlot, SLOT(hideCurve(QString)));
    connect(activePlot, SIGNAL(curveRemoved(QString)), this, SLOT(removeCurve(QString)));

    // Update plot when scrollbar is moved (via translator method setPlotWindowPosition()
    connect(this, SIGNAL(plotWindowPositionUpdated(quint64)), activePlot, SLOT(setWindowPosition(quint64)));
}

void AQLinechartWidget::appendData(int uasId, const QString& curve, const QString& unit, QVariant &variant,
                                   quint64 usec)
{
    QMetaType::Type type = static_cast<QMetaType::Type>(variant.type());
    bool ok;
    double value = variant.toDouble(&ok);
    if(!ok || type == QMetaType::QByteArray || type == QMetaType::QString)
        return;
    bool isDouble = type == QMetaType::Float || type == QMetaType::Double;

    if ((selectedMAV == -1 ) || (selectedMAV == uasId ))
    {
        // Order matters here, first append to plot, then update curve list
        activePlot->appendData(curve+unit, usec, value);

        // Store data
        QLabel* label = curveLabels->value(curve+unit, NULL);
        // Make sure the curve will be created if it does not yet exist
        if(!label)
        {
            if (!isDouble)
                intData.insert(curve+unit, 0);
            addCurve(curve, unit);
        }
        // Add int data
        if (!isDouble)
            intData.insert(curve+unit, value);
    }
}

/**
 * Add a curve to the curve list
 * @param curve The id-string of the curve
 **/
void AQLinechartWidget::addCurve(const QString& curve, const QString& unit)
{
    QLabel* label;
    QLabel* value;

    curveNames.insert(curve+unit, curve);
    curveUnits.insert(curve, unit);
    ListItems->append(curve);

    int labelRow = curvesWidgetLayout->rowCount();

    label = new QLabel(this);
    label->setText(curve);
    label->setStyleSheet(QString("QLabel {font-family:\"Arial\";font-size:12px;}"));
    curvesWidgetLayout->addWidget(label, labelRow, 1);

    // Label
    curveNameLabels.insert(curve+unit, label);

    // Value
    value = new QLabel(this);
    value->setNum(0.00);
    value->setStyleSheet(QString("QLabel {font-family:\"Arial\";font-size:12px;}"));
    curveLabels->insert(curve+unit, value);
    curvesWidgetLayout->addWidget(value, labelRow, 2);
}

void AQLinechartWidget::refresh()
{
    setUpdatesEnabled(false);
    QString str;

    QMap<QString, QLabel*>::iterator i;
    for (i = curveLabels->begin(); i != curveLabels->end(); ++i) {
        if (intData.contains(i.key())) {
            str.sprintf("% 11i", intData.value(i.key()));
        } else {
            double val = activePlot->getCurrentValue(i.key());
            int intval = static_cast<int>(val);
            if (intval >= 100000 || intval <= -100000) {
                str.sprintf("% 11i", intval);
            } else if (intval >= 10000 || intval <= -10000) {
                str.sprintf("% 11.2f", val);
            } else if (intval >= 1000 || intval <= -1000) {
                str.sprintf("% 11.4f", val);
            } else {
                str.sprintf("% 11.6f", val);
            }
        }
        // Value
        i.value()->setText(str);
    }
    setUpdatesEnabled(true);
}

/**
 * Defines the width of the sliding average filter and the width of the sliding median filter.
 * @param windowSize with (in values) of the sliding average/median filter. Minimum is 2
 */
void AQLinechartWidget::setAverageWindow(int windowSize)
{
    if (windowSize > 1) activePlot->setAverageWindow(windowSize);
}

/**
 * Remove the curve from the curve list.
 **/
void AQLinechartWidget::removeCurve(QString curve)
{
    QWidget* widget = NULL;

    QString unit = curveUnits.take(curve);

    widget = curveLabels->take(curve);
    if (widget) {
        curvesWidgetLayout->removeWidget(widget);
        delete widget;
    }

    widget = curveNameLabels.take(curve+unit);
    if (widget) {
        curvesWidgetLayout->removeWidget(widget);
        delete widget;
    }

    widget = colorIcons.take(curve+unit);
    if (widget) {
        curvesWidgetLayout->removeWidget(widget);
        delete widget;
    }

    widget = checkBoxes.take(curve+unit);
    if (widget) {
        disconnect(selectAllCheckBox, SIGNAL(clicked(bool)), widget, SLOT(setChecked(bool)));
        QObject::disconnect(widget, SIGNAL(clicked(bool)), this, SLOT(takeButtonClick(bool)));
        curvesWidgetLayout->removeWidget(widget);
        delete widget;
    }

    intData.remove(curve+unit);
}

void AQLinechartWidget::clearCurves()
{
    for (int i = 0; i < ListItems->size(); i++)
        removeCurve(ListItems->at(i));
}

QString AQLinechartWidget::getCurveName(const QString& key, bool shortEnabled)
{
    Q_UNUSED(shortEnabled);
    return curveNames.value(key);
}

void AQLinechartWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    setActive(true);
}

void AQLinechartWidget::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    setActive(false);
}

void AQLinechartWidget::setActive(bool active)
{
    if (activePlot) {
        activePlot->setActive(active);
    }
    if (active) {
        updateTimer->start(UPDATE_INTERVAL);
    } else {
        updateTimer->stop();
    }
}

/**
 *  Set the position of the plot window. The plot covers only a portion of the complete time series.
 * The scrollbar allows to select a window of the time series.
 * The right edge of the window is defined proportional to the position of the scrollbar.
 **/
void AQLinechartWidget::setPlotWindowPosition(int scrollBarValue)
{
    plotWindowLock.lockForWrite();
    // Disable automatic scrolling immediately
    int scrollBarRange = (MAX_TIME_SCROLLBAR_VALUE - MIN_TIME_SCROLLBAR_VALUE);
    double position = (static_cast<double>(scrollBarValue) - MIN_TIME_SCROLLBAR_VALUE) / scrollBarRange;
    quint64 scrollInterval;

    // Activate automatic scrolling if scrollbar is at the right edge
    if(scrollBarValue > MAX_TIME_SCROLLBAR_VALUE - (MAX_TIME_SCROLLBAR_VALUE - MIN_TIME_SCROLLBAR_VALUE) * 0.01f) {
        activePlot->setAutoScroll(true);
    } else {
        activePlot->setAutoScroll(false);
        quint64 rightPosition;
        /* If the data exceeds the plot window, choose the position according to the scrollbar position */
        if(activePlot->getDataInterval() > activePlot->getPlotInterval()) {
            scrollInterval = activePlot->getDataInterval() - activePlot->getPlotInterval();
            rightPosition = activePlot->getMinTime() + activePlot->getPlotInterval() + (scrollInterval * position);
        } else {
            /* If the data interval is smaller as the plot interval, clamp the scrollbar to the right */
            rightPosition = activePlot->getMinTime() + activePlot->getPlotInterval();
        }
        emit plotWindowPositionUpdated(rightPosition);
    }
    plotWindowLock.unlock();
}

/**
 *  Receive an updated plot window position.
 * The plot window can be changed by the arrival of new data or by other user interaction.
 * The scrollbar and other UI components can be notified by calling this method.
 *
 * @param position The absolute position of the right edge of the plot window, in milliseconds
 **/
void AQLinechartWidget::setPlotWindowPosition(quint64 position)
{
    plotWindowLock.lockForWrite();
    // Calculate the relative position
    double pos;
    // A relative position makes only sense if the plot is filled
    if(activePlot->getDataInterval() > activePlot->getPlotInterval()) {
        //TODO @todo Implement the scrollbar enabling in a more elegant way
        quint64 scrollInterval = position - activePlot->getMinTime() - activePlot->getPlotInterval();
        pos = (static_cast<double>(scrollInterval) / (activePlot->getDataInterval() - activePlot->getPlotInterval()));
    } else {
        pos = 1;
    }
    plotWindowLock.unlock();

    emit plotWindowPositionUpdated(static_cast<int>(pos * (MAX_TIME_SCROLLBAR_VALUE - MIN_TIME_SCROLLBAR_VALUE)));
}

/**
 * Set the time interval the plot displays. The time interval of the plot can be adjusted by this method.
 * If the data covers less time than the interval, the plot will be filled from the right to left.
 *
 * @param interval The time interval to plot
 **/
void AQLinechartWidget::setPlotInterval(int interval)
{
    if (interval < 1000) // convert to ms
        interval *= 1000;
    activePlot->setPlotInterval(static_cast<quint64>(interval));
}

/**
 * Set the curve be visible or not
 **/
void AQLinechartWidget::setCurveVisible(QString curve, bool visible){
    activePlot->setVisibleById(curve, visible);
}

/**
 *  Take the click of a curve activation / deactivation button.
 * This method allows to map a button to a plot curve.
 * The text of the button must equal the curve name to activate / deactivate.
 *
 * @param checked The visibility of the curve: true to display the curve, false otherwise
 **/
void AQLinechartWidget::takeButtonClick(bool checked)
{

    QCheckBox* button = qobject_cast<QCheckBox*>(QObject::sender());

    if(button != NULL)
    {
        activePlot->setVisibleById(button->objectName(), checked);

        QWidget* colorIcon = colorIcons.value(button->objectName(), 0);
        if (colorIcon) {
            QColor color = activePlot->getColorForCurve(button->objectName());
            if (color.isValid()) {
                QString colorstyle;
                colorstyle = colorstyle.sprintf("QWidget { background-color: #%X%X%X; }", color.red(), color.green(), color.blue());
                colorIcon->setStyleSheet(colorstyle);
                colorIcon->setAutoFillBackground(true);
            }
        }
    }
}

/**
 *  Factory method to create a new button.
 *
 * @param imagename The name of the image (should be placed at the standard icon location)
 * @param text The button text
 * @param parent The parent object (to ensure that the memory is freed after the deletion of the button)
 **/
QToolButton* AQLinechartWidget::createButton(QWidget* parent)
{
    QToolButton* button = new QToolButton(parent);
    button->setMinimumSize(QSize(20, 20));
    button->setMaximumSize(60, 20);
    button->setGeometry(button->x(), button->y(), 20, 20);
    return button;
}

void AQLinechartWidget::checkMaxMin(QString str){
    Q_UNUSED(str);
    if (ui.maxValue->text().toDouble() != maxValue || ui.minValue->text().toDouble() != minValue){
        maxValue = ui.maxValue->text().toDouble();
        minValue = ui.minValue->text().toDouble();
        activePlot->changeMaxMin(maxValue, minValue);
    }
}
