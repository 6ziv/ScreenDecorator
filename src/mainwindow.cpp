#include "mainwindow.h"
#include "SettingDialog.h"
#include "Welcome.h"
#include <QApplication>

#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include <QCloseEvent>
#include <windows.h>
#include <io.h>
#include <errno.h>
HMODULE hmod;
HWND hwindow;
HHOOK hhk;
HHOOK hhk1;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp)
{
    WPARAM wparam = (WPARAM)0x0683;
    LPARAM lparam = (LPARAM)0x5747;
    PostMessage(hwindow, WM_APP + 0x0518, wparam, lparam);
    return CallNextHookEx(hhk, code, wp, lp);
}
LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp)
{
    WPARAM wparam = (WPARAM)0x0683;
    LPARAM lparam = (LPARAM)0x5747;
    PostMessage(hwindow, WM_APP + 0x0518, wparam, lparam);
    return CallNextHookEx(hhk1, code, wp, lp);
}

#ifdef __cplusplus
extern "C"
{
#endif
void HookStart(){
    hhk = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hmod, 0);
    hhk1 = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hmod, 0);
}
void HookStop()
{
    if (hhk)
        UnhookWindowsHookEx(hhk);
    if (hhk1)
        UnhookWindowsHookEx(hhk1);
}

#ifdef __cplusplus
}
#endif
bool MainWindow::IsScreenSaver(){
     wchar_t syspath[2048];
     GetSystemDirectoryW(syspath,2048);
     wcscat_s(syspath,2048,L"\\Screen_Decorator.scr");
    if(_waccess(syspath,F_OK)==0)
        return true;
    else
        return false;
}
bool MainWindow::IsStartup(){
    HKEY hKey;
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);
    int err=RegQueryValueExW(hKey,TEXT("ScreenDecorator"),NULL,NULL,NULL,NULL);
    if(err==ERROR_SUCCESS)return true;else return false;
}
void MainWindow::Do_Setting(conf *val,bool run=false){

    qDebug()<<"Saving Config...";
    qDebug()<<val->Mode;
    qDebug()<<val->Opacity;
    qDebug()<<val->ImagePath;
    wchar_t Ctmp[2048];
    memset(Ctmp,0,sizeof(Ctmp));
    val->ImagePath.toWCharArray(Ctmp);
    SetConfig(DEF_OPT_FILE,Ctmp);

    int Itmp;
    Itmp=val->Mode;
    SetConfig(DEF_OPT_MODE,&Itmp);
    Itmp=val->Opacity;
    SetConfig(DEF_OPT_OPACITY,&Itmp);
    if(val->StartWithWindows!=this->IsStartup())
    {
        if(val->StartWithWindows)
            install();
        else
            uninstall();
    }
    if(val->IsScreenSaver!=this->IsScreenSaver())
    {
        if(val->IsScreenSaver){
           Add();
        }else{
           Remove();
        }
    }
    if(run){
        LoadConfig();
        Start();
    }else
        this->close();
    return;
}
int MainWindow::GetOpacity(){
    return opacity;
}
int MainWindow::GetMode(){
    return mode;
}
QString MainWindow::GetImageFile(){
    return ImagePath;
}

