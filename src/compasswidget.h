#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QWidget>
#include <QPen>
#include "uasinterface.h"

class CompassWidget : public QWidget
{
    Q_OBJECT
public:    
    CompassWidget(QWidget* parent = NULL);
    ~CompassWidget();

public slots:
    //Attitude from main autopilot / system state
    void updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp);
    //Attitude from one specific component / redundant autopilot
    void updateAttitude(UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp);
    //Set the currently monitored UAS
    virtual void setActiveUAS(UASInterface* uas);

protected:
    //Preferred Size
    QSize sizeHint() const;
    //Start updating widget
    void showEvent(QShowEvent* event);
    //Stop updating widget
    void hideEvent(QHideEvent* event);

    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *event);

signals:
    void visibilityChanged(bool visible);

private:
    bool shouldDisplayNavigationData();
    void drawTextCenter(QPainter& painter, QString text, float pixelSize, float x, float y);
    void drawAICompassDisk(QPainter& painter, QRectF area, float halfspan);
    void doPaint();

    UASInterface* uas; //The uas currently monitored
    float roll;
    float pitch;
    float heading;
    float navigationCrosstrackError;
    float navigationTargetBearing;
    qreal lineWidth;
    qreal fineLineWidth;
    //Text-Size
    qreal smallTextSize;
    qreal mediumTextSize;
    qreal largeTextSize;
    qreal LeoTextSize;
    // Globally used stuff only.
    QPen instrumentEdgePen;
    QBrush instrumentBackground;
    QFont font;
    QTimer* refreshTimer;       //Timer controls the update rate
    static const int tickValues[];
    static const QString compassWindNames[];
    static const int updateInterval = 40;
};

#endif // COMPASSWIDGET_H
