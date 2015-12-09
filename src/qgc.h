/*=====================================================================

 QGroundControl Open Source Ground Control Station

 (c) 2009 - 2011 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

 This file is part of the QGROUNDCONTROL project

 QGROUNDCONTROL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 QGROUNDCONTROL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

 ======================================================================*/

#ifndef QGC_H
#define QGC_H

#include <QDateTime>
#include <QColor>
#include <QThread>

#define MAVLINK_HEARTBEAT_DEFAULT_RATE 1


namespace QGC
{
const static int defaultSystemId = 255;
const static int defaultComponentId = 0;

const QColor colorCyan(55, 154, 195);
const QColor colorRed(154, 20, 20);
const QColor colorGreen(20, 200, 20);
const QColor colorYellow(255, 255, 0);
const QColor colorOrange(255, 140, 0);
const QColor colorMagenta(255, 0, 55);
const QColor colorDarkWhite(240, 240, 240);
const QColor colorDarkYellow(180, 180, 0);
const QColor colorBackground("#050508");
const QColor colorBlack(0, 0, 0);

/**  Get the current ground time in microseconds */
quint64 groundTimeUsecs();
/**  Get the current ground time in milliseconds */
quint64 groundTimeMilliseconds();
/**  Get the current ground time in seconds */
qreal groundTimeSeconds();
/**  Returns the angle limited to -pi - pi */
float limitAngleToPMPIf(float angle);
/**  Returns the angle limited to -pi - pi */
double limitAngleToPMPId(double angle);

/**  Returns the angle into degree */
float changeAngleToDegreeF(float angle);
double changeAngleToDegreeD(double angle);

const static int MAX_FLIGHT_TIME = 60 * 60 * 24 * 21;

class SLEEP : public QThread
{
public:
    /**
     *  Set a thread to sleep for seconds
     * @param s time in seconds to sleep
     **/
    static void sleep(unsigned long s) {
        QThread::sleep(s);
    }
    /**
     *  Set a thread to sleep for milliseconds
     * @param ms time in milliseconds to sleep
     **/
    static void msleep(unsigned long ms) {
        QThread::msleep(ms);
    }
    /**
     *  Set a thread to sleep for microseconds
     * @param us time in microseconds to sleep
     **/
    static void usleep(unsigned long us) {
        QThread::usleep(us);
    }
};

}

#define QGC_EVENTLOOP_DEBUG 0

#endif // QGC_H
