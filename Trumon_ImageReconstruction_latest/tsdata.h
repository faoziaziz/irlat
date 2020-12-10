#pragma once

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QSqlQuery>

class DeviceProfile
{
public:
    QByteArray deviceId;
    int profilePE;
};

class HasilOlah
{
public:
    int indexSumber;
    QByteArray deviceId;
    QByteArray imageHasil;
    int profilePe;
    int isnotFinished = 0;
    int lastIndex = 0;
};

class HasilImage
{
public:
    QQueue<HasilOlah> data;
    QMutex mutex;
};

class DataRaw
{
public:
    int indexNum;
    QByteArray deviceId;
    QByteArray fileContent;
    QString fileName;
    int fileSize;
    int profilePe;

    DataRaw();

    void ErrorChecker(void);
    void Decompress(void);

private:
    bool isDecrypted;
    bool isDecompressed;

};
