#include <QCoreApplication>

#include "config.h"
#include "printemulator.h"

DbVar *DbVar::s_instance;
SysVar *SysVar::s_instance;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << APP_NAME << APP_VER;
    qDebug() << PRASIMAX << APP_DATE;

    Hwinfo* hwinfo = new Hwinfo;
    hwinfo->RetrieveHwinfo(argv[2]);

    if(argc!=6){
        qDebug()<<"Usage:PrintEmulator <DB_Addr> <DB_Name> <DB_Port> <User> <Pass>";
        return 0;
    }

    QThread* threadPrintEmulator = new QThread;
    PrintEmulator* printEmulator = new PrintEmulator;

    printEmulator->SetDatabase(argc,argv);

    printEmulator->moveToThread(threadPrintEmulator);
    QObject::connect(threadPrintEmulator, SIGNAL(started()),printEmulator,SLOT(ProcessData()));

    threadPrintEmulator->start();

    return a.exec();
}
