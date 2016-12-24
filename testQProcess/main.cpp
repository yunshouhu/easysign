#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <string>
#include <iostream>
using namespace std;
//本cpp文件请使用utf-8格式编码(注：不是以utf-8无bom格式编码)，参考工具notepad++
void test001()
{
    QProcess pro;
    pro.start("ping");//测试linux 和window
    pro.waitForFinished();

    string str1 = "111中文";
    QByteArray all = pro.readAll();
    if(all.isEmpty())
    {
        all=pro.readAllStandardOutput();
        if(all.isEmpty())
        {
            all=pro.readAllStandardError();
        }
    }

    char* myChar = new char[all.length()];
    for(int i=0; i<all.length();i++)
    {
        myChar[i] = all[i];
    }
    string str2 = string(myChar);
    cout<<str2<<endl;//qt5 可以输出中文

    cout<<"=============="<<endl;

    cout<<string(all.data())<<endl;//qt5 可以输出中文

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

    qstr1=QString::fromLocal8Bit(all);
    qDebug()<<qstr1<<endl;  //中文显示正常，但是这里显示的是unicode编码

}

void test002()
{
    QProcess process;
    process.start("ping");
    process.waitForFinished();
    QByteArray all=process.readAll();
    if(all.isEmpty())
    {
        all=process.readAllStandardOutput();
        if(all.isEmpty())
        {
            all=process.readAllStandardError();
        }
    }

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
    if(all.isEmpty())
    {
        all=process.readAllStandardOutput();
        if(all.isEmpty())
        {
            all=process.readAllStandardError();
        }
    }

    cout<<string(all.data())<<endl;

   // QString result=QString(all);
   // qDebug()<<"result="<<result<<endl;

    QString result=QString::fromLocal8Bit(all.data());
    qDebug()<<"result2="<<result<<endl;

}
//Qt5 QProcess的使用和readAllStandardOutput()中文乱码的解决
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

   test001();
   test002();
   test003();

    QString line("你好");
    QString data=QStringLiteral(" 到底乱码还是unicode输出");
    qDebug()<<"test中文"<<line<<data<<endl;

    cout<<"test中文"<<QString(line.toUtf8()).toLocal8Bit().data()<<data.toLocal8Bit().data()<<endl;

    cout<<"test中文"<<line.toStdString()<<data.toLocal8Bit().data()<<endl;

    return a.exec();
}