void MainWindow::BroadcastWindowHWND(){
    HANDLE hfile = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(HWND),TEXT("My_Screen_decorator_95819"));
    if(NULL==hfile)
    {
        return;
    }

    PVOID pview = NULL;
    pview = ::MapViewOfFile(hfile,FILE_MAP_ALL_ACCESS,0,0,sizeof(HWND));
    if (NULL==pview)
    {
        return;
    }
    HWND window=(HWND)this->winId();
    hwindow=(HWND)this->winId();
    memcpy_s(pview,sizeof(HWND),&window,sizeof(HWND));
    return;
}
char *ContainAsPrefix(char *a,char *b){

    char *tmpa;
    char *tmpb;
    tmpa=a;tmpb=b;
    while(((*tmpa)=='\t')||((*tmpa)==' '))tmpa++;
    while(*tmpb){
        if(*tmpa==*tmpb){
            tmpa++;
            tmpb++;
        }else{
            return 0;
        }
    }
    while(((*tmpa)=='\t')||((*tmpa)==' '))tmpa++;
    return tmpa;
}
void MainWindow::Usage(){
    MessageBoxW(0,L"快捷键列表：\n\tctrl+Up 增加清晰度\n\tctrl+Down 减小清晰度\n\tctrl+Q 退出程序\n\tctrl+H 显示/隐藏\n\n隐藏状态下仅ctrl+H有效\n如果在非交互模式运行，以上热键均不起作用。\n",L"一个假的Usage",MB_OK);
    MessageBoxW(0,L"开关列表：\n\t交互运行：允许使用快捷键\n\t静默运行：程序退出时无须确认\n\t循环前置：设置一个定时器，以1s的间隔将窗口前置\n",L"一个假的Usage",MB_OK);
    MessageBoxW(0,L"命令行参数：\n\t无参数：启动欢迎界面。\n\t-a： 安装，作为屏幕保护程序。\n\t-d：删除，从屏幕保护程序列表。\n\t-f <file> ：更改背景图片（您设置的必须是允许的图片格式）\n\t-i：添加至开机自启动项目。\n\t-m <mode> 设置运行模式。\n\t\t其中，<mode>应当为以下值中若干个的和：\n\t\t\t1：关闭交互模式\n\t\t\t2：静默模式\n\t\t\t4：循环前置\n\t-o <opacity>：设置启动时的透明度。（0-99）\n\t\t0为完全透明\n\t-q：退出当前运行的全屏进程\n\t-r:直接启动\n\t-s：直接作为屏幕保护启动\n\t-u：从开机启动项中移除\n",L"一个假的Usage",MB_OK);

    this->close();
    return;
}
void MainWindow::SetConfig(int option,void* value){
    HKEY hKey=NULL;
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\ScreenDecorator", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);
    if(option==DEF_OPT_SELF)
        RegSetValueExW(hKey,TEXT("Exe"),0,REG_SZ,(LPBYTE)value,wcslen((wchar_t *)value)*sizeof(wchar_t));
    if(option==DEF_OPT_OPACITY)
        RegSetValueExW(hKey,TEXT("Opacity"),0,REG_DWORD,(LPBYTE)value,sizeof(int));
    if(option==DEF_OPT_MODE)
        RegSetValueExW(hKey,TEXT("Mode"),0,REG_DWORD,(LPBYTE)value,sizeof(int));
    if(option==DEF_OPT_FILE)
        RegSetValueExW(hKey,TEXT("File"),0,REG_SZ,(LPBYTE)value,wcslen((wchar_t *)value)*sizeof(wchar_t));

    return;
}
void MainWindow::LoadConfig(){

    //Load default first
    ImagePath=":/image/U.jpg";
    opacity=20;
    mode=0;

    HKEY hKey;
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\ScreenDecorator", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);

    qDebug()<<"loading";
    long unsigned int len;
    len=sizeof(int);
    long unsigned int type;
    int tmp_opacity=-0;
    RegQueryValueExW(hKey,TEXT("Opacity"),0,(LPDWORD)(&type),(LPBYTE)&tmp_opacity,(LPDWORD)(&len));
    if((tmp_opacity>=0)&&(tmp_opacity<=100)&&(type==REG_DWORD))opacity=tmp_opacity;
    int tmp_mode=-1;
    RegQueryValueExW(hKey,TEXT("Mode"),0,(LPDWORD)(&type),(LPBYTE)&tmp_mode,(LPDWORD)(&len));
    qDebug()<<"tmpmode="<<tmp_mode;
    if((tmp_mode>=0)&&(tmp_mode<=7)&&(type==REG_DWORD))mode=tmp_mode;

    wchar_t tmp[4096];
    len=4096;
    qDebug()<<"error:"<<RegQueryValueExW(hKey,TEXT("File"),0,(LPDWORD)(&type),(LPBYTE)tmp,(LPDWORD)(&len));
    QString Qtmp;
    Qtmp=QString::fromWCharArray(tmp);
    qDebug()<<"file="<<Qtmp;
    QPixmap tmppixmap;
    if(tmppixmap.load(Qtmp.simplified())&&(type==REG_SZ))
        ImagePath=Qtmp.simplified();
    qDebug()<<"Success Loading";
    return;

}

void MainWindow::install(){

    static wchar_t Me[1024];
    GetModuleFileNameW(NULL, Me, 1024);

    HKEY hKey=NULL;
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);
    RegSetValueExW(hKey,TEXT("ScreenDecorator"),0,REG_SZ,(LPBYTE)Me,sizeof(wchar_t)*wcslen(Me));

    if (hKey != NULL)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }
    return;
}
void MainWindow::uninstall(){
    HKEY hKey=NULL;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,(KEY_SET_VALUE), &hKey);

    RegDeleteValueW(hKey,TEXT("ScreenDecorator"));

    if (hKey != NULL)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }
    return;
}

