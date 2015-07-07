#include "hudwidget.h"
#include "uasmanager.h"
#include "mainwindow.h"
#include <QDebug>
#include <QRectF>
#include <cmath>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <qmath.h>

#define SEPARATE_COMPASS_ASPECTRATIO (3.0f/4.0f)
#define LINEWIDTH 0.0036f
#define SMALL_TEXT_SIZE 0.03f
#define MEDIUM_TEXT_SIZE (SMALL_TEXT_SIZE*1.2f)
#define LARGE_TEXT_SIZE (MEDIUM_TEXT_SIZE*1.2f)

#define SHOW_ZERO_ON_SCALES true

// all in units of display height
#define ROLL_SCALE_RADIUS 0.42f
#define ROLL_SCALE_TICKMARKLENGTH 0.04f
#define ROLL_SCALE_MARKERWIDTH 0.06f
#define ROLL_SCALE_MARKERHEIGHT 0.04f
// scale max. degrees
#define ROLL_SCALE_RANGE 60

// fraction of height to translate for each degree of pitch.
#define PITCHTRANSLATION 65.0
// 10 degrees for each line
#define PITCH_SCALE_RESOLUTION 5
#define PITCH_SCALE_MAJORWIDTH 0.1
#define PITCH_SCALE_MINORWIDTH 0.066

// Beginning from PITCH_SCALE_WIDTHREDUCTION_FROM degrees of +/- pitch, the
// width of the lines is reduced, down to PITCH_SCALE_WIDTHREDUCTION times
// the normal width. This helps keep orientation in extreme attitudes.
#define PITCH_SCALE_WIDTHREDUCTION_FROM 30
#define PITCH_SCALE_WIDTHREDUCTION 0.3

#define PITCH_SCALE_HALFRANGE 15

// The number of degrees to either side of the heading to draw the compass disk.
// 180 is valid, this will draw a complete disk. If the disk is partly clipped
// away, less will do.

#define COMPASS_DISK_MAJORTICK 10
#define COMPASS_DISK_ARROWTICK 45
#define COMPASS_DISK_MAJORLINEWIDTH 0.006
#define COMPASS_DISK_MINORLINEWIDTH 0.004
#define COMPASS_DISK_RESOLUTION 10
#define COMPASS_SEPARATE_DISK_RESOLUTION 5
#define COMPASS_DISK_MARKERWIDTH 0.2
#define COMPASS_DISK_MARKERHEIGHT 0.133

#define CROSSTRACK_MAX 1000
#define CROSSTRACK_RADIUS 0.6

#define TAPE_GAUGES_TICKWIDTH_MAJOR 0.25
#define TAPE_GAUGES_TICKWIDTH_MINOR 0.15

// The altitude difference between top and bottom of scale
#define ALTIMETER_LINEAR_SPAN 50
// every 5 meters there is a tick mark
#define ALTIMETER_LINEAR_RESOLUTION 5
// every 10 meters there is a number
#define ALTIMETER_LINEAR_MAJOR_RESOLUTION 10

// Projected: An experiment. Make tape appear projected from a cylinder, like a French "drum" style gauge.
// The altitude difference between top and bottom of scale
#define ALTIMETER_PROJECTED_SPAN 50
// every 5 meters there is a tick mark
#define ALTIMETER_PROJECTED_RESOLUTION 5
// every 10 meters there is a number
#define ALTIMETER_PROJECTED_MAJOR_RESOLUTION 10
// min. and max. vertical velocity
//#define ALTIMETER_PROJECTED

// min. and max. vertical velocity
#define ALTIMETER_VVI_SPAN 5
#define ALTIMETER_VVI_WIDTH 0.2

// Now the same thing for airspeed!
#define AIRSPEED_LINEAR_SPAN 15
#define AIRSPEED_LINEAR_RESOLUTION 1
#define AIRSPEED_LINEAR_MAJOR_RESOLUTION 5

#define UNKNOWN_BATTERY -1
#define UNKNOWN_ATTITUDE 0
#define UNKNOWN_ALTITUDE -1000
#define UNKNOWN_SPEED -1
#define UNKNOWN_COUNT -1
#define UNKNOWN_GPSFIXTYPE -1

