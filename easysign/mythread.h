#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include "dialog.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(Dialog *parent);

    Dialog* myDialog;

protected:
    void run();
};

#endif // MYTHREAD_H
