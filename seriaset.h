#ifndef SERIASET_H
#define SERIASET_H

#include <QDialog>
#include "ctrlig.h"
#include "qextserialenumerator.h"
namespace Ui {
class SeriaSet;
}

class SeriaSet : public QDialog
{
    Q_OBJECT

public:
    explicit SeriaSet(QWidget *parent = 0);
    ~SeriaSet();
signals:
    seriaOpen(bool isOpen);
private slots:
    void on_btnOK_clicked();
    void on_btnQuit_clicked();
    void on_cboxPortName_currentTextChanged(const QString &arg1);

    void on_cboxBaudRate_currentIndexChanged(int index);

private:
    void InitForm(void);
private:
    Ui::SeriaSet *ui;
    QStringList comList;//串口号
    QStringList baudList;//波特率
    QStringList parityList;//校验位
    QStringList dataBitsList;//数据位
    QStringList stopBitsList;//停止位
};

#endif // SERIASET_H
