#include "ctrlig.h"
#include "ui_ctrlig.h"
#define BUFF_LEN  9
#define CMDHEAD   0xAF
#define CMDADDR   0X02
#define CMDPWMFREQ_A 0X01
#define CMDPWMDUTY_A 0X02
#define CMDPWMFREQ_B 0X03
#define CMDPWMDUTY_B 0X04
#define CMDTEMPERA   0X05
#define CMDGETTEMPERA 0X07
#define CMDFLASHWRITE 0X09
#define CMDFLASHREAD 0X0A
#define CMDFLASHREAD_1 0XA1
#define CMDFLASHREAD_2 0XA2
#define CMDFLASHREAD_3 0XA3
CtrLig::CtrLig(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CtrLig)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
    setWindowTitle(tr("CtrLig"));
    labLive=new QLabel(QString(tr("已运行:0天0时0分0秒")));
    ui->statusbar->addWidget(labLive);
    labTime=new QLabel(QDateTime::currentDateTime().toString(tr("      当前时间:yyyy年MM月dd日 dddd HH:mm:ss")));
    ui->statusbar->addWidget(labTime);
    labSerPort=new QLabel(QString(tr("       端口：未知    发送:0字节     接收:0字节")));
    ui->statusbar->addWidget(labSerPort);
    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
    timerDate = new QTimer(this);
    timerDate->setInterval(1000);
    connect(timerDate,SIGNAL(timeout()),this,SLOT(ShowDateTime()));

    ui->SliDuty1->setMinimum(0);
    ui->SliDuty1->setMaximum(100);
    ui->SliDuty2->setMinimum(0);
    ui->SliDuty2->setMaximum(100);
    ui->SliDuty1->setTickPosition(QSlider::TicksBothSides);
    ui->SliDuty2->setTickPosition(QSlider::TicksBothSides);
    ui->SliDuty2->setFocusPolicy(Qt::StrongFocus);
    ui->SliDuty1->setFocusPolicy(Qt::StrongFocus);

    ui->spBDuty1->setMaximum(100);
    ui->spBDuty2->setMaximum(100);
    ui->spBFreq1->setMaximum(5000);
    ui->spBFreq2->setMaximum(5000);
    timerDate->start();
    InitForm();
    mPlot = new QCustomPlot();
    PlotInit(ui->customPlot);
    QRegExp rx("^-?(180|1?[0-7]?|[0-9]?\\d(\\.\\d{1,4})?)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->liedHiTe2->setValidator(pReg);//限定高低温范围
    ui->liedLoTe2->setValidator(pReg);

}
QextSerialPort *CtrLig::myCom = new QextSerialPort();

