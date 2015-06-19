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

AQLinechartWidget::AQLinechartWidget(int systemid, QWidget *parent) : QWidget (parent),
  sysid(systemid),
  activePlot(NULL),
  curvesLock(new QReadWriteLock()),
  plotWindowLock(),
  curveListIndex(0),
  curveListCounter(0),
  listedCurves(new QList<QString>()),
  curveLabels(new QMap<QString, QLabel*>()),
  curveMeans(new QMap<QString, QLabel*>()),
  curveMedians(new QMap<QString, QLabel*>()),
  curveVariances(new QMap<QString, QLabel*>()),
  curveMenu(new QMenu(this)),
  logFile(new QFile()),
  logindex(1),
  logging(false),
  logStartTime(0),
  updateTimer(new QTimer()),
  selectedMAV(-1)
{
    ui.setupUi(this);
    this->setMinimumSize(200, 150);

    // Add and customize curve list elements (left side)
    curvesWidget = new QWidget(ui.curveListWidget);
    ui.curveListWidget->setWidget(curvesWidget);
    curvesWidgetLayout = new QGridLayout(curvesWidget);
    curvesWidgetLayout->setMargin(2);
    curvesWidgetLayout->setSpacing(4);
    //curvesWidgetLayout->setSizeConstraint(QSizePolicy::Expanding);
    curvesWidgetLayout->setAlignment(Qt::AlignTop);

    curvesWidgetLayout->setColumnStretch(0, 0);
    curvesWidgetLayout->setColumnStretch(1, 0);
    curvesWidgetLayout->setColumnStretch(2, 80);
    curvesWidgetLayout->setColumnStretch(3, 0);
    curvesWidgetLayout->setColumnStretch(4, 0);
    curvesWidgetLayout->setColumnStretch(5, 0);
//    horizontalLayout->setColumnStretch(median, 50);
    curvesWidgetLayout->setColumnStretch(6, 0);

    curvesWidget->setLayout(curvesWidgetLayout);
    ListItems = new QList<QString>;
    // Create curve list headings
    QLabel* label;
    QLabel* value;
    QLabel* mean;
    QLabel* variance;

//    connect(ui.recolorButton, SIGNAL(clicked()), this, SLOT(recolor()));

    int labelRow = curvesWidgetLayout->rowCount();

//    selectAllCheckBox = new QCheckBox("", this);
//    connect(selectAllCheckBox, SIGNAL(clicked(bool)), this, SLOT(selectAllCurves(bool)));
//    curvesWidgetLayout->addWidget(selectAllCheckBox, labelRow, 0, 1, 2);

    label = new QLabel(this);
    label->setText("Name");
    curvesWidgetLayout->addWidget(label, labelRow, 2);

    // Value
    value = new QLabel(this);
    value->setText("Val");
    curvesWidgetLayout->addWidget(value, labelRow, 3);

    // Unit
    //curvesWidgetLayout->addWidget(new QLabel(tr("Unit")), labelRow, 4);

    // Mean
    mean = new QLabel(this);
    mean->setText("Mean");
    curvesWidgetLayout->addWidget(mean, labelRow, 4);

    // Variance
    variance = new QLabel(this);
    variance->setText("Variance");
    curvesWidgetLayout->addWidget(variance, labelRow, 5);

    // Create the layout
    createLayout();

//    connect(MainWindow::instance(), SIGNAL(styleChanged(int)), this, SLOT(recolor()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(refresh()));
//    connect(ui.uasSelectionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectActiveSystem(int)));

    updateTimer->setInterval(UPDATE_INTERVAL);
    readSettings();
}

