#ifndef WINDOW_H
#define WINDOW_H


#include <QWindow>

class Window : public QWindow
{
    Q_OBJECT
public:
    Window(QScreen *screen = 0);
    ~Window();

};

#endif // WINDOW_H