void CtrLig::IsSeriaOpen(bool isOpen)
{
    if(isOpen)
    {
    //必须打开串口才能进行槽函数连接
    connect(CtrLig::myCom,SIGNAL(readyRead()),this,SLOT(ReadMyCom()));
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(0).arg(0));
    }
}
void CtrLig::InitForm()
{
    ReceiveCount=0;
    SendCount=0;
    //读取数据(采用定时器读取数据，不采用事件，方便移植到linux)
    myReadTimer=new QTimer(this);
    myReadTimer->setInterval(3);
   // connect(myReadTimer,SIGNAL(timeout()),this,SLOT(ReadMyCom()));
   // this->ChangeEnable(false);

}
unsigned char CtrLig::CheckByte(QByteArray buf)
{
    unsigned int CheckSum = 0;
    if(buf.size() == BUFF_LEN)
    {
        for(int i =1;i<BUFF_LEN-1;i++)
        {
            CheckSum += buf.at(i);
        }
        qDebug("%x",CheckSum%256);
        return (unsigned char)(CheckSum%256);
    }
    else
        return 0;
}
CHAR_INT reTempData2Int;
CHAR_FLOAT reTempData2Float;
void CtrLig::ReadMyCom()
{
    if (CtrLig::myCom->bytesAvailable()<=0){return;}
    myHelper::Sleep(100);//延时100毫秒保证接收到的是一条完整的数据,而不是脱节的
    QByteArray buffer=CtrLig::myCom->readAll();
    ReceiveCount=ReceiveCount+buffer.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));

    //char * bufData = buffer.data();
    for(int i =0;i<buffer.size();i++)
        qDebug("%x",(unsigned char)buffer.at(i));
    qDebug("%x",buffer.size());
    if((buffer.size()==BUFF_LEN)&&(((unsigned char)buffer.at(0)) == CMDHEAD)&&(buffer.at(1) == CMDADDR)&&(buffer.at(2)==CMDGETTEMPERA)&&((unsigned char)buffer.at(8)==CheckByte(buffer)))
    {
            reTempData2Float.byte[0] = buffer.at(4);
            reTempData2Float.byte[1] = buffer.at(5);
            reTempData2Float.byte[2] = buffer.at(6);
            reTempData2Float.byte[3] = buffer.at(7);
            qDebug("%x",buffer.size());
    }
   else if((buffer.size()==BUFF_LEN)&&(((unsigned char)buffer.at(0)) == CMDHEAD)&&(buffer.at(1)==CMDADDR)&&((unsigned char)buffer.at(2)==CMDFLASHREAD_1)&&((unsigned char)buffer.at(8)==CheckByte(buffer)))
   {

            reTempData2Int.byte[0] = buffer.at(3);
            reTempData2Int.byte[1] = buffer.at(4);
            reTempData2Int.byte[2] = buffer.at(5);
            reTempData2Int.byte[3] = buffer.at(6);
            ui->spBFreq1->setValue(reTempData2Int.I_value);
            ui->spBDuty1->setValue((unsigned int)(buffer.at(7)));
            buffer.clear();

    }
     else if((buffer.size()==BUFF_LEN)&&(((unsigned char)buffer.at(0)) == CMDHEAD)&&(buffer.at(1)==CMDADDR)&&((unsigned char)buffer.at(2)==CMDFLASHREAD_2)&&((unsigned char)buffer.at(8)==CheckByte(buffer)))
    {
        reTempData2Int.byte[0] = buffer.at(3);
        reTempData2Int.byte[1] = buffer.at(4);
        reTempData2Int.byte[2] = buffer.at(5);
        reTempData2Int.byte[3] = buffer.at(6);
        ui->spBFreq2->setValue(reTempData2Int.I_value);
        ui->spBDuty2->setValue((unsigned int)(buffer.at(7)));
        buffer.clear();
    }
    else if((buffer.size()==BUFF_LEN)&&(((unsigned char)buffer.at(0)) == CMDHEAD)&&(buffer.at(1)==CMDADDR)&&((unsigned char)buffer.at(2)==CMDFLASHREAD_3)&&((unsigned char)buffer.at(8)==CheckByte(buffer)))
    {
           ui->spHiTemp1->setValue(buffer.at(4));
           ui->spLoTemp1->setValue(buffer.at(6));
           buffer.clear();
    }
}