AQLinechartWidget::~AQLinechartWidget()
{
//    stopLogging();
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

void AQLinechartWidget::readSettings()
{
    QSettings settings;
    settings.sync();
    settings.beginGroup("AQ_TELEMETRY_VIEW");
    if (settings.contains("SPLITTER_SIZES"))
        ui.splitter->restoreState(settings.value("SPLITTER_SIZES").toByteArray());
//  qDebug() << "resized to" << ui.splitter->sizes();
//    if (averageSpinBox) averageSpinBox->setValue(settings.value("AVG_WINDOW", DEFAULT_AVG_WINDOW).toInt());
    //if (intervalSpinBox) intervalSpinBox->setValue(settings.value("PLOT_INTERVAL", DEFAULT_PLOT_INTERVAL).toInt());
    settings.endGroup();
}

void AQLinechartWidget::createLayout()
{
    int colIdx = 0;

    // Create actions
//    createActions();

    // Setup the plot group box area layout
    QGridLayout* layout = new QGridLayout(ui.AQdiagramGroupBox);
    mainLayout = layout;
    layout->setHorizontalSpacing(8);
    layout->setMargin(2);

    // Create plot container widget
    activePlot = new LinechartPlot(this, sysid);
    // Activate automatic scrolling
    activePlot->setAutoScroll(true);

    // TODO Proper Initialization needed
    //    activePlot = getPlot(0);
    //    plotContainer->setPlot(activePlot);

    layout->addWidget(activePlot, 0, 0, 1, 8);
    layout->setRowStretch(0, 10);
    layout->setRowStretch(1, 1);


    // Linear scaling button
//    scalingLinearButton = createButton(this);
//    scalingLinearButton->setDefaultAction(setScalingLinear);
//    scalingLinearButton->setCheckable(true);
//    scalingLinearButton->setChecked(true);
//    scalingLinearButton->setToolTip(tr("Set linear scale for Y axis"));
//    scalingLinearButton->setWhatsThis(tr("Set linear scale for Y axis"));
//    layout->addWidget(scalingLinearButton, 1, colIdx++);
//    layout->setColumnStretch(0, 0);

    // Logarithmic scaling button
//    scalingLogButton = createButton(this);
//    scalingLogButton->setDefaultAction(setScalingLogarithmic);
//    scalingLogButton->setCheckable(true);
//    scalingLogButton->setChecked(false);
//    scalingLogButton->setToolTip(tr("Set logarithmic scale for Y axis"));
//    scalingLogButton->setWhatsThis(tr("Set logarithmic scale for Y axis"));
//    layout->addWidget(scalingLogButton, 1, colIdx++);
//    layout->setColumnStretch(1, 0);

    // Averaging spin box
//    averageSpinBox = new QSpinBox(this);
//    averageSpinBox->setToolTip(tr("Number of samples used to calculate mean and variance"));
//    averageSpinBox->setWhatsThis(tr("Number of samples used to calculate mean and variance"));
//    averageSpinBox->setRange(2, 9999);
//    averageSpinBox->setSingleStep(10);
//    averageSpinBox->setValue(DEFAULT_AVG_WINDOW);
//    averageSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    setAverageWindow(DEFAULT_AVG_WINDOW);
//    layout->addWidget(new QLabel(tr("Avg. Window:")), 1, colIdx++);
//    layout->addWidget(averageSpinBox, 1, colIdx++);
//    layout->setColumnStretch(2, 0);

    // Time interval spin box
//    intervalSpinBox = new QSpinBox(this);
//    intervalSpinBox->setToolTip(tr("Time span of plot window, in seconds"));
//    intervalSpinBox->setWhatsThis(tr("Time span of plot window, in seconds"));
//    intervalSpinBox->setRange(2, 999);
//    intervalSpinBox->setSuffix("s");
//    intervalSpinBox->setValue(DEFAULT_PLOT_INTERVAL);
//    intervalSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    setPlotInterval(DEFAULT_PLOT_INTERVAL);
//    layout->addWidget(new QLabel(tr("Time Span:")), 1, colIdx++);
//    layout->addWidget(intervalSpinBox, 1, colIdx++);
//    layout->setColumnStretch(2, 0);

    // Ground time button
    /*
    timeButton = new QCheckBox(this);
    timeButton->setText(tr("Ground Time"));
    timeButton->setToolTip(tr("Overwrite timestamp of data from vehicle with ground receive time. Helps if the plots are not visible because of missing or invalid onboard time."));
    timeButton->setWhatsThis(tr("Overwrite timestamp of data from vehicle with ground receive time. Helps if the plots are not visible because of missing or invalid onboard time."));
    layout->addWidget(timeButton, 1, colIdx++);
    connect(timeButton, SIGNAL(clicked(bool)), activePlot, SLOT(enforceGroundTime(bool)));
    connect(timeButton, SIGNAL(clicked()), this, SLOT(writeSettings()));
    */
    activePlot->enforceGroundTime(true);

    // spacer
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding), 1, colIdx);
    layout->setColumnStretch(colIdx++, 1);

    // Log Button
