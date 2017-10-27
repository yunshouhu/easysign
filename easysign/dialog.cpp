//#pragma once
//#pragma execution_character_set("utf-8")
//#pragma warning (disable:4819)

#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <string>
#include <iostream>
#include <QUrl>
#include <QTextBlock>
#include <QTextCursor>
#include <QFileDialog>

#include "mythread.h"

using namespace std;

#ifdef _WIN32
#define FileSeparator "/"
#else
#define FileSeparator "/"
#endif


QString fixpath(QString input)
{
    //C:\Users\ADMINI~1\AppData\Local\Temp\tmp/temp/smali_classes2
#ifdef _WIN32
    return input.replace("/",FileSeparator);
#else
    return input.replace("\\",FileSeparator);
#endif
}


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() |Qt::WindowMinimizeButtonHint| Qt::WindowMaximizeButtonHint);//添加最大化按钮

    setWindowTitle(QString::fromLocal8Bit("APK多平台快速签名助手"));
    //Qt自适应窗口大小需要使用布局管理器：一个是控件的SizePolicy属性设置，二是一定要设置顶级布局。

    ui->textEdit->setAcceptDrops(false);//禁止textedit的默认拖拽
    //支持拖放
    setAcceptDrops(true);//https://github.com/leichaojian/qt/blob/master/drop/mainwindow.cpp

    connect(ui->pushButton_signapk,SIGNAL(clicked()),this,SLOT(OnSignAPK()));
    connect(ui->pushButton_verify_signature,SIGNAL(clicked()),this,SLOT(OnVerifySignature()));
    connect(ui->pushButton_clearLog,SIGNAL(clicked()),this,SLOT(OnClearLog()));
    connect(ui->pushButton_selectapk,SIGNAL(clicked()),this,SLOT(OnSelectAPK()));

    connect(this,SIGNAL(OnMsgSignal(int,QString)),SLOT(OnDoMsgSignal(int,QString)));
    #ifndef _WIN32
        ui->groupBox->setTitle(QString::fromLocal8Bit("Drag APK to come in to sign, put the key.pem and key.pk8 in the current directory can switch signature! Support multi APK! Support window and linux!"));
        ui->pushButton_clearLog->setText("clearLog");
        ui->pushButton_exit->setText("exit");
        ui->pushButton_signapk->setText("sign apk");
        ui->pushButton_verify_signature->setText("verify signature");
    #endif


    //信号槽机制需要注册元数据
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");



    //检查java环境

    QProcess process;
    process.start("java");
    process.waitForFinished();
    QByteArray all=process.readAllStandardError();
    if(all.isEmpty())
    {
        all=process.readAllStandardOutput();
        if(all.isEmpty())
        {
            all=process.readAllStandardError();
        }
    }
    string result=string(all.data());
    cout<<result<<endl;
    if(result.empty() || result.length()<2)
    {
        qDebug()<<QString(all)<<endl;       
#ifdef _WIN32
    QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有检测到java环境！请下载jdk并配置环境变量JAVA_HOME等!"));
#else
    QMessageBox::information(this, QString::fromLocal8Bit("提示"),
                             QString::fromLocal8Bit("Not detected in the Java environment! Please download JDK and configure the environment variable JAVA_HOME!"));
#endif
        close();
        exit(0);
    }




}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::OnDoMsgSignal(int msgType,QString line)
{
    switch (msgType) {
    case 0:
        ui->textEdit->append(line);
        break;
    case 1:
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), line);

        break;
    default:
        break;
    }

   // this->repaint();
}

void Dialog::dragEnterEvent(QDragEnterEvent *event)
{
    // QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("dragEnterEvent"));
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();

}

QString GetCorrectInput(const QString  & szInput)
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
    bool  isDoClear=true;

    //往文本框中追加文件名
    foreach(QUrl url, urls) {
        QString filename = url.toLocalFile();
        if(filename.endsWith(".apk",Qt::CaseInsensitive) ||filename.endsWith(".jar",Qt::CaseInsensitive))
        {
            if(isDoClear)
            {
                //清除上轮数据
                fileList.clear();
                isDoClear=false;
            }
            ui->textEdit->append(filename);
            fileList.append(filename);
        }else{
            qDebug()<<"忽略："<<filename<<endl;
        }
    }
    if(fileList.size()>0)
    {
        //OnSignAPK();
        MyThread* mythread=new MyThread(this);
        mythread->start();

    }
}

