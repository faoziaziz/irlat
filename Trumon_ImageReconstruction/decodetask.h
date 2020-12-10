#pragma once

#include <QRunnable>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QSqlQuery>
#include <QDebug>
#include <QImage>

#include "config.h"
#include "tsdata.h"
#include "ticketcanvass.h"
#include "printerdecode.h"
#include "_epson_thermal.h"
#include "_epson_escp2.h"
#include "_epson_escp_9pin.h"
#include "_epson_thermal_full.h"

class DecodeTask : public QRunnable
{
public:
    DecodeTask(QSqlQuery *queryFileRaw, int profilePe, HasilImage *hasilImage);
    ~DecodeTask();

    void run();

    int taskNum;

    HasilImage *hasilImage;

private:
    DataRaw dataRaw;
    HasilOlah * decodeData(int printerSelection);
    int dataOffset = 0;

signals:
    void KirimHasil(int idSumber, QByteArray devId, QByteArray imHasil, int proPE);


};