CtrLig::~CtrLig()
{
    CtrLig::myCom->close();
    this->myReadTimer->stop();
    this->timerDate->stop();
    delete ui;
}
void CtrLig::on_action_triggered()
{
    SeriaSet *seriaUi = new SeriaSet();
    connect(seriaUi,SIGNAL(seriaOpen(bool)),this,SLOT(IsSeriaOpen(bool)));
    seriaUi->exec();
}
void CtrLig::ShowDateTime()
{
    uSecond++;
    if(uSecond == 60){uMin ++;uSecond = 0;}
    if(uMin == 60){uHour ++; uMin =0;}
    if(uHour == 24){uDay++;uHour=0;}
    labLive->setText(tr("已运行:%1天%2时%3分%4秒").arg(uDay).arg(uHour).arg(uMin).arg(uSecond));
    labTime->setText(QDateTime::currentDateTime().toString(tr("      当前时间:yyyy年MM月dd日 dddd HH:mm:ss")));
    if (CtrLig::myCom->isOpen())
    {
        GetTempera();
    }
}
void CtrLig::PlotInit(QCustomPlot *customPlot)
{
    customPlot->setNotAntialiasedElements(QCP::aeAll);
    //设定图例
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    customPlot->legend->setFont(font);
    customPlot->legend->setVisible(true);

    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    customPlot->graph(0)->setName("频率");
    customPlot->addGraph(); // red line
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    customPlot->graph(1)->setName("温度");
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    displayTimer = new QTimer();
  //  displayTimer->setInterval(200);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    displayTimer->start(10);
}
void CtrLig::realtimeDataSlot()
{
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed()/500.0; // time elapsed since start of demo, in seconds
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.002) // at most add point every 2 ms
  {
    // add data to lines:
    //  ui->customPlot->graph(0)->addData(key, );
   //   ui->customPlot->graph(0)->addData(key, 50);
      ui->customPlot->graph(1)->addData(key, reTempData2Float.f_value);
    // rescale value (vertical) axis to fit the current data:
    //ui->customPlot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customPlot->xAxis->setRange(key, 10, Qt::AlignRight);
  ui->customPlot->yAxis->setRange(-reTempData2Float.f_value-10,reTempData2Float.f_value+10);
  ui->customPlot->replot();
   // reTempData2Float.f_value =0;
  // calculate frames per second:
//  static double lastFpsKey;
//  static int frameCount;
//  ++frameCount;
//  if (key-lastFpsKey > 2) // average fps over 2 seconds
//  {
//    ui->statusBar->showMessage(
//          QString("%1 FPS, Total Data points: %2")
//          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//          .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
//          , 0);
//    lastFpsKey = key;
//    frameCount = 0;
//  }
}
//---------table widget---------------------
/*#define BUFF_LEN  20
unsigned char ComBuf[BUFF_LEN];
unsigned char ch = 0;
unsigned char check_byte = 0;
unsigned char CheckByte = 0;
unsigned char cmd_index = 0;
unsigned char index = 0;
unsigned char destin_id;
unsigned char source_id;
unsigned char cmd_name;
#define PROBER_ID 0x01
#define  PC_ID  0X0F
void CtrLig::SendReplyHead(void)//发送响应帧
{
    QByteArray buf;
    for(index=0;index<8;index++)
        buf.append(index,0xff);
    CtrLig::myCom->write(buf);
    if(source_id == PC_ID)
    {
        source_id |= 0x80;
    }
    buf[0] = source_id;
    CtrLig::myCom->write(buf);
    CheckByte = 0x00;
    CheckByte = CheckByte^source_id;
   // SendByte(PROBER_ID);
    CheckByte = CheckByte^PROBER_ID;
  //  SendByte(cmd_name);
    CheckByte = CheckByte^cmd_name;
}
#define DESTID 0x50
#define SOURCEID 0x51
#define CMDNAME 0X52
void CtrLig::SendCmdFram(void)
{
    QByteArray buf;
    buf.append(0,DESTID);
    buf.append(1,SOURCEID);
    buf.append(2,CMDNAME);
    buf.append(3,buf.length());

}
*/

void CtrLig::on_btnSig1_clicked(void)
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    CHAR_INT PWMFreq_A;
    CHAR_INT PWMDuty_A;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDPWMFREQ_A);
    if(ui->ChBEnPWM_1->isChecked() == true)
        buf.append(1);
    else
        buf.append((char)0);
    PWMFreq_A.I_value = ui->spBFreq1->value();
    buf.append(PWMFreq_A.byte[0]);
    buf.append(PWMFreq_A.byte[1]);
    buf.append(PWMFreq_A.byte[2]);
    buf.append(PWMFreq_A.byte[3]);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    CheckSum = 0;
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
//--------------DUTY-------------
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDPWMDUTY_A);
    if(ui->ChBEnPWM_1->isChecked() == true)
        buf.append(1);
    else
        buf.append((char)0);
    PWMDuty_A.I_value = ui->spBDuty1->value();
    buf.append(PWMDuty_A.byte[0]);
    buf.append(PWMDuty_A.byte[1]);
    buf.append(PWMDuty_A.byte[2]);
    buf.append(PWMDuty_A.byte[3]);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}
