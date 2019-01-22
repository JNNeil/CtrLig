#ifndef CTRLIG_H
#define CTRLIG_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "qextserialport.h"
#include "myhelper.h"
#include "seriaset.h"
#include "./plot/qcustomplot.h"
namespace Ui {
class CtrLig;
}

class CtrLig : public QMainWindow
{
    Q_OBJECT

public:
    explicit CtrLig(QWidget *parent = 0);
    ~CtrLig();

private slots:
    void on_action_triggered();
    void ShowDateTime();
    void IsSeriaOpen(bool);
    void ReadMyCom();
    void realtimeDataSlot();
    void on_btnSig1_clicked();
    void on_spBDuty1_valueChanged(int arg1);

    void on_SliDuty2_valueChanged(int value);

    void on_spBDuty2_valueChanged(int arg1);

    void on_SliDuty1_valueChanged(int value);

    void on_btnSig2_clicked();

    void on_btnSig3_clicked();

    void on_btnSig4_clicked();

    void on_btnWrite_clicked();

    void on_btnRead_clicked();

private:
    void InitForm();//初始化界面以及其他处理
    void PlotInit(QCustomPlot *customPlot);
    void SendReplyHead(void);
    void GetTempera();
    unsigned char CheckByte(QByteArray buf);
private:
    Ui::CtrLig *ui;
    //-----状态栏---------
    QStatusBar *statusBar;
    QLabel *labLive;    //已运行时间标签
    QLabel *labTime;    //当前时间标签
    QLabel *labSerPort;
    QTimer *timerDate;

    unsigned int uSecond = 0;
    unsigned int uDay = 0;
    unsigned int uHour = 0;
    unsigned int uMin = 0;
    unsigned int ReceiveCount = 0;
    unsigned int SendCount = 0;
    //-----
    QCustomPlot *mPlot;
    QTimer *displayTimer;
    //-----qextseri
public:
   static QextSerialPort *myCom;
    QTimer *myReadTimer;//定时读取串口数据
};
typedef union charfloat
{
    unsigned char byte[4];
    float f_value;
}CHAR_FLOAT;

typedef union charint
{
    unsigned char byte[4];
    unsigned int I_value;
}CHAR_INT;
#endif // CTRLIG_H
