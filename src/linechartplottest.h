#ifndef LINECHARTPLOTTEST_H
#define LINECHARTPLOTTEST_H

#define QUINT64_MIN Q_UINT64_C(0)
#define QUINT64_MAX Q_UINT64_C(18446744073709551615)

#include <QMap>
#include <QList>
#include <QMutex>
#include <QTime>
#include <QTimer>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_plot.h>
#include "chartplottest.h"
#include "mg.h"

class TimeScaleDrawTest: public QwtScaleDraw
{
public:

    virtual QwtText label(double v) const {
        QDateTime time = MG::TIME::msecToQDateTime(static_cast<quint64>(v));
        return time.toString("hh:mm:ss");
    }
};

/*==================================================================================*/

/**
 * Data container
 */
class QwtPlotCurve;

/**
 * Container class for the time series data
 *
 **/
class TimeSeriesDataTest
{
public:

    TimeSeriesDataTest(QwtPlot* plot, QString friendlyName = "data", quint64 plotInterval = 10000, quint64 maxInterval = 0, double zeroValue = 0);
    ~TimeSeriesDataTest();

    void append(quint64 ms, double value);

    QwtScaleMap* getScaleMap();

    int getCount() const;
    int size() const;
    const double* getX() const;
    const double* getY() const;

    const double* getPlotX() const;
    const double* getPlotY() const;
    int getPlotCount() const;

    int getID();
    QString getFriendlyName();
    double getMinValue();
    double getMaxValue();
    double getZeroValue();
    double getCurrentValue();
    void setZeroValue(double zeroValue);
    void setInterval(quint64 ms);
    void setAverageWindowSize(int windowSize);

protected:
    QwtPlot* plot;
    quint64 startTime;
    quint64 stopTime;
    quint64 interval;
    quint64 plotInterval;
    quint64 maxInterval;
    int id;
    quint64 plotCount;
    QString friendlyName;

    double lastValue; ///< The last inserted value
    double minValue;  ///< The smallest value in the dataset
    double maxValue;  ///< The largest value in the dataset
    double zeroValue; ///< The expected value in the dataset

    QMutex dataMutex;

    QwtScaleMap* scaleMap;

    void updateScaleMap();

private:
    quint64 count;
    QVector<double> ms;
    QVector<double> value;
    unsigned int averageWindow;
    QVector<double> outputMs;
    QVector<double> outputValue;
};

/*==================================================================================*/

/**
 * Time series plot
 **/
class LinechartPlotTest : public ChartPlotTest
{
    Q_OBJECT
public:
    LinechartPlotTest(QWidget *parent = NULL, int plotid=0, quint64 interval = LinechartPlotTest::DEFAULT_PLOT_INTERVAL);
    virtual ~LinechartPlotTest();

    void setZeroValue(QString id, double zeroValue);
    void removeAllData();

    QList<QwtPlotCurve*> getCurves();
    bool isVisible(QString id);
    /** Check if any curve is visible */
    bool anyCurveVisible();

    int getPlotId();
    /** Get the number of values to average over */
    int getAverageWindow();

    quint64 getMinTime();
    quint64 getMaxTime();
    quint64 getPlotInterval();
    quint64 getDataInterval();
    quint64 getWindowPosition();

    /** Get the last inserted value */
    double getCurrentValue(QString id);

    static const int SCALE_ABSOLUTE = 0;
    static const int SCALE_BEST_FIT = 1;
    static const int SCALE_LOGARITHMIC = 2;

    static const int DEFAULT_REFRESH_RATE = 100; ///< The default refresh rate is 10 Hz / every 100 ms
    static const int DEFAULT_PLOT_INTERVAL = 1000 * 16; ///< The default plot interval is 15 seconds
    static const int DEFAULT_SCALE_INTERVAL = 1000 * 8;

    // @trung
    /** Change max and min value of left scale */
    void changeMaxMin(double max, double min);

public slots:
    void appendData(QString dataname, quint64 ms, double value);
    void hideCurve(QString id);
    void showCurve(QString id);
    /** Enable auto-refreshing of plot */
    void setActive(bool active);

    // Functions referring to the currently active plot
    void setVisibleById(QString id, bool visible);

    void setCurveColor(QString id, QColor color);

    /** Enforce the use of the receive timestamp */
    void enforceGroundTime(bool enforce);
    /** Check if the receive timestamp is enforced */
    bool groundTime();

    // General interaction
    void setWindowPosition(quint64 end);
    void setPlotInterval(int interval);
    void setScaling(int scaling);
    void setAutoScroll(bool active);
    void paintRealtime();

    /** Set logarithmic plot y-axis scaling */
    void setLogarithmicScaling();
    /** Set linear plot y-axis scaling */
    void setLinearScaling();

    /** Set the number of values to average over */
    void setAverageWindow(int windowSize);
    void removeTimedOutCurves();

protected:
    QMap<QString, TimeSeriesDataTest*> data;
    QMap<QString, QwtScaleMap*> scaleMaps;
    QMap<QString, quint64> lastUpdate;

    //static const quint64 MAX_STORAGE_INTERVAL = Q_UINT64_C(300000);
    static const quint64 MAX_STORAGE_INTERVAL = Q_UINT64_C(0);  // The maximum interval which is stored

    int scaling;
    QwtScaleEngine* yScaleEngine;
    quint64 minTime;            ///< The smallest timestamp occured so far
    quint64 lastTime;           ///< Last added timestamp
    quint64 maxTime;            ///< The biggest timestamp occured so far
    quint64 maxInterval;
    quint64 storageInterval;

    double maxValue;
    double minValue;
    double valueInterval;

    int averageWindowSize;      ///< Size of sliding average / sliding median

    quint64 plotInterval;
    quint64 plotPosition;
    QTimer* updateTimer;
    QMutex datalock;
    QMutex windowLock;
    quint64 timeScaleStep;
    bool automaticScrollActive;
    QTime lastMaxTimeAdded;
    int plotid;
    bool m_active;              ///< Decides wether the plot is active or not
    bool m_groundTime;          ///< Enforce the use of the receive timestamp instead of the data timestamp
    QTimer timeoutTimer;

    // Methods
    void addCurve(QString id);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

private:
    TimeSeriesDataTest* d_data;
    QwtPlotCurve* d_curve;

signals:
    void curveAdded(QString idstring);
    void curveRemoved(QString name);
    void windowPositionChanged(quint64 position);
};

#endif // LINECHARTPLOTTEST_H