void CtrLig::GetTempera()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDGETTEMPERA);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    CheckSum = 0;
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}
void CtrLig::on_SliDuty1_valueChanged(int value)
{
    ui->spBDuty1->setValue(value);
}

void CtrLig::on_spBDuty1_valueChanged(int arg1)
{
    ui->SliDuty1->setValue(arg1);
}

void CtrLig::on_SliDuty2_valueChanged(int value)
{
    ui->spBDuty2->setValue(value);
}

void CtrLig::on_spBDuty2_valueChanged(int arg1)
{
    ui->SliDuty2->setValue(arg1);
}

void CtrLig::on_btnSig2_clicked()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开！")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDTEMPERA);
    int tempH = ui->spHiTemp1->value();
    int tempL = ui->spLoTemp1->value();
    if(tempH <= tempL)
    {
        myHelper::ShowMessageBoxError(QString(tr("高温设定低于或等于低温设定！")));
        return;
    }
    buf.append((char)0);
    buf.append(tempH);
    buf.append((char)0);
    buf.append(tempL);
    buf.append((char)0);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}

void CtrLig::on_btnSig3_clicked()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    CHAR_INT PWMFreq_B;
    CHAR_INT PWMDuty_B;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDPWMFREQ_B);
    if(ui->ChBEnPWM_2->isChecked() == true)
        buf.append(1);
    else
        buf.append((char)0);
    PWMFreq_B.I_value = ui->spBFreq2->value();
    buf.append(PWMFreq_B.byte[0]);
    buf.append(PWMFreq_B.byte[1]);
    buf.append(PWMFreq_B.byte[2]);
    buf.append(PWMFreq_B.byte[3]);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    CheckSum = 0;
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
//--------------DUTY-------------
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDPWMDUTY_B);
    if(ui->ChBEnPWM_2->isChecked() == true)
        buf.append(1);
    else
        buf.append((char)0);
    PWMDuty_B.I_value = ui->spBDuty2->value();
    buf.append(PWMDuty_B.byte[0]);
    buf.append(PWMDuty_B.byte[1]);
    buf.append(PWMDuty_B.byte[2]);
    buf.append(PWMDuty_B.byte[3]);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}

void CtrLig::on_btnSig4_clicked()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开！")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDTEMPERA);
    int tempH = ui->liedHiTe2->text().toInt();
    int tempL = ui->liedLoTe2->text().toInt();
    if(tempH <= tempL)
    {
        myHelper::ShowMessageBoxError(QString(tr("高温设定低于或等于低温设定！")));
        return;
    }
    buf.append((char)0);
    buf.append(tempH);
    buf.append((char)0);
    buf.append(tempL);
    buf.append((char)0);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}

void CtrLig::on_btnWrite_clicked()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开！")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDFLASHWRITE);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append(1);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}

void CtrLig::on_btnRead_clicked()
{
    if (!CtrLig::myCom->isOpen()) { myHelper::ShowMessageBoxError(QString(tr("串口未打开！")));return; }//串口没有打开
    QByteArray buf;
    unsigned int CheckSum = 0;
    buf.clear();
    buf.append(CMDHEAD);
    buf.append(CMDADDR);
    buf.append(CMDFLASHREAD);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append((char)0);
    buf.append(1);
    for(int i = 1;i<BUFF_LEN-1;i++)
    CheckSum += buf.at(i);
    CheckSum = CheckSum%256;
    buf.append(CheckSum);
    myCom->write(buf);
    SendCount=SendCount+buf.size();
    labSerPort->setText(QString(tr("       端口：%1 打开    发送:   %2字节     接收:   %3字节")).arg(CtrLig::myCom->portName()).arg(SendCount).arg(ReceiveCount));
}
