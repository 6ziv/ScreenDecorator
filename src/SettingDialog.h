#ifndef SETTING_DIALOG_H
#define SETTING_DIALOG_H
#include "mainwindow.h"
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include<QLineEdit>
#include<QRegExp>
#include<QRegExpValidator>
#include<windows.h>
class SettingDialog;
class FileNameEdit : public QLineEdit{
    Q_OBJECT
public:
    FileNameEdit(QWidget *);
    ~FileNameEdit();
    void mouseReleaseEvent(QMouseEvent *);
    SettingDialog *ptrparent;
};
class SettingDialog : public QDialog
{
    Q_OBJECT
private:
    void Generate_Settings();
    QCheckBox *StartWithWindows;
    QCheckBox *InstallAsScreenSaver;
    QCheckBox *RunSilently;
    QCheckBox *GoodPerformance;
    QCheckBox *AllowHotKeys;
    QSlider *Opacity;
    QLineEdit *LE_Opacity;
    QPushButton *Back_To_Default;
    QPushButton *RunBtn;
    QPushButton *ConfirmBtn;
    QPushButton *CancelBtn;
    QRegExp *NumOnly_Regex;
    QValidator *NumOnly_Validator;
private slots:
    void RunBtn_Clicked(bool);
    void ConfirmBtn_Clicked(bool);
    void CancelBtn_Clicked(bool);
    void TextChanged(QString Val);
    void SliderChanged(int Val);
    void Default(bool);
public:
    SettingDialog(MainWindow *parent);
    ~SettingDialog();
    MainWindow *My_Parent;
    conf Settings;
    QString Str_ImageFile;
    FileNameEdit *ImageFile;
};
#endif
