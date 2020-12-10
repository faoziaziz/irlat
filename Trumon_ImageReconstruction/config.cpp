#include "config.h"

/* Constructor */
Config::Config(QObject *parent) : QObject(parent)
{}

/* Destructor */
Config::~Config()
{}


/* Mengambil parameter untuk inisialiasi DB dari file config.ini */
void Config::RetrieveConfig(QString configPath)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("dbsetting");
    const QStringList childKeys = settings.childKeys();
    int flag = 0;
    foreach (const QString &childKey, childKeys)
    {
        //configContent.insert(flag,settings.value(childKey).toString());
        DbVar::instance()->set_value(flag, settings.value(childKey).toString());
        flag++;
    }
    settings.endGroup();
}


/* Mengambil parameter untuk licensing dari file config.ini [Belum dipakai] */
/*void Config::RetrieveLicense(QString configPath)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("licensing");
    const QStringList childKeys = settings.childKeys();
    int flag = 0;
    foreach (const QString &childKey, childKeys)
    {
        //licenseContent.insert(flag, settings.value(childKey).toString());
        flag++;
    }
    settings.endGroup();
}*/


/* Constructor */
Hwinfo::Hwinfo(QObject *parent) : QObject(parent)
{}


/* Destructor */
Hwinfo::~Hwinfo()
{}


/* Menyimpan data informasi host PC ke file config.ini */
void Hwinfo::RetrieveHwinfo(QString configPath)
{
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.beginGroup("hostinfo");

    //settings.setValue("host_name", QSysInfo::machineHostName());
    settings.setValue("os_name", QSysInfo::productType());
    settings.setValue("os_version", QSysInfo::kernelVersion());
    settings.setValue("architecture", QSysInfo::buildCpuArchitecture());

    settings.endGroup();
}
