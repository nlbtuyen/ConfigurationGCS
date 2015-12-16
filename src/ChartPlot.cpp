#include "ChartPlot.h"
#include "scrollbar.h"
#include "mainwindow.h"
#include <qwt/qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <qmath.h>

const QColor ChartPlot::baseColors[numColors] = {
    QColor(70,80,242),
    QColor(232,33,47),
    QColor(116,251,110),
    QColor(81,183,244),
    QColor(234,38,107),    
    QColor(151,59,239),
    QColor(242,255,128),
    QColor(231,72,28),
    QColor(236,48,221),
    QColor(75,133,243),
    QColor(203,254,121),
    QColor(104,64,240),
    QColor(200,54,238),
    QColor(104,250,138),
    QColor(235,43,165),
    QColor(98,248,176),
    QColor(161,252,116),
    QColor(87,231,246),
    QColor(230,126,23),
    QColor(92,247,217)
};

ChartPlot::ChartPlot(QWidget *parent):
    QwtPlot(parent),
    nextColorIndex(0),
    symbolWidth(4.0f),
    curveWidth(2.0f),
    gridWidth(0.8f),
    zoomerWidth(2.0f) // @trung
{
    // Initialize the list of curves.
    curves = QMap<QString, QwtPlotCurve*>();

    // Set the grid. The colorscheme was already set in generateColorScheme().
    grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->attach(this);

    colors = QList<QColor>();

    // Enable zooming
    QwtPlotCanvas *c = static_cast<QwtPlotCanvas*>(canvas()); // @trung
    zoomer = new ScrollZoomer(c);
    zoomer->setTrackerMode(QwtPicker::AlwaysOn);

    ///> Color map for plots, includes 20 colors
    ///> Map will start from beginning when the first 20 colors are exceeded
    for (int i = 0; i < numColors; ++i)
    {
        colors.append(baseColors[i]);
    }

    // @trung
    // Disable bottom scale and set max min value of left scale
    this->enableAxis(QwtPlot::xBottom, false);
//    this->setAxisScale(QwtPlot::yLeft, -5, 5, 1);

    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
    this->insertLegend(legend, QwtPlot::BottomLegend);

    styleChanged();
}

ChartPlot::~ChartPlot()
{

}

QColor ChartPlot::getNextColor()
{
    if(nextColorIndex >= colors.count())
    {
        nextColorIndex = 0;
    }
    return colors[nextColorIndex++];
}

QColor ChartPlot::getColorForCurve(const QString &id)
{
    return curves.value(id)->pen().color();
}

void ChartPlot::shuffleColors()
{
    foreach (QwtPlotCurve* curve, curves)
    {
        if (curve->isVisible()) {
            QPen pen(curve->pen());
            pen.setColor(getNextColor());
            curve->setPen(pen);
        }
    }
}

void ChartPlot::styleChanged()
{    
    QColor minPen(0x8C, 0x8C, 0x8C, 150);
    QColor majPen(0xB7, 0xB7, 0xB7, 150);
    QColor bgColor(255,255,255);

    resetColor();
    shuffleColors();

    grid->setMinorPen(QPen(minPen, gridWidth, Qt::DotLine));
    grid->setMajorPen(QPen(majPen, gridWidth, Qt::DotLine));
    setCanvasBackground(bgColor);

    QColor rbPen(0xB8, 0xD3, 0xE6);
    QColor trackPen(0x4A, 0xEB, 0xF7);
    zoomer->setRubberBandPen(QPen(rbPen, zoomerWidth, Qt::DotLine));
    zoomer->setTrackerPen(QPen(trackPen));

    // And finally refresh the widget to make sure all color changes are redrawn.
    replot();
}

void ChartPlot::changeMaxMinValue(int currentListCurve){
//    if ((max - min) <= 2){
//        this->setAxisScale(QwtPlot::yLeft, min, max, 0.1);
//    }else if ((max - min) >= 20 && (max - min) < 50){
//        this->setAxisScale(QwtPlot::yLeft, min, max, 5);
//    }else if ((max - min) < 20 && (max - min) > 2){
//        this->setAxisScale(QwtPlot::yLeft, min, max, 1);
//    }else if ((max - min) >= 50 && (max - min) <= 200){
//        this->setAxisScale(QwtPlot::yLeft, min, max, 10);
//    }else{
//        this->setAxisScale(QwtPlot::yLeft, min, max, 50);
    //    }
    if (currentListCurve == 0)
        this->setAxisScale(QwtPlot::yLeft, -180, 180, 30);
    else
        this->setAxisScale(QwtPlot::yLeft, -20, 20, 5);
//    this->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Floating,true);
    yScaleEngine = new QwtLinearScaleEngine();
    this->setAxisScaleEngine(QwtPlot::yLeft, yScaleEngine);
}

bool ChartPlot::checkResetZoom(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton){
        qDebug() << "right button is pressed";
        return true;
    }
}