/**
 * @brief FuncModuleWin::copyFile
 * @param fromFIleName 优盘里面的文件
 * @param toFileName 拷贝到/bin里面的启动文件
 * @return
 */
bool copyFileFromRes(const QString& resName, const QString &toFileName)
{
    char* byteTemp = new char[4096];//字节数组
    int fileSize = 0;
    int totalCopySize = 0;
    QFile tofile;
    tofile.setFileName(toFileName);
    if(!tofile.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
        qDebug() << "open tofile failed！！！";
        return false;
    }
    QDataStream out(&tofile);

    QFile fromfile(resName);
    if(!fromfile.open(QIODevice::ReadOnly)){
        qDebug() << "open fromfile failed！！！";
        return false;
    }
    fileSize = fromfile.size();
    QDataStream in(&fromfile);

    while(!in.atEnd())
    {
        int readSize = 0;
        //读入字节数组,返回读取的字节数量，如果小于4096，则到了文件尾
        readSize = in.readRawData(byteTemp, 4096);
        out.writeRawData(byteTemp, readSize);
        totalCopySize += readSize;
    }
    // 关闭文本流：
    fromfile.flush();
    tofile.flush();

    tofile.close();
    fromfile.close();

    if(totalCopySize == fileSize){
        tofile.setPermissions(QFile::ExeUser);
        return true;
    }
    else
        return false;
}
//大部分在子线程中操作
void Dialog::OnSignAPK()
{
    //QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("OnSignAPK"));
    foreach(QString apkpath, fileList) {

        QString apkpath_abs=GetCorrectInput(apkpath);
        //查看apk签名 jarsigner -verify -verbose -certs baidusdk.apk
        QString cmd="jarsigner -verify -verbose -certs "+apkpath_abs;
        qDebug()<<cmd<<endl;
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
        process.close();
        QString  result=QString::fromLocal8Bit(all.data());
        qDebug()<<"result="<<result<<endl;
        if(result.indexOf("CertPath")!=-1)
        {
#ifdef _WIN32
             QString ret=apkpath+ QString::fromLocal8Bit(" 已经签名！");
#else
             QString ret=apkpath+ QString::fromLocal8Bit(" Already signed！");
#endif
            emit OnMsgSignal(1,ret);
            continue ;
        }
        //签名apk
        QString tmpDir =  QDir::tempPath()+FileSeparator+"sign";
        QDir(tmpDir).mkdir(tmpDir);

        QString signapk_jar_path =tmpDir+FileSeparator+"signapk.jar";
        QString key_pem_path =tmpDir+FileSeparator+"key.pem";
        QString key_pk8_path =tmpDir+FileSeparator+"key.pk8";

        //1、先获取资源文件的signapk.jar
        if(!QFile::exists(signapk_jar_path))
        {
            bool ret=copyFileFromRes(":/sign/res/signapk.jar",signapk_jar_path);
            qDebug()<<"signapk_jar_path="<<ret<<endl;
        }
        if(!QFile::exists(key_pem_path))
        {
            bool ret=copyFileFromRes(":/sign/res/key.pem",key_pem_path);
            qDebug()<<"key_pem_path="<<ret<<endl;
        }
        if(!QFile::exists(key_pk8_path))
        {
            bool ret=copyFileFromRes(":/sign/res/key.pk8",key_pk8_path);
            qDebug()<<"key_pk8_path="<<ret<<endl;
        }
        qDebug()<<"tmpDir="<<tmpDir<<endl;

        QString outputapk_path=QString(apkpath).replace(".apk","_sign.apk");
        if(apkpath.endsWith(".jar"))
        {
            outputapk_path=QString(apkpath).replace(".jar","_sign.jar");
        }
        //获取当前目录
        QString currentDir = QDir::currentPath();

        //手动切换当前目录下的签名：
        QString currentDir_key_pk8=currentDir+FileSeparator+"key.pk8";
        QString currentDir_key_pem=currentDir+FileSeparator+"key.pem";
        if(QFile::exists(currentDir_key_pk8))
        {
            key_pk8_path=currentDir_key_pk8;

#ifdef _WIN32
            emit OnMsgSignal(0,QString::fromLocal8Bit("使用当前目录签名："));
#else
           emit OnMsgSignal(0,QString::fromLocal8Bit(" Using the current directory signature："));
#endif

           emit OnMsgSignal(0,key_pk8_path);

        }
        if(QFile::exists(currentDir_key_pem))
        {
            key_pem_path=currentDir_key_pem;
#ifdef _WIN32
            emit OnMsgSignal(0,QString::fromLocal8Bit("使用当前目录签名："));
#else
           emit OnMsgSignal(0,QString::fromLocal8Bit(" Using the current directory signature："));
#endif
            emit OnMsgSignal(0,key_pem_path);
        }

        cmd="java -jar "+GetCorrectInput(signapk_jar_path)+" "+GetCorrectInput(key_pem_path)+" "+GetCorrectInput(key_pk8_path)
                +" "+apkpath_abs+" "+GetCorrectInput(outputapk_path);
        qDebug()<<cmd<<endl;
        QProcess process_java;
        process_java.start(cmd);
        process_java.waitForStarted();
        process_java.waitForFinished(-1);//30000ms= 30s  //If msecs is -1, this function will not time out.
        if(QFile::exists(outputapk_path))
        {

            qDebug()<<"成功签名生成apk"<<endl;
            //ui->textEdit->append(QString::fromLocal8Bit("签名成功!%1 ==> %2").arg(apkpath).arg(outputapk_path));
#ifdef _WIN32
           emit OnMsgSignal(0,QString::fromLocal8Bit("签名成功!%1 ==> %2").arg(apkpath).arg(outputapk_path));
#else
           emit OnMsgSignal(0,QString::fromLocal8Bit("Signature success!%1 ==> %2").arg(apkpath).arg(outputapk_path));
#endif
        }else{
            QByteArray all=process_java.readAll();
            if(all.isEmpty())
            {
                all=process.readAllStandardOutput();
                if(all.isEmpty())
                {
                    all=process.readAllStandardError();
                }
            }
            QString  cmd_result=QString::fromLocal8Bit(all.data());

#ifdef _WIN32
           QString result=QString::fromLocal8Bit("签名失败！%1 ==> %2 msg:%3").arg(apkpath).arg(outputapk_path).arg(cmd_result);
#else
           QString result=QString::fromLocal8Bit("Signature failure！%1 ==> %2 msg:%3").arg(apkpath).arg(outputapk_path).arg(cmd_result);
#endif

            qDebug()<<result<<endl;
            //ui->textEdit->append(result);
            emit OnMsgSignal(0,result);
        }
        process_java.close();
    }
    emit OnMsgSignal(0,"finish!");

}

