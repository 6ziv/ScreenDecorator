#include "SettingDialog.h"
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QButtonGroup>
#include<QFileDialog>
#include<QDebug>
#include<QIcon>
FileNameEdit::FileNameEdit(QWidget *){
}

FileNameEdit::~FileNameEdit(){
}

void FileNameEdit::mouseReleaseEvent(QMouseEvent *){

    QString temp=QFileDialog::getOpenFileName(this, tr("open file"), " ",  tr("Allfile(*.*)"),nullptr,QFileDialog::ReadOnly);
    if(temp.length()==0)
        return;

    QImage *imgtemp=new QImage;
    if(!(imgtemp->load(temp)))
        return;
    delete imgtemp;

    ptrparent->Str_ImageFile=temp;
    if(QString::compare(ptrparent->Str_ImageFile,":/image/U.jpg",Qt::CaseSensitive)==0)
        ptrparent->ImageFile->setText(QString::fromWCharArray(L"默认图像"));
    else
        ptrparent->ImageFile->setText(ptrparent->Str_ImageFile);

    return;
}

void SettingDialog::Default(bool){

    this->Str_ImageFile=":/images/U.jpg";
    ImageFile->setText(QString::fromWCharArray(L"默认图像"));
    return;
}

void SettingDialog::Generate_Settings(){

    Settings.ImagePath=Str_ImageFile;
    Settings.IsScreenSaver=InstallAsScreenSaver->isChecked();
    Settings.StartWithWindows=StartWithWindows->isChecked();

    int mode;
    mode=0;

    if(RunSilently->isChecked())mode+=DEF_MOD_SILENT;
    if(GoodPerformance->isChecked())mode+=DEF_MOD_TIMER;
    if(!AllowHotKeys->isChecked())mode+=DEF_MOD_LOCKED;
    qDebug()<<mode<<"=";

    Settings.Mode=mode;
    Settings.Opacity=Opacity->value();
    qDebug()<<Settings.Mode;
    return;
}