void MainWindow::act_timer1(){
    if(this->windowOpacity()+0.005<=1)
        this->setWindowOpacity(this->windowOpacity()+0.005);
    else
    {
        this->setWindowOpacity(1);
        timer1->stop();
    //    running=false;
    }
    return;
}
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = (MSG *)message;
        if((msg->message == WM_APP+0x0518)&&(msg->wParam==0x5747)&&(msg->lParam==0x0683)){
            if(!running){
                running=true;
                timer1->start(100);
                tmpopacity=this->windowOpacity();
                HookStart();
            }
        }

        if((msg->message == WM_APP+0x0518)&&(msg->lParam==0x5747)&&(msg->wParam==0x0683)){
            if(running){
                running=false;
                timer1->stop();
                this->setWindowOpacity(tmpopacity);
                HookStop();
            }
        }

        if((msg->message == WM_APP+0x0518)&&(msg->lParam==0x8674)&&(msg->wParam==0x7498)){
            printf("OK,Bye\n");
            this->close();
            return true;
        }

        //When losing focus, get it back.
        if (msg->message == WM_KILLFOCUS)
            this->raise();

        //Hot key pressed:
        if (msg->message == WM_HOTKEY)
        {
            qDebug()<<"HotKey";
            //During messagebox: hotkeys blocked
            if(blocked){
                return false;
            }

            //Show/Hide
            if(msg->wParam==atomSH){
                if(this->isVisible()){

                    //Disable other hotkeys

                    UnregisterHotKey((HWND)this->winId(), atomPlus);
                    UnregisterHotKey((HWND)this->winId(), atomMinus);
                    UnregisterHotKey((HWND)this->winId(), atomBye);

                    //Hide the window
                    this->hide();
                }else {

                    //Enable other hotkeys
                    RegisterHotKey((HWND)this->winId(), atomPlus, MOD_CONTROL,  VK_UP);
                    RegisterHotKey((HWND)this->winId(), atomMinus, MOD_CONTROL,  VK_DOWN);
                    RegisterHotKey((HWND)this->winId(), atomBye, MOD_CONTROL,  0x51);

                    //Show the window
                    this->show();
                }
                return true;
            }else

            //This paragraph is useless.
            if(!this->isVisible()){
                return false;
            }
            else{

            //Less transparent
            if(msg->wParam==atomPlus){
                if(windowOpacity()<0.98)this->setWindowOpacity(this->windowOpacity()+0.02);
                return true;
            }else

            //More transparent
            if(msg->wParam==atomMinus){
                if(windowOpacity()>0.02)this->setWindowOpacity(this->windowOpacity()-0.02);
                return true;
            }else

            //Exit the application
            if(msg->wParam==atomBye){
                //Wait for confirm
                blocked=true;

                    //If silent mode is on, require no confirm.
                    if(mode&DEF_MOD_SILENT){
                        blocked=false;

                         //goto the destruction
                        this->close();
                        return true;
                    }
                if(MessageBoxA((HWND)this->winId(), "Maybe you just need to hide it with ctrl+H. If so, you can show it by pressing ctrl+H again.\n Press 'Yes' to quit the program and 'No' to resume.","Are You Sure To Quit?", MB_YESNO)==IDYES){
                    blocked=false;

                    //goto the destruction
                    this->close();
                    return true;
                }
                else
                    blocked=false;
            }else
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;

    //Useless one. But I hate seeing warnings, so I write this.
    return false;
}
void MainWindow::refresh(){
    if(blocked)return;
    this->raise();
    return;
}
void MainWindow::Start()
{
    qDebug()<<"start";
    //Avoid multiple runs
    hMutex = NULL;
    hMutex = ::CreateMutex(NULL, FALSE, TEXT("TestMutex"));
    DWORD dwRet=GetLastError();
    if (hMutex)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            if((mode&DEF_MOD_SILENT)==0)
                MessageBoxA(0,"It seems that already a copy of ScreenDecorator is running.","Error",MB_OK);

                //Exitting the Application.
                this->close();
                return;
        }
    }
    else
    {
        if((mode&DEF_MOD_SILENT)==0)
            MessageBoxA(0,"An unexpected error occurred.","Error",MB_OK);

        //Exitting the Application
        this->close();
        return;
    }

    BroadcastWindowHWND();



    //All hotkeys available at first
    blocked=false;

    //register hotkeys
    atomSH = GlobalAddAtom(TEXT("ExitFloatedForegroundShowHide"));
    atomPlus = GlobalAddAtom(TEXT("ExitFloatedForegroundPlus"));
    atomMinus = GlobalAddAtom(TEXT("ExitFloatedForegroundMinus"));
    atomBye = GlobalAddAtom(TEXT("ExitFloatedForegroundBye"));

    if((mode&DEF_MOD_LOCKED)==0){

        RegisterHotKey((HWND)this->winId(), atomSH, MOD_CONTROL,  0x48);
        RegisterHotKey((HWND)this->winId(), atomPlus, MOD_CONTROL,  VK_UP);
        RegisterHotKey((HWND)this->winId(), atomMinus, MOD_CONTROL,  VK_DOWN);
        RegisterHotKey((HWND)this->winId(), atomBye, MOD_CONTROL,  0x51);
    }


    //Setting the style of the window
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    this->setWindowFlags(this->windowFlags()|Qt::Tool|Qt::WindowStaysOnTopHint);
    this->showFullScreen();
    this->setWindowOpacity((float)opacity/100);

    SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST);

    //Drawing the image
    qDebug()<<ImagePath;
    QPalette palette;
    this->setAutoFillBackground(true);
    QPixmap unscaledpixmap(ImagePath);
    QSize picSize(this->width(),this->height());
    QPixmap pixmap = unscaledpixmap.scaled(picSize);
    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);

    //Bring to front
    this->raise();
    if(mode&DEF_MOD_TIMER){
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(1000);
    }
    qDebug()<<"fin";
    return;
}
void MainWindow::Add(){
    static wchar_t Me[2048];
    GetModuleFileNameW(NULL, Me, 2048);
    SetConfig(DEF_OPT_SELF,Me);
    wchar_t syspath[2048];
    GetSystemDirectoryW(syspath,2048);
    wcscat_s(syspath,2048,L"\\Screen_Decorator.scr");
    FILE *file=_wfopen(syspath,L"w+");
    if(file==0) {
        if(errno==EACCES) {
            fclose(file);
            ShellExecuteW(0,L"runas",Me,L"-a",NULL,SW_SHOWNORMAL);
        }
        this->close();
        return;
    }
    fclose(file);
    char tmp[65536];
    QFile in(":/others/ScreenDecorator.scr");
    in.open(QIODevice::ReadOnly);
    QDataStream instr(&in);
    QFile out(QString::fromWCharArray(syspath));
    out.open(QIODevice::WriteOnly);
    QDataStream outstr(&out);
    int len=instr.readRawData(tmp,65536);
    outstr.writeRawData(tmp,len);
    in.close();
    out.close();
    return;
}
void MainWindow::Remove(){
    wchar_t mypath[2048];
    wchar_t syspath[2048];
    GetModuleFileNameW(NULL,mypath,2048);
    GetSystemDirectoryW(syspath,2048);
    wcscat_s(syspath,2048,L"\\Screen_Decorator.scr");
    if(_waccess(syspath,F_OK)){
        this->close();
        return;
    }
    int rtn=_wremove(syspath);
    if(rtn) {
        if(errno==EACCES) {
            ShellExecuteW(0,L"runas",mypath,L"-d",NULL,SW_SHOWNORMAL);
        }
    }
    return;
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->setAttribute(Qt::WA_DeleteOnClose,true);
    this->setAttribute(Qt::WA_QuitOnClose,true);

    running=false;
    timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(act_timer1()));

    LoadConfig();

    QStringList arguments = QCoreApplication::arguments();
    QString myaddr = arguments.at(0);
    //Have no arguments
    if(arguments.size()<2){

        int ret=-1;
        Welcome *welcome=new Welcome(this,&ret);
        int OK=welcome->exec();
        delete welcome;
        if(OK==QDialog::Rejected)
            this->close();
        qDebug()<<"Returning "<<ret;

        if(ret==1){
            Usage();
        }else
        if(ret==2){
            int rtn;
            SettingDialog dlg(this);
            rtn=dlg.exec();
            if(rtn!=QDialog::Accepted)
                this->close();

        }else
        if(ret==3){
            LoadConfig();
            Start();
        }else
        if(ret==4){
            HANDLE hfile = ::OpenFileMapping(FILE_MAP_READ,FALSE,TEXT("My_Screen_decorator_95819"));
            if(NULL==hfile) {
                this->close();
                return;
            }

            PVOID pview = NULL;
            pview = ::MapViewOfFile(hfile,FILE_MAP_READ,0,0,sizeof(HWND));
            if (NULL==pview) {
                this->close();
                return;
            }
            HWND window;
            memcpy(&window,pview,sizeof(HWND));
            WPARAM wparam=(WPARAM)0x7498;
            LPARAM lparam=(LPARAM)0x8674;
            PostMessage(window,WM_APP+0x0518,wparam,lparam);
            this->close();
            return;
        }else
        {
            this->close();
        }
    }
    else{

    //Check the first parameter
    QString option = arguments.at(1);
    if(arguments.size()==2){

    //'-q' to quit the running application
    if(QString::compare(option.trimmed(),"-q",Qt::CaseInsensitive)==0){
        HANDLE hfile = ::OpenFileMapping(FILE_MAP_READ,FALSE,TEXT("My_Screen_decorator_95819"));
        if(NULL==hfile) {
            this->close();
            return;
        }

        PVOID pview = NULL;
        pview = ::MapViewOfFile(hfile,FILE_MAP_READ,0,0,sizeof(HWND));
        if (NULL==pview) {
            this->close();
            return;
        }
        HWND window;
        memcpy(&window,pview,sizeof(HWND));
        WPARAM wparam=(WPARAM)0x7498;
        LPARAM lparam=(LPARAM)0x8674;
        PostMessage(window,WM_APP+0x0518,wparam,lparam);
        this->close();
        return;

    }else

    //'-r' to run
    if(QString::compare(option.trimmed(),"-r",Qt::CaseInsensitive)==0){
        Start();
    }else

    //'-s' to start as a screensaver
    if(QString::compare(option.trimmed(),"-s",Qt::CaseInsensitive)==0){
        Start();
        running=true;
        timer1->start(100);
        tmpopacity=this->windowOpacity();
        printf("Going to work..");

        HookStart();
        printf("MaybeDone?");
    }else

    //'-a' to add to screen savers
    if(QString::compare(option.trimmed(),"-a",Qt::CaseInsensitive)==0){
        Add();
        ShellExecuteW(0,L"open",L"rundll32.exe",L"shell32.dll,Control_RunDLL desk.cpl,screensaver,@screensaver",NULL,SW_SHOWNORMAL);
        this->close();
        return;
    }else

    //'-d' to delete from screen savers
    if(QString::compare(option.trimmed(),"-d",Qt::CaseInsensitive)==0){
        Remove();
        this->close();
        return;
    }else

    //'-i' for installing to startup
    if(QString::compare(option.trimmed(),"-i",Qt::CaseInsensitive)==0){
        install();
        this->close();
    }else

    //'-u' for uninstalling from startup
    if(QString::compare(option.trimmed(),"-u",Qt::CaseInsensitive)==0){
        uninstall();
        this->close();
    }else{

        //parameter not known
        Usage();
    }

    }else

    if(arguments.size()==3){

    QString para=arguments.at(2);

    //'-f' for configuring a file to show
    if(QString::compare(option.trimmed(),"-f",Qt::CaseInsensitive)==0){
        wchar_t tmp[2048];
        memset(tmp,0,sizeof(tmp));

        para.toWCharArray(tmp);
        QImage tmpImg;
        QString tmpstr;
        tmpstr=QString::fromWCharArray(tmp);
        if(tmpImg.load(tmpstr))
            SetConfig(DEF_OPT_FILE,&tmp);
        this->close();
        return;
    }else

    //'-o' for configuring the default opacity
    if(QString::compare(option.trimmed(),"-o",Qt::CaseInsensitive)==0){
        bool rtn;
        int m_opacity=para.toInt(&rtn,10);
        if(rtn){
            if(m_opacity<100){
                SetConfig(DEF_OPT_OPACITY,&m_opacity);
            }else
                Usage();
        }
        else
            Usage();
        this->close();
        return;
    }else

    //'-m' for configuring the startup mode
    if(QString::compare(option.trimmed(),"-m",Qt::CaseInsensitive)==0){
        bool rtn;
        int m_mode=para.toInt(&rtn,10);
        if(rtn){
            if(m_mode<8){
                SetConfig(DEF_OPT_MODE,&m_mode);

            }else
                Usage();
        }
        else
            Usage();
        this->close();
        return;
    }else{

        //parameter not known
        Usage();
    }

    }else{

        //parameters more than 3
        Usage();
    }
    }
    return;
}
void MainWindow::closeEvent(QCloseEvent *e){
    //Cleaning up...
    printf("Maybe I am closing\n");
    CloseHandle(hMutex);
    if(mode&&DEF_MOD_LOCKED==0){
            UnregisterHotKey((HWND)this->winId(),atomSH);
        if(this->isVisible())
            UnregisterHotKey((HWND)this->winId(),atomPlus);
            UnregisterHotKey((HWND)this->winId(),atomMinus);
            UnregisterHotKey((HWND)this->winId(),atomBye);
    }
    GlobalDeleteAtom(atomSH);
    GlobalDeleteAtom(atomPlus);
    GlobalDeleteAtom(atomMinus);
    GlobalDeleteAtom(atomBye);

    e->accept();
    delete this;
    return;
}

MainWindow::~MainWindow()
{
    exit(0);
    return;
}