void Dialog::OnVerifySignature()
{
    // QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("OnVerifySignature"));
    if(fileList.size()==0)
    {

#ifdef _WIN32
          QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("没有拖入apk"));
#else
         QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("Not drag into the APK"));
#endif
        return ;
    }
    foreach(QString file, fileList) {

        QString apkpath=GetCorrectInput(file);
        //QMessageBox::information(this, QString::fromLocal8Bit("提示"), apkpath);
        //查看apk签名 jarsigner -verify -verbose -certs baidusdk.apk
        QString cmd="jarsigner -verify -verbose -certs "+apkpath;

        qDebug()<<cmd<<endl;
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

        QString  result=QString::fromLocal8Bit(all.data());
        qDebug()<<"result="<<result<<endl;
        QString title=QString::fromLocal8Bit("提示:")+file;
        QMessageBox::information(this,title ,result);
    }


}

void Dialog::OnClearLog()
{
    ui->textEdit->setText("");
}

void Dialog::OnSelectAPK()
{

    QString filename;
    filename = QFileDialog::getOpenFileName(this,
                                            QString::fromLocal8Bit("选择APK文件"), "",QString::fromLocal8Bit("APK (*.apk *.jar)")); //选择路径
    if(filename.isEmpty())
    {
        return;
    }else
    {
        fileList.append(filename);
       // ui->textEdit->append(filename);
        emit OnMsgSignal(0,filename);

        if(fileList.size()>0)
        {
            MyThread* mythread=new MyThread(this);
            mythread->start();
        }

    }
    //ui->textEdit->setText("select");
}
