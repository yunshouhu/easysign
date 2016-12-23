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
     setWindowFlags(windowFlags() |Qt::WindowMinimizeButtonHint| Qt::WindowMaximizeButtonHint);//�����󻯰�ť
    // setWindowTitle(QString::fromUtf8("APK��ƽ̨����ǩ������"));//
     setWindowTitle(QString::fromLocal8Bit("APK��ƽ̨����ǩ������"));
     //Qt����Ӧ���ڴ�С��Ҫʹ�ò��ֹ�������һ���ǿؼ���SizePolicy�������ã�����һ��Ҫ���ö������֡�

     ui->textEdit->setAcceptDrops(false);//��ֹtextedit��Ĭ����ק
     //֧���Ϸ�
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
    // QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("dragEnterEvent"));
    //���Ϊ�ļ�����֧���Ϸ�
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();

}

QString GetCrrectInput(const QString  & szInput)
{
    // Ϊ�˽������������пո������
    QString szDest   = szInput;
    // �ж��Ƿ��пո�
    if(szDest.indexOf(' ') < 0)
    {
        // û�пո�
        return szDest;
    }
    // �пո�,��ת������� //���˫����
    szDest="\""+szInput+"\"";
    return szDest;
}

void Dialog::dropEvent(QDropEvent *event)
{
    // QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("dropEvent"));
    //ע�⣺��������ж��ļ����ڣ���˼���û�һ�����϶��˶���ļ����������϶�һ��Ŀ¼
    //������ȡ����Ŀ¼�����ڲ�ͬ�Ĳ���ƽ̨�£��Լ���д����ʵ�ֶ�ȡ����Ŀ¼�ļ���
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    fileList.clear();
    //���ı�����׷���ļ���
    foreach(QUrl url, urls) {
        QString filename = url.toLocalFile();
        if(filename.endsWith(".apk",Qt::CaseInsensitive) || filename.endsWith(".zip",Qt::CaseInsensitive)||filename.endsWith(".jar",Qt::CaseInsensitive))
        {
            ui->textEdit->append(filename);
            fileList.append(filename);
        }else{
            qDebug()<<"���ԣ�"<<filename<<endl;
        }

    }
}

void Dialog::OnSignAPK()
{
     QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("OnSignAPK"));
     foreach(QString file, fileList) {

        QString apkpath=GetCrrectInput(file);
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), apkpath);
     }

     /*
     apkpath=GetCrrectInput(apkpath);
     //��֤�Ƿ��Ѿ�ǩ��
     string cmd_jarsigner="jarsigner -verify -verbose -certs "+string(UNICODEconvertANSI(apkpath.GetBuffer(),CP_THREAD_ACP));
     string result_jarsigner=DoCommand(cmd_jarsigner);
     if(result_jarsigner.find("jar ����֤")!=-1)
     {
         CString ret;
         ret.Format(L"%s �Ѿ�ǩ����",apkpath.GetBuffer());
         AfxMessageBox(ret);
         return ;
     }*/

}

void Dialog::OnVerifySignature()
{
   // QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("OnVerifySignature"));
    if(fileList.size()==0)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("û������apk"));
        return ;
    }
    foreach(QString file, fileList) {

       QString apkpath=GetCrrectInput(file);
       //QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), apkpath);
       //�鿴apkǩ�� jarsigner -verify -verbose -certs baidusdk.apk
       QString cmd="jarsigner -verify -verbose -certs "+apkpath;

       qDebug()<<cmd<<endl;
       QProcess process;
       process.start("ping");
       process.waitForFinished();
       QByteArray all=process.readAll();
       cout<<string(all.data())<<endl;

      QString  result=QString::fromLocal8Bit(all.data());
       qDebug()<<"result="<<result<<endl;

       QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),result);
    }


}