/*
 *@TODO:
 * global fixed pens (and painters too?)
 * repaint on demand multiple canvases
 * multi implementation with shared model class
 */
double HUDWidget_round(double value, int digits=0)
{
    return floor(value * pow((float)10, digits) + 0.5) / pow((float)10, digits);
}

const int HUDWidget::tickValues[] = {10, 20, 30, 45, 60};
const QString HUDWidget::compassWindNames[] = {
    QString("N"),
    QString("NE"),
    QString("E"),
    QString("SE"),
    QString("S"),
    QString("SW"),
    QString("W"),
    QString("NW")
};

HUDWidget::HUDWidget(QWidget *parent) :
    QWidget(parent),

    uas(NULL),

    altimeterMode(GPS_MAIN),
    altimeterFrame(ASL),
    speedMode(GROUND_MAIN),
    didReceivePrimaryAltitude(false),
    didReceivePrimarySpeed(false),

    roll(UNKNOWN_ATTITUDE),
    pitch(UNKNOWN_ATTITUDE),
    heading(UNKNOWN_ATTITUDE),
    primaryAltitude(UNKNOWN_ALTITUDE),
    GPSAltitude(UNKNOWN_ALTITUDE),
    aboveHomeAltitude(UNKNOWN_ALTITUDE),
    primarySpeed(UNKNOWN_SPEED),
    groundspeed(UNKNOWN_SPEED),
    verticalVelocity(UNKNOWN_ALTITUDE),
    navigationCrosstrackError(0),
    navigationTargetBearing(UNKNOWN_ATTITUDE),

    layout(COMPASS_INTEGRATED),
    style(OVERLAY_HSI),
    redColor(QColor::fromHsvF(0, 0.75, 0.9)),
    amberColor(QColor::fromHsvF(0.12, 0.6, 1.0)),
    greenColor(QColor::fromHsvF(0.25, 0.8, 0.8)),
    lineWidth(2),
    fineLineWidth(1),
    instrumentEdgePen(QColor::fromHsvF(0, 0, 0.65, 0.5)),
    instrumentBackground(QColor::fromHsvF(0, 0, 1, 0.3)),
    instrumentOpagueBackground(QColor::fromHsvF(0, 0, 0.3, 1.0)),
    font("Bitstream Vera Sans"),

    refreshTimer(new QTimer(this))
{
    //setMinimumSize(120, 80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));
    if (UASManager::instance()->getActiveUAS() != NULL) setActiveUAS(UASManager::instance()->getActiveUAS());

    // Refresh timer
    refreshTimer->setInterval(updateInterval);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
}

HUDWidget::~HUDWidget()
{
    refreshTimer->stop();
}


QSize HUDWidget::sizeHint() const
{
    return QSize(300, (300.0f * 3.0f) / 4);
}

void HUDWidget::showEvent(QShowEvent* event)
{
    // React only to internal (pre-display)
    // events
    QWidget::showEvent(event);
    refreshTimer->start(updateInterval);
    emit visibilityChanged(true);
}

void HUDWidget::hideEvent(QHideEvent* event)
{
    // React only to internal (pre-display)
    // events
    refreshTimer->stop();
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}

qreal constrain1(qreal value, qreal min, qreal max) {
    if (value<min) value=min;
    else if(value>max) value=max;
    return value;
}

void HUDWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);

    qreal size = e->size().width();

    lineWidth = constrain1(size*LINEWIDTH, 1, 6);
    fineLineWidth = constrain1(size*LINEWIDTH*2/3, 1, 2);

    instrumentEdgePen.setWidthF(fineLineWidth);

    smallTestSize = size * SMALL_TEXT_SIZE;
    mediumTextSize = size * MEDIUM_TEXT_SIZE;
    largeTextSize = size * LARGE_TEXT_SIZE;
    LeoTextSize = size * LARGE_TEXT_SIZE*1.3; //@Leo

}

