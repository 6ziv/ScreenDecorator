#include "Welcome.h"
#include<QApplication>
#include<QDesktopWidget>
#include<QTimer>
#include<QDebug>

bool Welcome::nativeEvent(const QByteArray &eventType, void *message,long *){
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = (MSG *)message;
        if(msg->message == WM_KEYDOWN){
            if(msg->wParam==VK_F1){
                *rtn=1;
                FromFunc=true;
                this->accept();
            }
            if(msg->wParam==VK_F8){
                *rtn=2;
                FromFunc=true;
                this->accept();
            }
            if(msg->wParam==VK_F5){
                *rtn=3;
                FromFunc=true;
                this->accept();
            }
            if(msg->wParam==VK_F12){
                *rtn=4;
                FromFunc=true;
                this->accept();
            }
        }
    }
    return false;
}

Welcome::Welcome(MainWindow *parent,int *ReturnValue) :
        QDialog(parent)
{

    FromFunc=false;
    rtn=ReturnValue;
    QRect rec = QApplication::desktop()->screenGeometry(this);
    this->setGeometry(rec.width()/3,rec.height()/3,rec.width()/3,rec.height()/3);
    this->setWindowFlag(Qt::WindowStaysOnTopHint,true);
    this->setWindowFlag(Qt::FramelessWindowHint,true);
    this->setModal(true);
    SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOPMOST);

    QPalette palette;
    this->setAutoFillBackground(true);
    QPixmap unscaledpixmap(":/image/Welcome.jpg");
    QSize picSize(this->width(),this->height());
    QPixmap pixmap = unscaledpixmap.scaled(picSize);
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);

    Timer=new QTimer(this);
    connect(Timer,SIGNAL(timeout()),this,SLOT(TimeUp()));
    Timer->setSingleShot(true);
    Timer->start(5000);

    return;
}
void Welcome::TimeUp(){
    delete Timer;
    Timer=nullptr;
    FromFunc=true;
    *rtn=3;
    this->accept();
    return;
}
void Welcome::closeEvent(QCloseEvent *){
    if(!FromFunc)
        this->reject();
}
Welcome::~Welcome()
{

    qDebug()<<"del";
    if(Timer!=nullptr)
        delete Timer;
}
