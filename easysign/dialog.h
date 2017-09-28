#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QDrag>
#include <QTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QList>
#include <QDebug>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
signals:
    void OnMsgSignal(int msgType,QString line);

public slots:
    void OnSelectAPK();
    void OnSignAPK();
    void OnVerifySignature();
    void OnClearLog();
    void OnDoMsgSignal(int msgType,QString line);
private:
    Ui::Dialog *ui;
    QList<QString> fileList;
};

#endif // DIALOG_H
