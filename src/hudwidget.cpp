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
#define SHOW_ZERO_ON_SCALES true


#define UNKNOWN_ATTITUDE 0
#define UNKNOWN_ALTITUDE -1000
#define UNKNOWN_SPEED -1

// all in units of display height
#define ROLL_SCALE_RADIUS 0.42f
#define ROLL_SCALE_MARKERWIDTH 0.06f
#define ROLL_SCALE_MARKERHEIGHT 0.04f
#define ROLL_SCALE_TICKMARKLENGTH 0.04f

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

const int HUDWidget::tickValues[] = {10, 20, 30, 45, 60};

HUDWidget::HUDWidget(QWidget *parent) :
    QWidget(parent),
    uas(NULL),
    roll(UNKNOWN_ATTITUDE),
    pitch(UNKNOWN_ATTITUDE),
    heading(UNKNOWN_ATTITUDE),
    navigationTargetBearing(UNKNOWN_ATTITUDE),
    style(OVERLAY_HSI),
    redColor(QColor::fromHsvF(0, 0.75, 0.9)),
    amberColor(QColor::fromHsvF(0.12, 0.6, 1.0)),
    greenColor(QColor::fromHsvF(0.25, 0.8, 0.8)),
    lineWidth(2),
    fineLineWidth(1),
    instrumentEdgePen(QColor::fromHsvF(0, 0, 0.65, 0.5)),
    instrumentBackground(QColor::fromHsvF(0, 0, 0.3, 0.3)),
    instrumentOpagueBackground(QColor::fromHsvF(0, 0, 0.3, 1.0)),
    font("Bitstream Vera Sans"),
    refreshTimer(new QTimer(this))
{
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

void HUDWidget::updateAttitude(UASInterface *uas, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw))
    {
        this->roll = roll * (180.0 / M_PI);
        this->pitch = pitch * (180.0 / M_PI);
        yaw = yaw * (180.0 / M_PI);
        if (yaw<0) yaw+=360;
        this->heading = yaw;
    }
}

void HUDWidget::updateAttitude(UASInterface *uas, int component, double roll, double pitch, double yaw, quint64 timestamp)
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

void HUDWidget::setActiveUAS(UASInterface *uas)
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
        // Set new UAS
        this->uas = uas;
    }
}

void HUDWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    doPaint();
}
qreal  constrain_HUD(qreal value, qreal min, qreal max) {
    if (value<min) value=min;
    else if(value>max) value=max;
    return value;
}

void HUDWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    qreal size = e->size().width();

    lineWidth =  constrain_HUD(size*LINEWIDTH, 1, 6);
    fineLineWidth =  constrain_HUD(size*LINEWIDTH*2/3, 1, 2);

    instrumentEdgePen.setWidthF(fineLineWidth);

    smallTextSize = size * SMALL_TEXT_SIZE;
    mediumTextSize = size * MEDIUM_TEXT_SIZE;
    largeTextSize = size * LARGE_TEXT_SIZE;
    LeoTextSize = size * LARGE_TEXT_SIZE*1.3; //@Leo
}

QSize HUDWidget::sizeHint() const
{
    return QSize(300, (300.0f * 3.0f) / 4);
}

void HUDWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refreshTimer->start(updateInterval);
    emit visibilityChanged(true);
}

void HUDWidget::hideEvent(QHideEvent *event)
{
    refreshTimer->stop();
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}

