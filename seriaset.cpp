#include "seriaset.h"
#include "ui_seriaset.h"

SeriaSet::SeriaSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeriaSet)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
     myHelper::FormInCenter(this);
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
    InitForm();
    if(!CtrLig::myCom->isOpen())
    {
        BaudRateType BaudRateEnum = CtrLig::myCom->baudRate();
        qDebug("%d",BaudRateEnum);
        unsigned int BaudRate = BaudRateEnum;
        ui->cboxPortName->setCurrentText(CtrLig::myCom->portName());
        switch(BaudRate)
        {
            case 2400 :    ui->cboxBaudRate->setCurrentIndex(0);break;
            case 4800 :    ui->cboxBaudRate->setCurrentIndex(1);break;
            case 9600 :    ui->cboxBaudRate->setCurrentIndex(2);break;
            case 14400 :    ui->cboxBaudRate->setCurrentIndex(3);break;
            case 19200 :    ui->cboxBaudRate->setCurrentIndex(4);break;
            case 38400 :    ui->cboxBaudRate->setCurrentIndex(5);break;
            case 56000 :    ui->cboxBaudRate->setCurrentIndex(6);break;
            case 57600 :    ui->cboxBaudRate->setCurrentIndex(7);break;
            case 76800 :    ui->cboxBaudRate->setCurrentIndex(8);break;
            case 115200 :    ui->cboxBaudRate->setCurrentIndex(9);break;
            case 128000 :    ui->cboxBaudRate->setCurrentIndex(10);break;
        default:ui->cboxBaudRate->setCurrentIndex(11);break;
        }

    }
}

SeriaSet::~SeriaSet()
{
    delete ui;
}
void SeriaSet::InitForm(void)
{

#ifdef Q_OS_WIN//如果是windows系统
    comList<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"
          <<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12"
         <<"COM13"<<"COM14"<<"COM15";
#else//如果是unix或者其他系统
    comList<<"ttyUSB0"<<"ttyUSB1"<<"ttyUSB2"<<"ttyUSB3"<<"ttyUSB4"<<"ttyUSB5"
          <<"ttyS0"<<"ttyS1"<<"ttyS2"<<"ttyS3"<<"ttyS4"<<"ttyS5"<<"ttyS6"
         <<"ttyS7"<<"ttyS8"<<"ttyS9";
#endif

    //ui->cboxPortName->addItems(comList);
   // ui->cboxPortName->setCurrentIndex(0);

    baudList<<"2400"<<"4800"<<"9600"
          <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
         <<"76800"<<"115200"<<"128000"<<"256000";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(2);

    parityList<<"无"<<"奇"<<"偶";

#ifdef Q_OS_WIN//如果是windows系统
    parityList<<"标志";
#endif

    parityList<<"空格";

    ui->cboxParity->addItems(parityList);
    ui->cboxParity->setCurrentIndex(0);

    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->cboxDataBit->addItems(dataBitsList);
    ui->cboxDataBit->setCurrentIndex(3);

    stopBitsList<<"1";

#ifdef Q_OS_WIN//如果是windows系统
    stopBitsList<<"1.5";
#endif

    stopBitsList<<"2";

    ui->cboxStopBit->addItems(stopBitsList);
    ui->cboxStopBit->setCurrentIndex(0);
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    QStringList strCom;
    strCom.clear();
    for( int i = 0; i < ports.count(); ++i)
       strCom << ports.at(i).portName.toLocal8Bit().constData();
    ui->cboxPortName->addItems(strCom);
}

void SeriaSet::on_btnOK_clicked()
{
     QString portName = ui->cboxPortName->currentText();
     qDebug()<<"CurrentPort: "<<portName;
     CtrLig::myCom->setPortName(portName);
    if (CtrLig::myCom->open(QIODevice::ReadWrite))
    {
        //清空缓冲区
        CtrLig::myCom->flush();
        //设置波特率
        CtrLig::myCom->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
        //设置数据位
        CtrLig::myCom->setDataBits((DataBitsType)ui->cboxDataBit->currentText().toInt());
        //设置校验位
        CtrLig::myCom->setParity((ParityType)ui->cboxParity->currentIndex());
        //设置停止位
        CtrLig::myCom->setStopBits((StopBitsType)ui->cboxStopBit->currentIndex());
        CtrLig::myCom->setFlowControl(FLOW_OFF);
        CtrLig::myCom->setTimeout(10);

       // this->ChangeEnable(true);
       // labSerPort->setText(QString(tr("端口：%1打开    发送:%2字节     接收:%3字节")).arg(ui->cboxPortName->currentText()).arg(0).arg(0));

        emit seriaOpen(true);
        this->close();
     }
    else
    {
        myHelper::ShowMessageBoxError("串口打开失败，请检查串口号！");
    }

}

void SeriaSet::on_btnQuit_clicked()
{
    this->close();
}
void SeriaSet::on_cboxPortName_currentTextChanged(const QString &arg1)
{
    CtrLig::myCom->close();
}

void SeriaSet::on_cboxBaudRate_currentIndexChanged(int index)
{
    CtrLig::myCom->close();
}