void HUDWidget::paintEvent(QPaintEvent *event)
{
    // Event is not needed
    // the event is ignored as this widget
    // is refreshed automatically
    Q_UNUSED(event);
    doPaint();
}

/*
 * Interface towards qgroundcontrol
 */
/**
 *
 * @param uas the UAS/MAV to monitor/display with the HUD
 */
void HUDWidget::setActiveUAS(UASInterface* uas)
{
    if (this->uas != NULL) {
        // Disconnect any previously connected active MAV
        disconnect(this->uas, 0, this, 0);
    }

    if (uas) {
        // Now connect the new UAS
        // Setup communication
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*, double, double, double, quint64)));
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,int,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*,int,double, double, double, quint64)));
        connect(uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateGlobalPosition(UASInterface*,double,double,double,quint64)));
        connect(uas, SIGNAL(gpsSpeedChanged(UASInterface*, double, quint64)), this, SLOT(updatePrimarySpeed(UASInterface*,double,quint64)));
        connect(uas, SIGNAL(gpsSpeedChanged(UASInterface*, double, double, double, quint64)), this, SLOT(updateGPSSpeed(UASInterface*,double, double, double,quint64)));

        // Set new UAS
        this->uas = uas;
    }
}

void HUDWidget::updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw))
    {
        // TODO: Units conversion? // Called from UAS.cc l. 646
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

/*
 * TODO! Implementation or removal of this.
 * Currently a dummy.
 */
void HUDWidget::updateAttitude(UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(timestamp);
    // Called from UAS.cc l. 616
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw)) {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

/*
 * TODO! Examine what data comes with this call, should we consider it airspeed, ground speed or
 * should we not consider it at all?
 */
void HUDWidget::updatePrimarySpeed(UASInterface* uas, double speed, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);

    primarySpeed = speed;
    didReceivePrimarySpeed = true;
}

void HUDWidget::updateGPSSpeed(UASInterface* uas, double speed, double y, double z, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(y);
    Q_UNUSED(z);
    Q_UNUSED(timestamp);

    groundspeed = speed;
    if (!didReceivePrimarySpeed)
        primarySpeed = speed;
}

void HUDWidget::updateClimbRate(UASInterface* uas, double climbRate, quint64 timestamp) {
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    verticalVelocity = climbRate;
}

void HUDWidget::updatePrimaryAltitude(UASInterface* uas, double altitude, quint64 timestamp) {
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    primaryAltitude = altitude;
    didReceivePrimaryAltitude = true;
}

void HUDWidget::updateGPSAltitude(UASInterface* uas, double altitude/*, quint64 timestamp*/) {
    Q_UNUSED(uas);
    //    Q_UNUSED(timestamp);
    GPSAltitude = altitude;
    if (!didReceivePrimaryAltitude)
        primaryAltitude = altitude;
}

void HUDWidget::updateGlobalPosition(UASInterface* uas, double lat, double lon, double altitude, quint64 timestamp) {
    Q_UNUSED(lat);
    Q_UNUSED(lon);
    Q_UNUSED(timestamp);
    updateGPSAltitude(uas, altitude);
}

void HUDWidget::updateNavigationControllerErrors(UASInterface* uas, double altitudeError, double speedError, double xtrackError) {
    Q_UNUSED(uas);
    this->navigationAltitudeError = altitudeError;
    this->navigationSpeedError = speedError;
    this->navigationCrosstrackError = xtrackError;
}


/*
 * Private and such
 */

// TODO: Move to UAS. Real working implementation.
bool HUDWidget::isAirplane() {
    if (!this->uas)
        return false;
    switch(this->uas->getSystemType()) {
    case MAV_TYPE_GENERIC:
    case MAV_TYPE_FIXED_WING:
    case MAV_TYPE_AIRSHIP:
    case MAV_TYPE_FLAPPING_WING:
        return true;
    default:
        return false;
    }
}

// TODO: Implement. Should return true when navigating.
// That would be (APM) in AUTO and RTL modes.
// This could forward to a virtual on UAS bool isNavigatingAutonomusly() or whatever.
bool HUDWidget::shouldDisplayNavigationData() {
    return true;
}

