#ifndef HUDWIDGET_H
#define HUDWIDGET_H

#include <QWidget>
#include <QPen>
#include "uasinterface.h"

class HUDWidget : public QWidget
{
    Q_OBJECT
public:
    HUDWidget(QWidget* parent = NULL);
    ~HUDWidget();

public slots:
    /** @brief Attitude from main autopilot / system state */
    void updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp);
    /** @brief Attitude from one specific component / redundant autopilot */
    void updateAttitude(UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp);
    //Set the currently monitored UAS
    virtual void setActiveUAS(UASInterface* uas);

protected:
    enum Style {
        NO_OVERLAYS,                    // Hzon not visible through tapes nor through feature panels. Frames with margins between.
        OVERLAY_HORIZONTAL,             // Hzon visible through tapes and (frameless) feature panels.
        OVERLAY_HSI                     // Hzon visible through everything except bottom feature panels.
    };

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *e);

    /** @brief Preferred Size */
    QSize sizeHint() const;
    /** @brief Start updating widget */
    void showEvent(QShowEvent* event);
    /** @brief Stop updating widget */
    void hideEvent(QHideEvent* event);

signals:
    void visibilityChanged(bool visible);

private:
    void drawTextLeftCenter(QPainter& painter, QString text, float pixelSize, float x, float y);
    void drawTextRightCenter(QPainter& painter, QString text, float pixelSize, float x, float y);
    void drawTextCenterBottom(QPainter& painter, QString text, float pixelSize, float x, float y);
    void drawAIGlobalFeatures(QPainter& painter, QRectF mainArea, QRectF paintArea);
    void drawAIAirframeFixedFeatures(QPainter& painter, QRectF area);
    void drawPitchScale(QPainter& painter, QRectF area, float intrusion, bool drawNumbersLeft, bool drawNumbersRight);
    void drawRollScale(QPainter& painter, QRectF area, bool drawTicks, bool drawNumbers);
    void drawAIAttitudeScales(QPainter& painter, QRectF area, float intrusion);

    void doPaint();

    UASInterface* uas;          ///< The uas currently monitored

    float roll;
    float pitch;
    float heading;
    float navigationTargetBearing;

    Style style;        // The AI style (tapes translusent or opague)
    QColor redColor;
    QColor amberColor;
    QColor greenColor;
    qreal lineWidth;
    qreal fineLineWidth;
    qreal smallTextSize;
    qreal mediumTextSize;
    qreal largeTextSize;
    qreal LeoTextSize;

    static const int tickValues[];

    // Globally used stuff only.
    QPen instrumentEdgePen;
    QBrush instrumentBackground;
    QBrush instrumentOpagueBackground;
    QFont font;
    QTimer* refreshTimer;       ///< The main timer, controls the update rate
    static const int updateInterval = 40;

};

#endif // HUDWIDGET_H
