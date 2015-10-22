
#ifndef INCREMENTALPLOT_H
#define INCREMENTALPLOT_H

#include <QTimer>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <QMap>
#include "ChartPlot.h"

class QwtPlotCurve;

/**
 *  Plot data container for growing data
 */
class CurveData
{
public:
    CurveData();

    void append(double *x, double *y, int count);

    /**  The number of datasets held in the data structure */
    int count() const;
    /**  The reserved size of the data structure in units */
    int size() const;
    const double *x() const;
    const double *y() const;

private:
    int d_count;
    QVector<double> d_x;
    QVector<double> d_y;
};

/**
 *  Incremental plotting widget
 *
 * This widget plots data incrementally when new data arrives.
 * It will only repaint the minimum screen content necessary to avoid
 * a too high CPU consumption. It auto-scales the plot to new data.
 */
class IncrementalPlot : public ChartPlot
{
    Q_OBJECT
public:
    /**  Create a new, empty incremental plot */
    IncrementalPlot(QWidget *parent = NULL);
    virtual ~IncrementalPlot();

    /**  Get the state of the grid */
    bool gridEnabled() const;

    /**  Read out data from a curve */
    int data(const QString &key, double* r_x, double* r_y, int maxSize);

public slots:
    /**  Append one data point */
    void appendData(const QString &key, double x, double y);

    /**  Append multiple data points */
    void appendData(const QString &key, double* x, double* y, int size);

    /**  Reset the plot scaling to the default value */
    void resetScaling();

    /**  Update the plot scale based on current data/symmetric mode */
    void updateScale();

    /**  Remove all data from the plot and repaint */
    void removeData();

    /**  Show the plot legend */
    void showLegend(bool show);

    /**  Show the plot grid */
    void showGrid(bool show);

    /**  Set new plot style */
    void setStyleText(const QString &style);

    /**  Set symmetric axis scaling mode */
    void setSymmetric(bool symmetric);

protected slots:
    /**  Handle the click on a legend item */
    void handleLegendClick(QwtPlotItem* item, bool on);

protected:
    bool symmetric;        ///< Enable symmetric plotting
    QwtLegend* legend;     ///< Plot legend
    double xmin;           ///< Minimum x value seen
    double xmax;           ///< Maximum x value seen
    double ymin;           ///< Minimum y value seen
    double ymax;           ///< Maximum y value seen
    QString styleText;     ///< Curve style set by setStyleText

private:
    QMap<QString, CurveData* > d_data;      ///< Data points
    /** Helper function to apply styleText style to the given curve */
    void updateStyle(QwtPlotCurve *curve);
};

#endif /* INCREMENTALPLOT_H */
