#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <windows.h>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#define DEF_OPT_SELF -1
#define DEF_OPT_OPACITY 0
#define DEF_OPT_MODE 1
#define DEF_OPT_FILE  2

#define DEF_MOD_LOCKED 1
#define DEF_MOD_SILENT 2
#define DEF_MOD_TIMER  4

typedef struct conf{
    QString ImagePath;
    int Opacity;
    int Mode;
    bool IsScreenSaver;
    bool StartWithWindows;
}conf;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void closeEvent(QCloseEvent * e);
    ~MainWindow();
    int GetOpacity();
    int GetMode();
    QString GetImageFile();
    bool IsScreenSaver();
    bool IsStartup();
    void Do_Setting(conf *val,bool run);
private:
    ATOM atomSH;
    ATOM atomPlus;
    ATOM atomMinus;
    ATOM atomBye;
    HANDLE hMutex;
    QString ImagePath;
    QTimer *timer1;
    int mode;
    int opacity;
    bool blocked;
    bool running;
    double tmpopacity;
    void Start();
    void Add();
    void Remove();
    void install();
    void uninstall();
    void Usage();
    void SetConfig(int option,void* value);
    void LoadConfig();
    void BroadcastWindowHWND();
    HMODULE dll;
private slots:
    void refresh();
    void act_timer1();
};

#endif
// MAINWINDOW_H
