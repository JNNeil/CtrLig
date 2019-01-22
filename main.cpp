#include <QApplication>
#include <QTranslator>
#include <QSharedMemory>
#include "myhelper.h"
#include "ctrlig.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //�����ַ�����������ʽ
    myHelper::SetUTF8Code();
    myHelper::SetStyle();

    //���������ַ�
    QTranslator translator;
    translator.load(":/qt_zh_CN.qm");
    a.installTranslator(&translator);
    //���������ڴ�,�ж��Ƿ��Ѿ����г���
    QSharedMemory men("SystemName");
    if(!men.create(1))
    {
         myHelper::ShowMessageBoxError("�����Ѿ����У�������Զ��رգ� ");
            return 1;
    }
    CtrLig w;
    w.show();

    return a.exec();
}
