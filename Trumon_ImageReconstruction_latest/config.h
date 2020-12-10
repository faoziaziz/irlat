#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QSysInfo>

#include "default.h"
#include "sysvar.h"

class Config : public QObject
{
    Q_OBJECT

public:
    explicit Config(QObject *parent = 0);
    ~Config();

    void RetrieveConfig(QString configPath);
    void RetrieveLicense(QString configPath);
};

class Hwinfo : public QObject
{
    Q_OBJECT

public:
    explicit Hwinfo(QObject *parent = 0);
    ~Hwinfo();

    void RetrieveHwinfo(QString configPath);
};