void HUDWidget::drawTextLeftCenter(QPainter &painter, QString text, float pixelSize, float x, float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignLeft | Qt::TextDontClip;
    painter.drawText(x, y-bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextRightCenter(QPainter &painter, QString text, float pixelSize, float x, float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignRight | Qt::TextDontClip;
    painter.drawText(x - bounds.width(), y - bounds.height()/2, bounds.width(), bounds.height(), flags, text);
}

void HUDWidget::drawTextCenterBottom(QPainter &painter, QString text, float pixelSize, float x, float y)
{
    font.setPixelSize(pixelSize);
    painter.setFont(font);

    QFontMetrics metrics = QFontMetrics(font);
    QRect bounds = metrics.boundingRect(text);
    int flags = Qt::AlignCenter;
    painter.drawText(x-bounds.width()/2, y , bounds.width(), bounds.height(), flags, text);
}

qreal pitchAngleToTranslation_HUD(qreal viewHeight, float pitch) {
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

//Ve Background
void HUDWidget::drawAIGlobalFeatures(QPainter &painter, QRectF mainArea, QRectF paintArea)
{
    painter.resetTransform();
    painter.translate(QPointF(85,85)); //@Leo: old: painter.translate(mainArea.center() );

    qreal pitchPixels = pitchAngleToTranslation_HUD(mainArea.height(), pitch);
    qreal gradientEnd = pitchAngleToTranslation_HUD(mainArea.height(), 60);

    if (roll == roll) // check for NaN
        painter.rotate(-roll);
    painter.translate(0, pitchPixels);

    // Calculate the radius of area we need to paint to cover all.
    QTransform rtx = painter.transform().inverted();

    QPointF topLeft = rtx.map(paintArea.topLeft());
    QPointF topRight = rtx.map(paintArea.topRight());
    QPointF bottomLeft = rtx.map(paintArea.bottomLeft());
    QPointF bottomRight = rtx.map(paintArea.bottomRight());
    // Just make a rectangluar basis.
    qreal minx = min4(topLeft.x(), topRight.x(), bottomLeft.x(), bottomRight.x());
    qreal maxx = max4(topLeft.x(), topRight.x(), bottomLeft.x(), bottomRight.x());
    qreal miny = min4(topLeft.y(), topRight.y(), bottomLeft.y(), bottomRight.y());
    qreal maxy = max4(topLeft.y(), topRight.y(), bottomLeft.y(), bottomRight.y());

    QPointF hzonLeft = QPoint(minx, 0);
    QPointF hzonRight = QPoint(maxx, 0);

    //Sky
    QPainterPath skyPath(hzonLeft);
    skyPath.lineTo(QPointF(minx, miny));
    skyPath.lineTo(QPointF(maxx, miny));
    skyPath.lineTo(hzonRight);
    skyPath.closeSubpath();

    QLinearGradient skyGradient(0, -gradientEnd, 0, 0);
    skyGradient.setColorAt(0, QColor::fromHsvF(0.6, 1.0, 0.7));
    skyGradient.setColorAt(1, QColor::fromHsvF(0.6, 0.25, 0.9));
    QBrush skyBrush(skyGradient);
    painter.fillPath(skyPath, skyBrush);

    //Ground
    QPainterPath groundPath(hzonRight);
    groundPath.lineTo(maxx, maxy);
    groundPath.lineTo(minx, maxy);
    groundPath.lineTo(hzonLeft);
    groundPath.closeSubpath();

    QLinearGradient groundGradient(0, gradientEnd, 0, 0);
    groundGradient.setColorAt(0, QColor::fromHsvF(0.25, 1, 0.5));
    groundGradient.setColorAt(1, QColor::fromHsvF(0.25, 0.25, 0.5));
    QBrush groundBrush(groundGradient);
    painter.fillPath(groundPath, groundBrush);

    //Green Middle Line
    QPen pen;
    pen.setWidthF(lineWidth);
    pen.setColor(greenColor);
    painter.setPen(pen);
    QPointF start(-mainArea.width(), 0);
    QPoint end(mainArea.width(), 0);
    painter.drawLine(start, end);
}

void HUDWidget::drawAIAirframeFixedFeatures(QPainter &painter, QRectF area)
{
    painter.resetTransform();
    painter.translate(QPointF(85,85)); //@Leo: old: painter.translate(mainArea.center() );

    qreal w = area.width();
    qreal h = area.height();

    QPen pen;
    pen.setWidthF(lineWidth * 1.5);
    pen.setColor(redColor);
    painter.setPen(pen);

    float length = 0.15f; //length of red line
    float side = 0.7f;//@Leo old: 0.5f
    // The 2 lines at sides.
    painter.drawLine(QPointF(-side*w, 0), QPointF(-(side-length)*w, 0));
    painter.drawLine(QPointF(side*w, 0), QPointF((side-length)*w, 0));

    float rel = length/qSqrt(2); //red line
    // The gull
    painter.drawLine(QPointF(rel*w, rel*w/2), QPoint(0, 0)); //red line
    painter.drawLine(QPointF(-rel*w, rel*w/2), QPoint(0, 0)); // red line

    // The roll scale marker.
    QPainterPath markerPath(QPointF(0, -w*ROLL_SCALE_RADIUS+1));
    markerPath.lineTo(-h*ROLL_SCALE_MARKERWIDTH/2, -w*(ROLL_SCALE_RADIUS-ROLL_SCALE_MARKERHEIGHT)+1);
    markerPath.lineTo(h*ROLL_SCALE_MARKERWIDTH/2, -w*(ROLL_SCALE_RADIUS-ROLL_SCALE_MARKERHEIGHT)+1);
    markerPath.closeSubpath();
    painter.drawPath(markerPath);
}

//Ve Pitch - Compass goi
void HUDWidget::drawPitchScale(QPainter &painter, QRectF area, float intrusion, bool drawNumbersLeft, bool drawNumbersRight)
{
    Q_UNUSED(area);
    Q_UNUSED(intrusion);
    // The area should be quadratic but if not width is the major size.
    qreal w = 170;

    QPen pen;
    pen.setWidthF(lineWidth);
    pen.setColor(Qt::white);
    painter.setPen(pen);

    QTransform savedTransform = painter.transform();

    // find the mark nearest center
    int snap = qRound((double)(pitch/PITCH_SCALE_RESOLUTION))*PITCH_SCALE_RESOLUTION;
    int _min = snap-PITCH_SCALE_HALFRANGE;
    int _max = snap+PITCH_SCALE_HALFRANGE;
    for (int degrees=_min; degrees<=_max; degrees+=PITCH_SCALE_RESOLUTION) {
        bool isMajor = degrees % (PITCH_SCALE_RESOLUTION*2) == 0;
        float linewidth =  isMajor ? PITCH_SCALE_MAJORWIDTH : PITCH_SCALE_MINORWIDTH;
        if (abs(degrees) > PITCH_SCALE_WIDTHREDUCTION_FROM) {
            int fromVertical = abs(pitch>=0 ? 90-pitch : -90-pitch);
            float temp = fromVertical * 1/(90.0f-PITCH_SCALE_WIDTHREDUCTION_FROM);
            linewidth *= (PITCH_SCALE_WIDTHREDUCTION * (1-temp) + temp);
        }

        float shift = pitchAngleToTranslation_HUD(w, pitch-degrees);

        // Intrusion detection and evasion.

        painter.translate(0, shift);
        QPointF start(-linewidth*w, 0);
        QPointF end(linewidth*w, 0);

        painter.drawLine(start, end);

        if (isMajor && (drawNumbersLeft||drawNumbersRight)) {
            int displayDegrees = degrees;
            if(displayDegrees>90)
                displayDegrees = 180-displayDegrees;
            else if (displayDegrees<-90)
                displayDegrees = -180 - displayDegrees;
            if (SHOW_ZERO_ON_SCALES || degrees) {
                QString s_number;
                s_number.sprintf("%d", displayDegrees);
                if (drawNumbersLeft)
                    drawTextRightCenter(painter, s_number, LeoTextSize, -0.1 * w, 0); //@Leo : length cua thanh doc
                if (drawNumbersRight)
                    drawTextLeftCenter(painter, s_number, LeoTextSize, 0.1 * w, 0); //@Leo : length cua thanh doc
            }
        }
        painter.setTransform(savedTransform);
    }
}

//Ve Roll - Vong cung
void HUDWidget::drawRollScale(QPainter &painter, QRectF area, bool drawTicks, bool drawNumbers)
{
    qreal w = area.width();
    if (w<area.height()) w = area.height();

    QPen pen;
    pen.setWidthF(lineWidth);
    pen.setColor(Qt::white);
    painter.setPen(pen);

    qreal _size = w * ROLL_SCALE_RADIUS*2;
    QRectF arcArea(-_size/2, - _size/2, _size, _size);
    painter.drawArc(arcArea, (90-ROLL_SCALE_RANGE)*16, ROLL_SCALE_RANGE*2*16);
    if (drawTicks) {
        int length = sizeof(tickValues)/sizeof(int);
        qreal previousRotation = 0;
        for (int i=0; i<length*2+1; i++) {
            int degrees = (i==length) ? 0 : (i>length) ?-tickValues[i-length-1] : tickValues[i];
            painter.rotate(degrees - previousRotation);
            previousRotation = degrees;

            QPointF start(0, -_size/2);
            QPointF end(0, -(1.0+ROLL_SCALE_TICKMARKLENGTH)*_size/2);

            painter.drawLine(start, end);

            QString s_number;
            if (SHOW_ZERO_ON_SCALES || degrees)
                s_number.sprintf("%d", abs(degrees));

            if (drawNumbers) {
                drawTextCenterBottom(painter, s_number, LeoTextSize, 0, -(ROLL_SCALE_RADIUS+ROLL_SCALE_TICKMARKLENGTH*6)*w); //@Leo
            }
        }
    }
}

//Ve Compass : ve pitch + roll
void HUDWidget::drawAIAttitudeScales(QPainter &painter, QRectF area, float intrusion)
{
    // To save computations, we do these transformations once for both scales:
    painter.resetTransform();
    painter.translate(QPointF(85,85)); //@Leo: old: painter.translate(mainArea.center() );
    painter.rotate(-roll);
    QTransform saved = painter.transform();

    drawRollScale(painter, area, true, true); //vong cung
    painter.setTransform(saved);
    drawPitchScale(painter, area, intrusion, true, true); //line doc
}

inline qreal tapesGaugeWidthFor(qreal containerWidth, qreal preferredAIWidth) {
    qreal result = (containerWidth - preferredAIWidth) / 2.0f;
    qreal minimum = containerWidth / 5.5f;
    if (result < minimum) result = minimum;
    return result;
}

/*
 * =========================================
 * ============= Main: Paint ===============
 * =========================================
 */

void HUDWidget::doPaint()
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    // The AI centers on this area.
    QRectF AIMainArea;
    // The AI paints on this area. It should contain the AIMainArea.
    QRectF AIPaintArea;

    qreal tapeGaugeWidth;
    float compassAIIntrusion = 0;
    tapeGaugeWidth = tapesGaugeWidthFor(width(), width());
    qreal aiheight = width() - tapeGaugeWidth*2;
    qreal panelsHeight = 0;

    AIMainArea = QRectF(
                tapeGaugeWidth,
                0,
                width()-tapeGaugeWidth*2,
                aiheight);

    AIPaintArea = style == OVERLAY_HSI ?
                QRectF(
                    0,
                    0,
                    width(),
                    height() - panelsHeight) : AIMainArea;

    bool hadClip = painter.hasClipping();
    painter.setClipping(true);
    painter.setClipRect(AIPaintArea);

    //@Leo: Ve Background
    drawAIGlobalFeatures(painter, AIMainArea, AIPaintArea);
    //@Leo: Ve compass tren
    drawAIAttitudeScales(painter, AIMainArea, compassAIIntrusion);
    drawAIAirframeFixedFeatures(painter, AIMainArea);

    QRectF area = QRectF(-85,-85,170,170);
    painter.setPen(instrumentEdgePen);
    painter.drawEllipse(area);

    ///@Leo: circle of widget
    QBrush color(QColor(240,240,240));
    QPointF top = QPoint(0,-85);
    QPointF right = QPoint(85,0);
    QPointF bottom = QPoint(0,85);
    QPointF left = QPoint(-85,0);
    QRectF rect = QRect(-85,-85,170,170);

    //Top Right
    QPainterPath topRight(top);
    topRight.lineTo(85,-85);
    topRight.lineTo(right);
    topRight.arcTo(rect,0,90.0);
    painter.fillPath(topRight,color);

    //Top Left
    QPainterPath topLeft(left);
    topLeft.lineTo(-85,-85);
    topLeft.lineTo(top);
    topLeft.arcTo(rect,90,90);
    painter.fillPath(topLeft,color);

    //Bottom Left
    QPainterPath bottomLeft(bottom);
    bottomLeft.lineTo(-85,85);
    bottomLeft.lineTo(left);
    bottomLeft.arcTo(rect,180.0,90.0);
    painter.fillPath(bottomLeft,color);

    //Bottom Right
    QPainterPath bottomRight(right);
    bottomRight.lineTo(85,85);
    bottomRight.lineTo(bottom);
    bottomRight.arcTo(rect,270,90);
    painter.fillPath(bottomRight,color);

    painter.setClipping(hadClip);
    painter.end();
}

