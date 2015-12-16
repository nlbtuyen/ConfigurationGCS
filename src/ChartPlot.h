#ifndef CHARTPLOT_H
#define CHARTPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include "scrollzoomer.h"
#include <QMouseEvent>
#include <qwt_scale_engine.h>

class ChartPlot : public QwtPlot
{
    Q_OBJECT
public:
    ChartPlot(QWidget *parent = NULL);
    virtual ~ChartPlot();

    /** Get next color of color map */
    QColor getNextColor();

    /** Get color for curve id */
    QColor getColorForCurve(const QString &id);

    /** Reset color map */
    void shuffleColors();

    void resetColor() { nextColorIndex = 0; }

    // @trung
    /** Change max and min value of left scale */
    void changeMaxMinValue(int currentListCurve); // @trung
    bool checkResetZoom(QMouseEvent *event);

public slots:

    /** Generate coloring for this plot canvas based on current window theme */
    void styleChanged();

protected:
    const static int numColors = 20;
    const static QColor baseColors[numColors];
    QList<QColor> colors;                       ///< Colormap for curves
    int nextColorIndex;                         ///< Next index in color map
    QMap<QString, QwtPlotCurve* > curves;       ///< Plot curves
    QwtPlotGrid* grid;                          ///< Plot grid

    float symbolWidth;                          ///< Width of curve symbols in pixels
    float curveWidth;                           ///< Width of curve lines in pixels
    float gridWidth;                            ///< Width of gridlines in pixels

    ScrollZoomer* zoomer;  ///< Zoomer class for widget     // @trung
    float zoomerWidth; ///< Width of zoomer selection box   // @trung
    QwtScaleEngine* yScaleEngine;
};

#endif // CHARTPLOT_H
