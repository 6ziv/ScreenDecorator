#ifndef WELCOME_H
#define WELCOME_H

#include "mainwindow.h"
#include<QDialog>
#include<QCloseEvent>
class Welcome : public QDialog
{
    Q_OBJECT
private:
    int *rtn;
    QTimer *Timer;
    bool FromFunc;
private slots:
    void TimeUp();
public:
    Welcome(MainWindow *parent,int *ReturnValue);
    ~Welcome();
    void closeEvent(QCloseEvent *);
    bool nativeEvent(const QByteArray &eventType, void *message,long *);
};
#endif