//    logButton = new QToolButton(this);
//    logButton->setToolTip(tr("Start to log curve data into a CSV or TXT file"));
//    logButton->setWhatsThis(tr("Start to log curve data into a CSV or TXT file"));
//    logButton->setText(tr("Start Logging"));
//    layout->addWidget(logButton, 1, colIdx++);
//    layout->setColumnStretch(3, 0);


    ui.AQdiagramGroupBox->setLayout(layout);

    // Add actions

    // Connect notifications from the user interface to the plot
    connect(this, SIGNAL(curveRemoved(QString)), activePlot, SLOT(hideCurve(QString)));
    connect(MainWindow::instance(), SIGNAL(styleChanged(int)), activePlot, SLOT(styleChanged(int)));

    // Update scrollbar when plot window changes (via translator method setPlotWindowPosition()
//    connect(activePlot, SIGNAL(windowPositionChanged(quint64)), this, SLOT(setPlotWindowPosition(quint64)));

    connect(activePlot, SIGNAL(curveRemoved(QString)), this, SLOT(removeCurve(QString)));

    // Update plot when scrollbar is moved (via translator method setPlotWindowPosition()
    connect(this, SIGNAL(plotWindowPositionUpdated(quint64)), activePlot, SLOT(setWindowPosition(quint64)));

    // Set scaling
//    connect(scalingLinearButton, SIGNAL(clicked()), this, SLOT(setLinearScaling()));
//    connect(scalingLogButton, SIGNAL(clicked()), this, SLOT(setLogarithmicScaling()));

    // set average window
//    connect(averageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setAverageWindow(int)));
    // set plot interval
//    connect(intervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setPlotInterval(int)));
    // start/stop logging
//    connect(logButton, SIGNAL(clicked()), this, SLOT(startLogging()));

}

void AQLinechartWidget::appendData(int uasId, const QString& curve, const QString& unit, QVariant &variant, quint64 usec)
{
    QMetaType::Type type = static_cast<QMetaType::Type>(variant.type());
    bool ok;
    double value = variant.toDouble(&ok);
    if(!ok || type == QMetaType::QByteArray || type == QMetaType::QString)
        return;
    bool isDouble = type == QMetaType::Float || type == QMetaType::Double;

    // if ((selectedMAV == -1 && isVisible()) || (selectedMAV == uasId && isVisible()))
    if ((selectedMAV == -1 ) || (selectedMAV == uasId ))
    {
        // Order matters here, first append to plot, then update curve list
        activePlot->appendData(curve+unit, usec, value);
        // Store data
        QLabel* label = curveLabels->value(curve+unit, NULL);
        // Make sure the curve will be created if it does not yet exist
        if(!label)
        {
            //qDebug() << "ADDING CURVE" << curve << "IN APPENDDATA DOUBLE";
            if (!isDouble)
                intData.insert(curve+unit, 0);
            addCurve(curve, unit);
        }
        // Add int data
        if (!isDouble)
            intData.insert(curve+unit, value);
    }

    // Log data
    if (logging)
    {
        if (activePlot->isVisible(curve+unit))
        {
            if (usec == 0) usec = QGC::groundTimeMilliseconds();
            if (logStartTime == 0) logStartTime = usec;
            qint64 time = usec - logStartTime;
            if (time < 0) time = 0;

            logFile->write(QString(QString::number(time) + "\t" + QString::number(uasId) + "\t" + curve + "\t" + QString::number(value) + "\n").toLatin1());
            logFile->flush();
        }
    }
}

void AQLinechartWidget::refresh()
{
    setUpdatesEnabled(false);
    QString str;
    // Value

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
    // Mean
    QMap<QString, QLabel*>::iterator j;
    for (j = curveMeans->begin(); j != curveMeans->end(); ++j) {
        double val = activePlot->getMean(j.key());
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
        j.value()->setText(str);
    }


//    QMap<QString, QLabel*>::iterator k;
//    for (k = curveMedians->begin(); k != curveMedians->end(); ++k)
//    {
//        // Median
//        str.sprintf("%+.2f", activePlot->getMedian(k.key()));
//        k.value()->setText(str);
//    }

    QMap<QString, QLabel*>::iterator l;
    for (l = curveVariances->begin(); l != curveVariances->end(); ++l) {
        // Variance
        str.sprintf("% 8.3e", activePlot->getVariance(l.key()));
        l.value()->setText(str);
    }

    setUpdatesEnabled(true);
}

