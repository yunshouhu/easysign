#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <QProcess>
#include <string>
#include <iostream>
using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
     ui->setupUi(this);
     setWindowFlags(windowFlags() |Qt::WindowMinimizeButtonHint| Qt::WindowMaximizeButtonHint);//添加最大化按钮
    // setWindowTitle(QString::fromUtf8("APK多平台快速签名助手"));//
     setWindowTitle(QString::fromLocal8Bit("APK多平台快速签名助手"));
     //Qt自适应窗口大小需要使用布局管理器：一个是控件的SizePolicy属性设置，二是一定要设置顶级布局。

     ui->textEdit->setAcceptDrops(false);//禁止textedit的默认拖拽
     //支持拖放
     setAcceptDrops(true);//https://github.com/leichaojian/qt/blob/master/drop/mainwindow.cpp

     connect(ui->pushButton_signapk,SIGNAL(clicked()),this,SLOT(OnSignAPK()));
     connect(ui->pushButton_verify_signature,SIGNAL(clicked()),this,SLOT(OnVerifySignature()));


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::dragEnterEvent(QDragEnterEvent *event)
{
    // QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("dragEnterEvent"));
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();

}

QString GetCrrectInput(const QString  & szInput)
{
    // 为了解决传入参数中有空格的问题
    QString szDest   = szInput;
    // 判断是否有空格
    if(szDest.indexOf(' ') < 0)
    {
        // 没有空格
        return szDest;
    }
    // 有空格,用转义符处理 //添加双引号
    szDest="\""+szInput+"\"";
    return szDest;
}

void Dialog::dropEvent(QDropEvent *event)
{
    // QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("dropEvent"));
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    fileList.clear();
    //往文本框中追加文件名
    foreach(QUrl url, urls) {
        QString filename = url.toLocalFile();
        if(filename.endsWith(".apk",Qt::CaseInsensitive) || filename.endsWith(".zip",Qt::CaseInsensitive)||filename.endsWith(".jar",Qt::CaseInsensitive))
        {
            ui->textEdit->append(filename);
            fileList.append(filename);
        }else{
            qDebug()<<"忽略："<<filename<<endl;
        }

    }
}

void Dialog::OnSignAPK()
{
     QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("OnSignAPK"));
     foreach(QString file, fileList) {

        QString apkpath=GetCrrectInput(file);
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), apkpath);
     }

     /*
     apkpath=GetCrrectInput(apkpath);
     //验证是否已经签名
     string cmd_jarsigner="jarsigner -verify -verbose -certs "+string(UNICODEconvertANSI(apkpath.GetBuffer(),CP_THREAD_ACP));
     string result_jarsigner=DoCommand(cmd_jarsigner);
     if(result_jarsigner.find("jar 已验证")!=-1)
     {
         CString ret;
         ret.Format(L"%s 已经签名！",apkpath.GetBuffer());
         AfxMessageBox(ret);
         return ;
     }*/

}

void Dialog::OnVerifySignature()
{
   // QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("OnVerifySignature"));
    if(fileList.size()==0)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有拖入apk"));
        return ;
    }
    foreach(QString file, fileList) {

       QString apkpath=GetCrrectInput(file);
       //QMessageBox::information(this, QString::fromLocal8Bit("提示"), apkpath);
       //查看apk签名 jarsigner -verify -verbose -certs baidusdk.apk
       QString cmd="jarsigner -verify -verbose -certs "+apkpath;

       qDebug()<<cmd<<endl;
       QProcess process;
       process.start("ping");
       process.waitForFinished();
       QByteArray all=process.readAll();
       cout<<string(all.data())<<endl;

      QString  result=QString::fromLocal8Bit(all.data());
       qDebug()<<"result="<<result<<endl;

       QMessageBox::information(this, QString::fromLocal8Bit("提示"),result);
    }


}
