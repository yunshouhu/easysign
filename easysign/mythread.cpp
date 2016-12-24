#include "mythread.h"
#include <QDebug>
MyThread::MyThread(Dialog *parent)
{

    this->myDialog=parent;
}


void MyThread::run()
{
    myDialog->OnSignAPK();
    qDebug()<<"run finish\n";
    //myDialog->appendLog("finish!");

}