void AQLinechartWidget::addCurve(const QString& curve, const QString& unit)
{
    LinechartPlot* plot = activePlot;
    QCheckBox *checkBox;
    QLabel* label;
    QLabel* value;
//    QLabel* unitLabel;
    QLabel* mean;
    QLabel* variance;

    curveNames.insert(curve+unit, curve);
    curveUnits.insert(curve, unit);
    ListItems->append(curve);

    int labelRow = curvesWidgetLayout->rowCount();

    checkBox = new QCheckBox(this);
    checkBoxes.insert(curve+unit, checkBox);
    checkBox->setCheckable(true);
    checkBox->setObjectName(curve+unit);
    checkBox->setToolTip(tr("Enable the curve in the graph window"));
    checkBox->setWhatsThis(tr("Enable the curve in the graph window"));

    curvesWidgetLayout->addWidget(checkBox, labelRow, 0);

    QWidget* colorIcon = new QWidget(this);
    colorIcons.insert(curve+unit, colorIcon);
    colorIcon->setMinimumSize(5, 14);
    colorIcon->setMaximumSize(5, 14);

    curvesWidgetLayout->addWidget(colorIcon, labelRow, 1);

    label = new QLabel(this);
    label->setText(curve);
    curvesWidgetLayout->addWidget(label, labelRow, 2);

    QColor color(Qt::gray);// = plot->getColorForCurve(curve+unit);
    QString colorstyle;
    colorstyle = colorstyle.sprintf("QWidget { background-color: #%X%X%X; }", color.red(), color.green(), color.blue());
    colorIcon->setStyleSheet(colorstyle);
    colorIcon->setAutoFillBackground(true);

    // Label
    curveNameLabels.insert(curve+unit, label);
//    curveLabels->insert(curve+unit, value);

    // Value
    value = new QLabel(this);
    value->setNum(0.00);
    value->setStyleSheet(QString("QLabel {font-family:\"Courier\"; font-weight: bold;}"));
    value->setToolTip(tr("Current value of %1 in %2 units").arg(curve, unit));
    value->setWhatsThis(tr("Current value of %1 in %2 units").arg(curve, unit));
    curveLabels->insert(curve+unit, value);
    curvesWidgetLayout->addWidget(value, labelRow, 3);

    /*
    // Unit
    unitLabel = new QLabel(this);
    unitLabel->setText(unit);
    unitLabel->setStyleSheet(QString("QLabel {color: %1;}").arg("#AAAAAA"));
    //qDebug() << "UNIT" << unit;
    unitLabel->setToolTip(tr("Unit of ") + curve);
    unitLabel->setWhatsThis(tr("Unit of ") + curve);
    curvesWidgetLayout->addWidget(unitLabel, labelRow, 4);
    */

    // Mean
    mean = new QLabel(this);
    mean->setNum(0.00);
    mean->setStyleSheet(QString("QLabel {font-family:\"Courier\"; font-weight: bold;}"));
    mean->setToolTip(tr("Arithmetic mean of %1 in %2 units").arg(curve, unit));
    mean->setWhatsThis(tr("Arithmetic mean of %1 in %2 units").arg(curve, unit));
    curveMeans->insert(curve+unit, mean);
    curvesWidgetLayout->addWidget(mean, labelRow, 4);

//    // Median
//    median = new QLabel(form);
//    value->setNum(0.00);
//    curveMedians->insert(curve, median);
//    horizontalLayout->addWidget(median);

    // Variance
    variance = new QLabel(this);
    variance->setNum(0.00);
    variance->setStyleSheet(QString("QLabel {font-family:\"Courier\"; font-weight: bold;}"));
    variance->setToolTip(tr("Variance of %1 in (%2)^2 units").arg(curve, unit));
    variance->setWhatsThis(tr("Variance of %1 in (%2)^2 units").arg(curve, unit));
    curveVariances->insert(curve+unit, variance);
    curvesWidgetLayout->addWidget(variance, labelRow, 5);

    /* Color picker
    QColor color = QColorDialog::getColor(Qt::green, this);
         if (color.isValid()) {
             colorLabel->setText(color.name());
             colorLabel->setPalette(QPalette(color));
             colorLabel->setAutoFillBackground(true);
         }
        */

    // Set stretch factors so that the label gets the whole space


    // Load visibility settings
    // TODO

    // Connect actions
//    connect(selectAllCheckBox, SIGNAL(clicked(bool)), checkBox, SLOT(setChecked(bool)));
    QObject::connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(takeButtonClick(bool)));
    QObject::connect(this, SIGNAL(curveVisible(QString, bool)), plot, SLOT(setVisibleById(QString, bool)));

    // Set UI components to initial state
    checkBox->setChecked(false);
    plot->setVisibleById(curve+unit, false);
}

