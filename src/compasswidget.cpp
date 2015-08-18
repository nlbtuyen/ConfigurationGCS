#include "compasswidget.h"
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

#ifndef isinf
#define isinf(x) ((x)!=(x))
#endif

#ifndef isnan
#define isnan(x) ((x)!=(x))
#endif

#define LINEWIDTH 0.0036f
#define SMALL_TEXT_SIZE 0.03f
#define MEDIUM_TEXT_SIZE (SMALL_TEXT_SIZE*1.2f)
#define LARGE_TEXT_SIZE (MEDIUM_TEXT_SIZE*1.2f)

#define COMPASS_DISK_MAJORTICK 10
#define COMPASS_DISK_ARROWTICK 45
#define COMPASS_DISK_RESOLUTION 10
#define COMPASS_DISK_MARKERWIDTH 0.2
#define COMPASS_DISK_MARKERHEIGHT 0.133

#define CROSSTRACK_MAX 1000
#define CROSSTRACK_RADIUS 0.6

//Air-speed
#define UNKNOWN_ATTITUDE 0
#define UNKNOWN_SPEED -1

const QString CompassWidget::compassWindNames[] = {
    QString("N"),
    QString("NE"),
    QString("E"),
    QString("SE"),
    QString("S"),
    QString("SW"),
    QString("W"),
    QString("NW")
};

CompassWidget::CompassWidget(QWidget *parent):
    QWidget(parent),
    uas(NULL),
    roll(UNKNOWN_ATTITUDE),
    pitch(UNKNOWN_ATTITUDE),
    heading(UNKNOWN_ATTITUDE),
    navigationCrosstrackError(0),
    navigationTargetBearing(UNKNOWN_ATTITUDE),
    lineWidth(2),
    fineLineWidth(1),
    instrumentEdgePen(QColor::fromHsvF(0, 0, 0.65, 0.5)),
    instrumentBackground(QColor::fromHsvF(0, 0, 1, 0.3)),
    font("Bitstream Vera Sans"),
    refreshTimer(new QTimer(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));
    if (UASManager::instance()->getActiveUAS() != NULL)
        setActiveUAS(UASManager::instance()->getActiveUAS());

    // Refresh timer
    refreshTimer->setInterval(updateInterval);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
}

CompassWidget::~CompassWidget()
{
    refreshTimer->stop();
}

void CompassWidget::setActiveUAS(UASInterface *uas)
{
    if (this->uas != NULL) {
        // Disconnect any previously connected active MAV
        disconnect(this->uas, 0, this, 0);
    }

    if (uas)
    {
        // Now connect the new UAS
        // Setup communication
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*, double, double, double, quint64)));
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,int,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*,int,double, double, double, quint64)));
        // Set new UAS
        this->uas = uas;
    }
}

void CompassWidget::updateAttitude(UASInterface *uas, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw)) {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

void CompassWidget::updateAttitude(UASInterface *uas, int component, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw)) {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

QSize CompassWidget::sizeHint() const
{
    return QSize(300, (300.0f * 3.0f) / 4);
}

void CompassWidget::showEvent(QShowEvent *event)
{
    // React only to internal (pre-display) events
    QWidget::showEvent(event);
    refreshTimer->start(updateInterval);
    emit visibilityChanged(true);
}

void CompassWidget::hideEvent(QHideEvent *event)
{
    // React only to internal (pre-display) events
    refreshTimer->stop();
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}

qreal constrain_compass(qreal value, qreal min, qreal max) {
    if (value<min) value=min;
    else if(value>max) value=max;
    return value;
}

void CompassWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    qreal size = e->size().width();

    lineWidth = constrain_compass(size*LINEWIDTH, 1, 6);
    fineLineWidth = constrain_compass(size*LINEWIDTH*2/3, 1, 2);

    instrumentEdgePen.setWidthF(fineLineWidth);

    smallTextSize = size * SMALL_TEXT_SIZE;
    mediumTextSize = size * MEDIUM_TEXT_SIZE;
    largeTextSize = size * LARGE_TEXT_SIZE;
    LeoTextSize = size * LARGE_TEXT_SIZE*1.3; //@Leo

}

void CompassWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    doPaint();
}

bool CompassWidget::shouldDisplayNavigationData()
{
    return true;
}

void CompassWidget::drawTextCenter(QPainter &painter, QString text, float pixelSize, float x, float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);
    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignCenter |  Qt::TextDontClip;
    painter.drawText(x - bounds.width()/2, y - bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

//draw compass
void CompassWidget::drawAICompassDisk(QPainter &painter, QRectF area, float halfspan)
{
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

    for (int tickYaw = firstTick; tickYaw <= lastTick; tickYaw += COMPASS_DISK_RESOLUTION)
    {
        int displayTick = tickYaw;
        if (displayTick < 0)
            displayTick += 360;
        else if (displayTick >= 360)
            displayTick -= 360;

        //yaw in center
        float off = tickYaw - heading;
        //wrap that to [-180..180]
        if (off <= -180)
            off += 360;
        else if (off > 180)
            off -= 360;

        painter.translate(area.center());
        painter.rotate(off);

        bool drewArrow = false;
        bool isMajor = displayTick % COMPASS_DISK_MAJORTICK == 0;

        if (displayTick==30 || displayTick==60 ||
                displayTick==120 || displayTick==150 ||
                displayTick==210 || displayTick==240 ||
                displayTick==300 || displayTick==330)
        {
            // draw a number
            QString s_number;
            s_number.sprintf("%d", displayTick/10);
            painter.setPen(scalePen);
            drawTextCenter(painter, s_number, smallTextSize, 0, -innerRadius*0.75);
        }
        else
        {
            if (displayTick % COMPASS_DISK_ARROWTICK == 0)
            {
                //draw arrow
                QPainterPath markerPath(QPointF(0, -innerRadius*(1-COMPASS_DISK_MARKERHEIGHT/2)));
                markerPath.lineTo(innerRadius*COMPASS_DISK_MARKERWIDTH/4, -innerRadius);
                markerPath.lineTo(-innerRadius*COMPASS_DISK_MARKERWIDTH/4, -innerRadius);
                markerPath.closeSubpath();
                painter.setPen(scalePen);
                painter.setBrush(Qt::SolidPattern);
                painter.drawPath(markerPath);
                painter.setBrush(Qt::NoBrush);
                drewArrow = true;

                if ((displayTick%90 == 0))
                {
                    // Also draw a label for 0 90 180 && 270
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

    ///draw rect cover center number
    painter.setPen(instrumentEdgePen);
    painter.drawRoundedRect(digitalCompassRect, instrumentEdgePen.widthF()*2/3, instrumentEdgePen.widthF()*2/3);

    /* final safeguard for really stupid systems */
    int digitalCompassValue = static_cast<int>(qRound((double)heading)) % 360;

    //@Leo: white center value
    QString s_digitalCompass;
    s_digitalCompass.sprintf("%03d", digitalCompassValue);

    QPen pen;
    pen.setWidthF(lineWidth);
    pen.setColor(QColor(220,91,33));
    painter.setPen(pen);

    drawTextCenter(painter, s_digitalCompass, LeoTextSize, 0, -radius*0.38-digitalCompassUpshift);

}

/*
 * =========================================
 * ============= Main: Paint ===============
 * =========================================
 */
void CompassWidget::doPaint()
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    // The AI paints on this area. It should contain the AIMainArea.
    QRectF AIPaintArea;

    qreal compassHalfSpan = 180;

    AIPaintArea = QRectF(
                0,
                0,
                width(),
                height());

    drawAICompassDisk(painter, AIPaintArea, compassHalfSpan);
    painter.end();
}


