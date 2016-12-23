#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <string>
#include <iostream>
using namespace std;

void test001()
{
    QProcess pro;
    pro.start("ping.exe");
    pro.waitForFinished();

    string str1 = "111中文";
    QByteArray qba = pro.readAllStandardOutput();
    char* myChar = new char[qba.length()];
    for(int i=0; i<qba.length();i++)
    {
        myChar[i] = qba[i];
    }
    string str2 = string(myChar);
    cout<<str2<<endl;//qt5 可以输出中文

    cout<<"=============="<<endl;

    cout<<string(qba.data())<<endl;//qt5 可以输出中文

    cout<<"=============="<<endl;
    cout<<str1<<endl;
    delete []myChar;
    QString qstr1=QString::fromStdString(str1);
    qDebug()<<qstr1<<endl;  //中文乱码

    qstr1=QString::fromLocal8Bit(str1.c_str());
    qDebug()<<qstr1<<endl;  //中文显示正常，但是这里显示的是unicode编码

    qstr1=QString::fromStdString(str2);
    qDebug()<<qstr1<<endl;  //中文乱码

    qstr1=QString::fromLocal8Bit(str2.c_str());
    qDebug()<<qstr1<<endl;  //中文显示正常，但是这里显示的是unicode编码

    qstr1=QString::fromLocal8Bit(qba);
    qDebug()<<qstr1<<endl;  //中文显示正常，但是这里显示的是unicode编码

}

void test002()
{
    QProcess process;
    process.start("ping.exe");
    process.waitForFinished();
    QByteArray all=process.readAll();

    cout<<string(all.data())<<endl;

    QString result=QString(all);
    qDebug()<<"result="<<result<<endl;
    result=QString::fromLocal8Bit(all.data());
    qDebug()<<"result2="<<result<<endl;

}


void test003()
{

    QString apkpath="d:/mlgx.apk";
    QString cmd="jarsigner -verify -verbose -certs "+apkpath;
    qDebug()<<"jarsigner "<<cmd<<endl;

    QProcess process;
    process.start(cmd);
    process.waitForFinished();
    QByteArray all=process.readAll();

    cout<<string(all.data())<<endl;

    QString result=QString(all);
    qDebug()<<"result="<<result<<endl;

    result=QString::fromLocal8Bit(all.data());
    qDebug()<<"result2="<<result<<endl;

}
//Qt5 QProcess的使用和readAllStandardOutput()中文乱码的解决
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    test003();


    return a.exec();
}


