#include <QApplication>
#include <QTranslator>
#include <QSharedMemory>
#include "myhelper.h"
#include "ctrlig.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置字符编码和外观样式
    myHelper::SetUTF8Code();
    myHelper::SetStyle();

    //加载中文字符
    QTranslator translator;
    translator.load(":/qt_zh_CN.qm");
    a.installTranslator(&translator);
    //创建共享内存,判断是否已经运行程序
    QSharedMemory men("SystemName");
    if(!men.create(1))
    {
         myHelper::ShowMessageBoxError("程序已经运行，软件将自动关闭！ ");
            return 1;
    }
    CtrLig w;
    w.show();

    return a.exec();
}
