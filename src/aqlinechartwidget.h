#ifndef AQLINECHARTWIDGET_H
#define AQLINECHARTWIDGET_H

#include <QGridLayout>
#include <QWidget>
#include <QFrame>
#include <QComboBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QScrollBar>
#include <QSpinBox>
#include <QMap>
#include <QString>
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <QReadWriteLock>
#include <QToolButton>
#include <QTimer>
#include <qwt_plot_curve.h>

#include "LinechartPlot.h"
#include "uasinterface.h"
#include "ui_AQLinechart.h"

class AQLinechartWidget : public QWidget
{
    Q_OBJECT

public:
    AQLinechartWidget(int systemid, QWidget *parent = 0);
    ~AQLinechartWidget();
    // @trung
    /** Set curve visible */
    void setCurveVisible(QString curve, bool visible);
    /** Set maxValue of left scale */
    double maxValue;
    /** Set minValue of left scale */
    double minValue;    

    static const int MIN_TIME_SCROLLBAR_VALUE = 0;      ///< The minimum scrollbar value
    static const int MAX_TIME_SCROLLBAR_VALUE = 16383;  ///< The maximum scrollbar value
    static const int DEFAULT_CURVE_LIST_WIDTH = 200;    // default width of curve listing frame
    static const int DEFAULT_AVG_WINDOW = 200;          // default average window value
    static const quint64 DEFAULT_PLOT_INTERVAL = 10;    // plot interval - time length of X axis
    static const int UPDATE_INTERVAL = 500;             ///< Time between number updates, in milliseconds
    static const int MAX_CURVE_MENUITEM_NUMBER = 8;
    static const int PAGESTEP_TIME_SCROLLBAR_VALUE = (MAX_TIME_SCROLLBAR_VALUE - MIN_TIME_SCROLLBAR_VALUE) / 10;

public slots:
    void addCurve(const QString& curve, const QString& unit);
    void removeCurve(QString curve);
    /** Remove all curves */
    void clearCurves();
    /** Append double data to the given curve. */
    void appendData(int uasId, const QString& curve, const QString& unit, QVariant& variant, quint64 usec);

    void takeButtonClick(bool checked);
    void setPlotWindowPosition(int scrollBarValue);
    void setPlotWindowPosition(quint64 position);
    void setPlotInterval(int interval);
    /** Start automatic updates once visible */
    void showEvent(QShowEvent* event);
    /** Stop automatic updates once hidden */
    void hideEvent(QHideEvent* event);
    void setActive(bool active);
    /** Select one MAV for curve display */
    void selectActiveSystem(int mav);
    /** Set the number of values to average over */
    void setAverageWindow(int windowSize);
    /** Refresh the view */
    void refresh();
    /** Write the current configuration to disk */
    void writeSettings();
    /** Read the current configuration from disk */
    void readSettings();
    /** Check max and min value change */
    void checkMaxMin(QString str);

protected:
    void removeCurveFromList(QString curve);
    QToolButton* createButton(QWidget* parent);
    void createCurveItem(QString curve);
    void createLayout();
    /** Get the name for a curve key */
    QString getCurveName(const QString& key, bool shortEnabled);

    int sysid;                            ///< ID of the unmanned system this plot belongs to
    LinechartPlot* activePlot;            ///< Plot for this system
    QReadWriteLock* curvesLock;           ///< A lock (mutex) for the concurrent access on the curves
    QReadWriteLock plotWindowLock;        ///< A lock (mutex) for the concurrent access on the window position

    int curveListIndex;
    int curveListCounter;                   ///< Counter of curves in curve list
    QList<QString>* ListItems;              ///< Curves listed
    QList<QString>* listedCurves;           ///< Curves listed
    QMap<QString, QLabel*>* curveLabels;    ///< References to the curve labels
    QMap<QString, QLabel*> curveNameLabels; ///< References to the curve labels
    QMap<QString, QString> curveNames;      ///< Full curve names
    QMap<QString, int> intData;             ///< Current values for integer-valued curves
    QMap<QString, QWidget*> colorIcons;     ///< Reference to color icons
    QMap<QString, QCheckBox*> checkBoxes;   ///< Reference to curve selection checkboxes
    QMap<QString, QString> curveUnits;      ///< Curve units by name

    QWidget* curvesWidget;                ///< The QWidget containing the curve selection button
    QGridLayout* curvesWidgetLayout;      ///< The layout for the curvesWidget QWidget
    QScrollBar* scrollbar;                ///< The plot window scroll bar

    QAction* addNewCurve;                 ///< Add curve candidate to the active curves

    QMenu* curveMenu;
    QGridLayout* mainLayout;

    QPointer<QCheckBox> timeButton;

    QTimer* updateTimer;
    QCheckBox* selectAllCheckBox;
    int selectedMAV; ///< The MAV for which plot items are accepted, -1 for all systems    

private:
    Ui::AQLinechart ui;

signals:
    /**
         * This signal is emitted if a curve is removed from the list
         *
         * @param curve The removed plot curve
         **/
    void curveRemoved(QString curve);

    /**
         * This signal is emitted if a curve has been moved or added
         *
         * @param curve The moved or added curve
         * @param position The x-position of the curve (The centerline)
         **/
    void curveSet(QString curve, int position);

    /**
         * This signal is emitted to change the visibility of a curve
         *
         * @param curve The changed curve
         * @pram visible The visibility
         **/
    void curveVisible(QString curve, bool visible);

    void plotWindowPositionUpdated(quint64 position);
    void plotWindowPositionUpdated(int position);
};

#endif // AQLINECHARTWIDGET_H