/**
 * @brief Remove the curve from the curve list.
 *
 * @param curve The curve to remove
 * @see addCurve()
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

    widget = curveMeans->take(curve+unit);
    if (widget) {
        curvesWidgetLayout->removeWidget(widget);
        widget->deleteLater();
    }

//    widget = curveMedians->take(curve);
//    curvesWidgetLayout->removeWidget(widget);
//    widget->deleteLater();

    widget = curveVariances->take(curve+unit);
    if (widget) {
        curvesWidgetLayout->removeWidget(widget);
        widget->deleteLater();
    }

    intData.remove(curve+unit);
}

void AQLinechartWidget::clearCurves()
{
    for (int i = 0; i < ListItems->size(); i++)
        removeCurve(ListItems->at(i));
}

void AQLinechartWidget::recolor()
{
    activePlot->styleChanged(MainWindow::instance()->getStyle());
    foreach (QString key, colorIcons.keys())
    {
        QWidget* colorIcon = colorIcons.value(key, 0);
        if (colorIcon) {
            QString colorstyle;
            QColor color = activePlot->getColorForCurve(key);
            colorstyle = colorstyle.sprintf("QWidget { background-color: #%X%X%X; }", color.red(), color.green(), color.blue());
            colorIcon->setStyleSheet(colorstyle);
            colorIcon->setAutoFillBackground(true);
        }
    }
}

QString AQLinechartWidget::getCurveName(const QString& key, bool shortEnabled)
{
    Q_UNUSED(shortEnabled);
    return curveNames.value(key);
}

void AQLinechartWidget::setShortNames(bool enable)
{
    foreach (QString key, curveNames.keys())
    {
        curveNameLabels.value(key)->setText(getCurveName(key, enable));
    }
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
 * @brief Set the position of the plot window.
 * The plot covers only a portion of the complete time series. The scrollbar
 * allows to select a window of the time series. The right edge of the window is
 * defined proportional to the position of the scrollbar.
 *
 * @param scrollBarValue The value of the scrollbar, in the range from MIN_TIME_SCROLLBAR_VALUE to MAX_TIME_SCROLLBAR_VALUE
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


    // The slider position must be mapped onto an interval of datainterval - plotinterval,
    // because the slider position defines the right edge of the plot window. The leftmost
    // slider position must therefore map to the start of the data interval + plot interval
    // to ensure that the plot is not empty

    //  start> |-- plot interval --||-- (data interval - plotinterval) --| <end

    //@TODO Add notification of scrollbar here
    //plot->setWindowPosition(rightPosition);

    plotWindowLock.unlock();
}

/**
 * @brief Receive an updated plot window position.
 * The plot window can be changed by the arrival of new data or by
 * other user interaction. The scrollbar and other UI components
 * can be notified by calling this method.
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
        //scrollbar->setDisabled(false);
        quint64 scrollInterval = position - activePlot->getMinTime() - activePlot->getPlotInterval();



        pos = (static_cast<double>(scrollInterval) / (activePlot->getDataInterval() - activePlot->getPlotInterval()));
    } else {
        //scrollbar->setDisabled(true);
        pos = 1;
    }
    plotWindowLock.unlock();

    emit plotWindowPositionUpdated(static_cast<int>(pos * (MAX_TIME_SCROLLBAR_VALUE - MIN_TIME_SCROLLBAR_VALUE)));
}

/**
 * @brief Set the time interval the plot displays.
 * The time interval of the plot can be adjusted by this method. If the
 * data covers less time than the interval, the plot will be filled from
 * the right to left
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
 * @brief Take the click of a curve activation / deactivation button.
 * This method allows to map a button to a plot curve.The text of the
 * button must equal the curve name to activate / deactivate.
 *
 * @param checked The visibility of the curve: true to display the curve, false otherwise
 **/
void AQLinechartWidget::takeButtonClick(bool checked)
{

    QCheckBox* button = qobject_cast<QCheckBox*>(QObject::sender());

    if(button != NULL)
    {
        activePlot->setVisibleById(button->objectName(), checked);


//      CurveIsActive[]
//      qDebug() << button->objectName();
//      int index = ListItems->indexOf(button->objectName());
//      CurveIsActive[index] = checked;

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

///**
// * @brief Set logarithmic scaling for the curve
// **/
//void AQLinechartWidget::setLogarithmicScaling()
//{
//    activePlot->setLogarithmicScaling();
//    scalingLogButton->setChecked(true);
//    scalingLinearButton->setChecked(false);
//}

///**
// * @brief Set linear scaling for the curve
// **/
//void AQLinechartWidget::setLinearScaling()
//{
//    activePlot->setLinearScaling();
//    scalingLogButton->setChecked(false);
//    scalingLinearButton->setChecked(true);
//}

/**
 * @brief Factory method to create a new button.
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