SettingDialog::SettingDialog(MainWindow *parent) :
        QDialog(parent)
{

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    My_Parent=(MainWindow *)parent;

    Settings.Opacity=My_Parent->GetOpacity();
    Settings.Mode=My_Parent->GetMode();
    Settings.ImagePath=My_Parent->GetImageFile();
    Settings.StartWithWindows=My_Parent->IsStartup();
    Settings.IsScreenSaver=My_Parent->IsScreenSaver();

    QVBoxLayout *pLayout = new QVBoxLayout();

    StartWithWindows = new QCheckBox(this);
    StartWithWindows->setText(QString::fromWCharArray(L"开机启动"));
    StartWithWindows->setChecked(Settings.StartWithWindows);
    pLayout->addWidget(StartWithWindows);

    InstallAsScreenSaver = new QCheckBox(this);
    InstallAsScreenSaver->setText(QString::fromWCharArray(L"注册为屏幕保护"));
    InstallAsScreenSaver->setChecked(Settings.IsScreenSaver);
    pLayout->addWidget(InstallAsScreenSaver);

    RunSilently = new QCheckBox(this);
    RunSilently->setText(QString::fromWCharArray(L"静默运行（退出时无须确认）"));
    RunSilently->setChecked((bool)(Settings.Mode&DEF_MOD_SILENT));
    pLayout->addWidget(RunSilently);

    GoodPerformance = new QCheckBox(this);
    GoodPerformance->setText(QString::fromWCharArray(L"循环前置（增加了一个并不明显的性能消耗）"));
    GoodPerformance->setChecked((bool)(Settings.Mode&DEF_MOD_TIMER));
    pLayout->addWidget(GoodPerformance);

    AllowHotKeys = new QCheckBox(this);
    AllowHotKeys->setText(QString::fromWCharArray(L"交互运行（允许用户使用ReadMe中提到的热键）"));
    AllowHotKeys->setChecked(!(bool)(Settings.Mode&DEF_MOD_LOCKED));
    pLayout->addWidget(AllowHotKeys);

    QHBoxLayout *hLayout1 = new QHBoxLayout();
    QLabel *Lab1=new QLabel(QString::fromWCharArray(L"透明度"),this,0);
    hLayout1->addWidget(Lab1);
    Opacity=new QSlider(Qt::Horizontal,this);
    Opacity->setMaximum(100);
    Opacity->setValue(Settings.Opacity);
    Opacity->setEnabled(true);

    LE_Opacity=new QLineEdit(this);
    LE_Opacity->setMaxLength(3);
    LE_Opacity->setMaximumWidth(30);
    LE_Opacity->setText(QString::number(Settings.Opacity,10));

    NumOnly_Regex=new QRegExp("[0-9]+$");
    NumOnly_Validator=new QRegExpValidator(*NumOnly_Regex,LE_Opacity);

    connect(LE_Opacity,SIGNAL(textChanged(QString)),this,SLOT(TextChanged(QString)));
    connect(Opacity,SIGNAL(valueChanged(int)),this,SLOT(SliderChanged(int)));

    hLayout1->addWidget(Opacity);
    hLayout1->addWidget(LE_Opacity);
    pLayout->addLayout(hLayout1);

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    QLabel *Lab2=new QLabel(QString::fromWCharArray(L"图像文件"),this,0);

    ImageFile=new FileNameEdit(this);
    ImageFile->ptrparent=this;
    ImageFile->setReadOnly(true);

    Str_ImageFile=Settings.ImagePath;
    if(QString::compare(Str_ImageFile,QString::fromWCharArray(L":/image/U.jpg"),Qt::CaseSensitive)==0)
        ImageFile->setText(QString::fromWCharArray(L"默认图像"));
    else
        ImageFile->setText(Str_ImageFile);

    hLayout2->addWidget(Lab2);
    hLayout2->addWidget(ImageFile);

    Back_To_Default=new QPushButton(QString::fromWCharArray(L"使用默认图像"),0);
    connect(Back_To_Default,SIGNAL(clicked(bool)),this,SLOT(Default(bool)));

    hLayout2->addWidget(Back_To_Default);
    pLayout->addLayout(hLayout2);

    QHBoxLayout *hLayout3 = new QHBoxLayout();

    RunBtn=new QPushButton(QString::fromWCharArray(L"保存并执行"),this);
    connect(RunBtn,SIGNAL(clicked(bool)),this,SLOT(RunBtn_Clicked(bool)));

    ConfirmBtn=new QPushButton(QString::fromWCharArray(L"保存并退出"),this);
    connect(ConfirmBtn,SIGNAL(clicked(bool)),this,SLOT(ConfirmBtn_Clicked(bool)));

    CancelBtn=new QPushButton(QString::fromWCharArray(L"取消"),this);
    connect(CancelBtn,SIGNAL(clicked(bool)),this,SLOT(CancelBtn_Clicked(bool)));

    hLayout3->addWidget(RunBtn);
    hLayout3->addWidget(ConfirmBtn);
    hLayout3->addWidget(CancelBtn);

    pLayout->addLayout(hLayout3);

    pLayout->setSpacing(10);
    pLayout->setContentsMargins(10, 3, 10, 3);
    setLayout(pLayout);

    this->setWindowTitle("Settings");
    QIcon ico(":/image/Settings.ico");
    this->setWindowIcon(ico);
    resize(300, 300);
    this->setVisible(true);

    return;
}

void SettingDialog::TextChanged(QString Val){

    int tmp;
    bool ok;
    tmp=Val.toInt(&ok,10);

    if(ok){
        if((tmp>=0)&&(tmp<=100))
            Opacity->setValue(tmp);
        else{
            if(tmp<=0){
                Opacity->setValue(0);
                LE_Opacity->setText(QString::number(0,10));
            }
            else{
                Opacity->setValue(100);
                LE_Opacity->setText(QString::number(100,10));
            }
        }
    }else{
        if(Val.length()==0){
            Opacity->setValue(0);
            LE_Opacity->setText(QString::number(0,10));
        }else
            LE_Opacity->setText(QString::number(Opacity->value(),10));
    }
    return;
}

void SettingDialog::SliderChanged(int Val){

    LE_Opacity->setText(QString::number(Val,10));
    return;
}

void SettingDialog::RunBtn_Clicked(bool){

    Generate_Settings();
    qDebug()<<Settings.Mode;
    My_Parent->Do_Setting(&Settings,true);
    this->accept();
    return;
}

void SettingDialog::ConfirmBtn_Clicked(bool){

    Generate_Settings();
    qDebug()<<"Mode:"<<Settings.Mode;

    My_Parent->Do_Setting(&Settings,false);
    this->reject();
    return;
}

void SettingDialog::CancelBtn_Clicked(bool){
    this->reject();
    return;
}

SettingDialog::~SettingDialog()
{
}
