#ifndef _QEXTSERIALPORT_P_H_
#define _QEXTSERIALPORT_P_H_

#include "qextserialport.h"
#include <QtCore/QReadWriteLock>

#  include <QtCore/qt_windows.h>

#include <stdlib.h>

// This is QextSerialPort's read buffer, needed by posix system.
// ref: QRingBuffer & QIODevicePrivateLinearBuffer
class QextReadBuffer
{
public:
    inline QextReadBuffer(size_t growth=4096)
        : len(0), first(0), buf(0), capacity(0), basicBlockSize(growth) {
    }

    ~QextReadBuffer() {
        delete [] buf;
    }

    inline void clear() {
        first = buf;
        len = 0;
    }

    inline int size() const {
        return len;
    }

    inline bool isEmpty() const {
        return len == 0;
    }

    inline int read(char *target, int size) {
        int r = qMin(size, len);
        if (r == 1) {
            *target = *first;
            --len;
            ++first;
        } else {
            memcpy(target, first, r);
            len -= r;
            first += r;
        }
        return r;
    }

    inline char *reserve(size_t size) {
        if ((first - buf) + len + size > capacity) {
            size_t newCapacity = qMax(capacity, basicBlockSize);
            while (newCapacity < len + size)
                newCapacity *= 2;
            if (newCapacity > capacity) {
                // allocate more space
                char *newBuf = new char[newCapacity];
                memmove(newBuf, first, len);
                delete [] buf;
                buf = newBuf;
                capacity = newCapacity;
            } else {
                // shift any existing data to make space
                memmove(buf, first, len);
            }
            first = buf;
        }
        char *writePtr = first + len;
        len += (int)size;
        return writePtr;
    }

    inline void chop(int size) {
        if (size >= len)
            clear();
        else
            len -= size;
    }

    inline void squeeze() {
        if (first != buf) {
            memmove(buf, first, len);
            first = buf;
        }
        size_t newCapacity = basicBlockSize;
        while (newCapacity < size_t(len))
            newCapacity *= 2;
        if (newCapacity < capacity) {
            char *tmp = static_cast<char *>(realloc(buf, newCapacity));
            if (tmp) {
                buf = tmp;
                capacity = newCapacity;
            }
        }
    }

    inline QByteArray readAll() {
        char *f = first;
        int l = len;
        clear();
        return QByteArray(f, l);
    }

    inline int readLine(char *target, int size) {
        int r = qMin(size, len);
        char *eol = static_cast<char *>(memchr(first, '\n', r));
        if (eol)
            r = 1+(eol-first);
        memcpy(target, first, r);
        len -= r;
        first += r;
        return int(r);
    }

    inline bool canReadLine() const {
        return memchr(first, '\n', len);
    }

private:
    int len;
    char *first;
    char *buf;
    size_t capacity;
    size_t basicBlockSize;
};

class QWinEventNotifier;
class QReadWriteLock;
class QSocketNotifier;

class QextSerialPortPrivate
{
    Q_DECLARE_PUBLIC(QextSerialPort)
public:
    QextSerialPortPrivate(QextSerialPort *q);
    ~QextSerialPortPrivate();
    enum DirtyFlagEnum
    {
        DFE_BaudRate = 0x0001,
        DFE_Parity = 0x0002,
        DFE_StopBits = 0x0004,
        DFE_DataBits = 0x0008,
        DFE_Flow = 0x0010,
        DFE_TimeOut = 0x0100,
        DFE_ALL = 0x0fff,
        DFE_Settings_Mask = 0x00ff //without TimeOut
    };
    mutable QReadWriteLock lock;
    QString port;
    PortSettings settings;
    QextReadBuffer readBuffer;
    int settingsDirtyFlags;
    ulong lastErr;
    QextSerialPort::QueryMode queryMode;

    // platform specific members

    HANDLE handle;
    OVERLAPPED overlap;
    COMMCONFIG commConfig;
    COMMTIMEOUTS commTimeouts;
    QWinEventNotifier *winEventNotifier;
    DWORD eventMask;
    QList<OVERLAPPED *> pendingWrites;
    QReadWriteLock *bytesToWriteLock;


    /*fill PortSettings*/
    void setBaudRate(BaudRateType baudRate, bool update=true);
    void setDataBits(DataBitsType dataBits, bool update=true);
    void setParity(ParityType parity, bool update=true);
    void setStopBits(StopBitsType stopbits, bool update=true);
    void setFlowControl(FlowType flow, bool update=true);
    void setTimeout(long millisec, bool update=true);
    void setPortSettings(const PortSettings &settings, bool update=true);

    void platformSpecificDestruct();
    void platformSpecificInit();
    void translateError(ulong error);
    void updatePortSettings();

    qint64 readData_sys(char *data, qint64 maxSize);
    qint64 writeData_sys(const char *data, qint64 maxSize);
    void setDtr_sys(bool set=true);
    void setRts_sys(bool set=true);
    bool open_sys(QIODevice::OpenMode mode);
    bool close_sys();
    bool flush_sys();
    ulong lineStatus_sys();
    qint64 bytesAvailable_sys() const;

    void _q_onWinEvent(HANDLE h);

    void _q_canRead();

    QextSerialPort *q_ptr;
};

#endif //_QEXTSERIALPORT_P_H_
