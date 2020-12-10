#pragma once

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QThreadPool>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QDebug>
#include <QThread>

#include <config.h>
#include <default.h>
#include <tsdata.h>
#include <decodetask.h>

#include "sysvar.h"

class PrintEmulator : public QObject
{
    Q_OBJECT
public:
    explicit PrintEmulator(QObject *parent = 0);
    ~PrintEmulator();
    bool SetDatabase(int argc, char *argv[]);

private:
    QSqlDatabase dbConnection;

    bool ReconnectDatabase(void);
    int queryLimit;
    int queryLastIndex;
    int maxThread;

    int noDataCounter;

    void InsertImage(int indexSumber, QByteArray deviceId, QByteArray imageToInsert);
    void InsertIbase(int indexSumber, QByteArray deviceId, QByteArray base64_data);
    void InsertTeks(int indexSumber, QByteArray deviceId, QByteArray Teks);
    void UpdateFlagPE(int indexToUpdate, int nilaiFlag);

    QMap<QByteArray, DeviceProfile> deviceList;

    HasilImage hasilImage;

    QTimer *timerDeviceList;
    QTimer *timerProcessData;
    QTimer *timerSaveResult;

    QThreadPool *threadPool;
    int taskCreated;
    int taskSkipped;
    int taskFinished;

    QMap<int, DecodeTask*> decodeTask;


signals:

public slots:
    void ProcessData(void);
    void SaveResult(void);
    void RefreshDeviceList(void);

};
