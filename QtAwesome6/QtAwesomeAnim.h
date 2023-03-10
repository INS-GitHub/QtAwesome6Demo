#ifndef QTAWESOMEANIMATION_H
#define QTAWESOMEANIMATION_H

#if __cplusplus <= 199711L
#define QTAWESOME_NULL NULL
#else
#define QTAWESOME_NULL nullptr
#endif

#include <QObject>

class QPainter;
class QRect;
class QTimer;
class QWidget;

class QtAwesomeAnimation : public QObject
{
    Q_OBJECT

public:
    QtAwesomeAnimation( QWidget* parentWidget, int interval = 10, int step = 1);

    void setup( QPainter& painter, const QRect& rect );

public slots:
    void update();

private:
    QWidget* parentWidgetRef_;
    QTimer* timer_;
    int interval_;
    int step_;
    float angle_;

};

#endif // QTAWESOMEANIMATION_H