void HUDWidget::drawTextCenter (
        QPainter& painter,
        QString text,
        float pixelSize,
        float x,
        float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignCenter |  Qt::TextDontClip; // For some reason the bounds rect is too small!
    painter.drawText(x /*+bounds.x()*/ -bounds.width()/2, y /*+bounds.y()*/ -bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextLeftCenter (
        QPainter& painter,
        QString text,
        float pixelSize,
        float x,
        float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignLeft | Qt::TextDontClip; // For some reason the bounds rect is too small!
    painter.drawText(x /*+bounds.x()*/, y /*+bounds.y()*/ -bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextRightCenter (
        QPainter& painter,
        QString text,
        float pixelSize,
        float x,
        float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignRight | Qt::TextDontClip; // For some reason the bounds rect is too small!
    painter.drawText(x /*+bounds.x()*/ -bounds.width(), y /*+bounds.y()*/ -bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextCenterTop (
        QPainter& painter,
        QString text,
        float pixelSize,
        float x,
        float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignCenter | Qt::TextDontClip; // For some reason the bounds rect is too small!
    painter.drawText(x /*+bounds.x()*/ -bounds.width()/2, y+bounds.height() /*+bounds.y()*/, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextCenterBottom (
        QPainter& painter,
        QString text,
        float pixelSize,
        float x,
        float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignCenter;
    painter.drawText(x /*+bounds.x()*/ -bounds.width()/2, y /*+bounds.y()*/, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawInstrumentBackground(QPainter& painter, QRectF edge) {
    painter.setPen(instrumentEdgePen);
    painter.drawRect(edge);
}


void HUDWidget::fillInstrumentOpagueBackground(QPainter& painter, QRectF edge) {
    painter.setPen(instrumentEdgePen);
    painter.setBrush(instrumentOpagueBackground);
    painter.drawRect(edge);
    painter.setBrush(Qt::NoBrush);
}

qreal pitchAngleToTranslation1(qreal viewHeight, float pitch) {
    return pitch * viewHeight / PITCHTRANSLATION;
}

inline qreal min4(qreal a, qreal b, qreal c, qreal d) {
    if(b<a) a=b;
    if(c<a) a=c;
    if(d<a) a=d;
    return a;
}

inline qreal max4(qreal a, qreal b, qreal c, qreal d) {
    if(b>a) a=b;
    if(c>a) a=c;
    if(d>a) a=d;
    return a;
}

void HUDWidget::drawAICompassDisk(QPainter& painter, QRectF area, float halfspan) {
    float start = heading - halfspan;
    float end = heading + halfspan;

    int firstTick = ceil(start / COMPASS_DISK_RESOLUTION) * COMPASS_DISK_RESOLUTION;
    int lastTick = floor(end / COMPASS_DISK_RESOLUTION) * COMPASS_DISK_RESOLUTION;

    float radius = area.width()/2;
    float innerRadius = radius * 0.96;
    painter.resetTransform();
    painter.setBrush(instrumentBackground);
    painter.setPen(instrumentEdgePen);
    painter.drawEllipse(area);
    painter.setBrush(Qt::NoBrush);

    QPen scalePen(Qt::black);
    scalePen.setWidthF(fineLineWidth);

    for (int tickYaw = firstTick; tickYaw <= lastTick; tickYaw += COMPASS_DISK_RESOLUTION) {
        int displayTick = tickYaw;
        if (displayTick < 0) displayTick+=360;
        else if (displayTick>=360) displayTick-=360;

        // yaw is in center.
        float off = tickYaw - heading;
        // wrap that to ]-180..180]
        if (off<=-180) off+= 360; else if (off>180) off -= 360;

        painter.translate(area.center());
        painter.rotate(off);
        bool drewArrow = false;
        bool isMajor = displayTick % COMPASS_DISK_MAJORTICK == 0;

        if (displayTick==30 || displayTick==60 ||
                displayTick==120 || displayTick==150 ||
                displayTick==210 || displayTick==240 ||
                displayTick==300 || displayTick==330) {
            // draw a number
            QString s_number;
            s_number.sprintf("%d", displayTick/10);
            painter.setPen(scalePen);
            drawTextCenter(painter, s_number, /*COMPASS_SCALE_TEXT_SIZE*radius*/ smallTestSize, 0, -innerRadius*0.75);
        } else {
            if (displayTick % COMPASS_DISK_ARROWTICK == 0) {
                if (displayTick!=0) {
                    QPainterPath markerPath(QPointF(0, -innerRadius*(1-COMPASS_DISK_MARKERHEIGHT/2)));
                    markerPath.lineTo(innerRadius*COMPASS_DISK_MARKERWIDTH/4, -innerRadius);
                    markerPath.lineTo(-innerRadius*COMPASS_DISK_MARKERWIDTH/4, -innerRadius);
                    markerPath.closeSubpath();
                    painter.setPen(scalePen);
                    painter.setBrush(Qt::SolidPattern);
                    painter.drawPath(markerPath);
                    painter.setBrush(Qt::NoBrush);
                    drewArrow = true;
                }
                if (displayTick%90 == 0) {
                    // Also draw a label
                    QString name = compassWindNames[displayTick / 45];
                    painter.setPen(scalePen);
                    drawTextCenter(painter, name, LeoTextSize, 0, -innerRadius*0.75);
                }
            }
        }
        // draw the scale lines. If an arrow was drawn, stay off from it.

        QPointF p_start = drewArrow ? QPoint(0, -innerRadius*0.94) : QPoint(0, -innerRadius);
        QPoint p_end = isMajor ? QPoint(0, -innerRadius*0.86) : QPoint(0, -innerRadius*0.90);

        painter.setPen(scalePen);
        painter.drawLine(p_start, p_end);
        painter.resetTransform();
    }

    painter.setPen(scalePen);
    painter.translate(area.center());
    QPainterPath markerPath(QPointF(0, -radius-2));
    markerPath.lineTo(radius*COMPASS_DISK_MARKERWIDTH/2,  -radius-radius*COMPASS_DISK_MARKERHEIGHT-2);
    markerPath.lineTo(-radius*COMPASS_DISK_MARKERWIDTH/2, -radius-radius*COMPASS_DISK_MARKERHEIGHT-2);
    markerPath.closeSubpath();
    painter.drawPath(markerPath);

    qreal digitalCompassYCenter = -radius*0.52;
    qreal digitalCompassHeight = radius*0.28;

    QPointF digitalCompassBottom(0, digitalCompassYCenter+digitalCompassHeight);
    QPointF  digitalCompassAbsoluteBottom = painter.transform().map(digitalCompassBottom);

    qreal digitalCompassUpshift = digitalCompassAbsoluteBottom.y()>height() ? digitalCompassAbsoluteBottom.y()-height() : 0;

    QRectF digitalCompassRect(-radius/3, -radius*0.52-digitalCompassUpshift, radius*2/3, radius*0.28);
    painter.setPen(instrumentEdgePen);
    painter.drawRoundedRect(digitalCompassRect, instrumentEdgePen.widthF()*2/3, instrumentEdgePen.widthF()*2/3);

    /* final safeguard for really stupid systems */
    int digitalCompassValue = static_cast<int>(qRound((double)heading)) % 360;

    //@Leo: white center value
    QString s_digitalCompass;
    s_digitalCompass.sprintf("%03d", digitalCompassValue);

    QPen pen;
    pen.setWidthF(lineWidth);
    pen.setColor(Qt::white);
    painter.setPen(pen);

    drawTextCenter(painter, s_digitalCompass, LeoTextSize, 0, -radius*0.38-digitalCompassUpshift);

    //  dummy
    //  navigationTargetBearing = 10;
    //  navigationCrosstrackError = 500;

    // The CDI
    if (shouldDisplayNavigationData() && navigationTargetBearing != UNKNOWN_ATTITUDE && !isinf(navigationCrosstrackError)) {
        painter.resetTransform();
        painter.translate(area.center());
        bool errorBeyondRadius = false;
        if (abs(navigationCrosstrackError) > CROSSTRACK_MAX) {
            errorBeyondRadius = true;
            navigationCrosstrackError = navigationCrosstrackError>0 ? CROSSTRACK_MAX : -CROSSTRACK_MAX;
        }

        float r = radius * CROSSTRACK_RADIUS;
        float x = navigationCrosstrackError / CROSSTRACK_MAX * r;
        float y = qSqrt(r*r - x*x); // the positive y, there is also a negative.

        float sillyHeading = 0;
        float angle = sillyHeading - navigationTargetBearing; // TODO: sign.
        painter.rotate(-angle);

        QPen pen;
        pen.setWidthF(lineWidth);
        pen.setColor(Qt::black);
        painter.setPen(pen);

        painter.drawLine(QPointF(x, y), QPointF(x, -y));
    }
}

#define TOP         (1<<0)
#define BOTTOM      (1<<1)
#define LEFT        (1<<2)
#define RIGHT       (1<<3)

#define TOP_2       (1<<4)
#define BOTTOM_2    (1<<5)
#define LEFT_2      (1<<6)
#define RIGHT_2     (1<<7)

void applyMargin1(QRectF& area, float margin, int where) {
    if (margin < 0.01) return;

    QRectF save(area);
    qreal consumed;

    if (where & LEFT) {
        area.setX(save.x() + (consumed = margin));
    } else if (where & LEFT_2) {
        area.setX(save.x() + (consumed = margin/2));
    } else {
        consumed = 0;
    }

    if (where & RIGHT) {
        area.setWidth(save.width()-consumed-margin);
    } else if (where & RIGHT_2) {
        area.setWidth(save.width()-consumed-margin/2);
    } else {
        area.setWidth(save.width()-consumed);
    }

    if (where & TOP) {
        area.setY(save.y() + (consumed = margin));
    } else if (where & TOP_2) {
        area.setY(save.y() + (consumed = margin/2));
    } else {
        consumed = 0;
    }

    if (where & BOTTOM) {
        area.setHeight(save.height()-consumed-margin);
    } else if (where & BOTTOM_2) {
        area.setHeight(save.height()-consumed-margin/2);
    } else {
        area.setHeight(save.height()-consumed);
    }
}

void setMarginsForInlineLayout1(qreal margin, QRectF& panel1, QRectF& panel2, QRectF& panel3, QRectF& panel4) {
    applyMargin1(panel1, margin, BOTTOM|LEFT|RIGHT_2);
    applyMargin1(panel2, margin, BOTTOM|LEFT_2|RIGHT_2);
    applyMargin1(panel3, margin, BOTTOM|LEFT_2|RIGHT_2);
    applyMargin1(panel4, margin, BOTTOM|LEFT_2|RIGHT);
}

void setMarginsForCornerLayout1(qreal margin, QRectF& panel1, QRectF& panel2, QRectF& panel3, QRectF& panel4) {
    applyMargin1(panel1, margin, BOTTOM|LEFT|RIGHT_2);
    applyMargin1(panel2, margin, BOTTOM|LEFT_2|RIGHT_2);
    applyMargin1(panel3, margin, BOTTOM|LEFT_2|RIGHT_2);
    applyMargin1(panel4, margin, BOTTOM|LEFT_2|RIGHT);
}

inline qreal tapesGaugeWidthFor(qreal containerWidth, qreal preferredAIWidth) {
    qreal result = (containerWidth - preferredAIWidth) / 2.0f;
    qreal minimum = containerWidth / 5.5f;
    if (result < minimum) result = minimum;
    return result;
}

void HUDWidget::doPaint() {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    // The AI paints on this area. It should contain the AIMainArea.
    QRectF AIPaintArea;

    painter.fillRect(rect(), Qt::black);

    qreal compassHalfSpan = 180;

    AIPaintArea = QRectF(
                0,
                0,
                width(),
                height());

    drawAICompassDisk(painter, AIPaintArea, compassHalfSpan);
    painter.end();
}

