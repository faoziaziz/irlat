#include "decodetask.h"

/* Constructor */
DecodeTask::DecodeTask(QSqlQuery *queryFileRaw, int profilePe, HasilImage *hasilImageHost)
{
    dataRaw.indexNum    = queryFileRaw->value(0).toInt();
    dataRaw.deviceId    = queryFileRaw->value(1).toByteArray();
    dataRaw.fileName    = queryFileRaw->value(2).toString();
    dataRaw.fileSize    = queryFileRaw->value(3).toInt();
    dataRaw.fileContent = queryFileRaw->value(4).toByteArray();
    dataRaw.profilePe   = profilePe;

    hasilImage = hasilImageHost;

    taskNum = dataRaw.indexNum;
    setAutoDelete(true);
}

/* Destructor */
DecodeTask::~DecodeTask()
{
    //qDebug()<< "del#"<<taskNum; ///
}

/* Menjalankan proses decoding */
void DecodeTask::run()
{
    //qDebug() << "Run task#" << taskNum;
    //QThread::msleep(100+(qrand() % 1000));

    dataRaw.Decompress();

    int printerSelection = dataRaw.profilePe / 1000;

    HasilOlah * tmpHasilOlah;
    do {
        if(printerSelection == 0){
            /* Auto Select All Possible Printer */
            tmpHasilOlah = decodeData(4);
        }else{
            tmpHasilOlah = decodeData(printerSelection);
        }

        //write hasil
        while(!hasilImage->mutex.tryLock(10));
        hasilImage->data.enqueue(*tmpHasilOlah);
        hasilImage->mutex.unlock();

        while(!hasilImage->data.isEmpty())
        {
            QThread::msleep(100);
        }
    } while(tmpHasilOlah->isnotFinished != 0 && tmpHasilOlah->lastIndex != 0);
}

HasilOlah* DecodeTask::decodeData(int printerSelection){
    TicketCanvass *ticketCanvass = new TicketCanvass(10000,50000);

    PrinterDecode *printerDecode;

    switch (printerSelection)
    {
        case 1:
            printerDecode = new _EpsonThermal;
            printerDecode->deInterlace = false;
        break;

        case 2:
            printerDecode = new _EpsonESCP2;
            printerDecode->deInterlace = false;
        break;

        case 3:
            printerDecode = new _EpsonESCP9PIN;
            printerDecode->deInterlace = false;
        break;

        case 4:
            printerDecode = new _EpsonThermal;
            printerDecode->deInterlace = true;
        break;

        case 5:
            printerDecode = new _EpsonESCP2;
            printerDecode->deInterlace = true;
        break;

        case 6:
            printerDecode = new _EpsonESCP9PIN;
            printerDecode->deInterlace = true;
        break;

        default:
            printerDecode = new _EpsonThermalFull;
        break;
    }

    HasilOlah * tmpHasilOlah = new HasilOlah;

    int pixcount = printerDecode->Decode(&dataRaw.fileContent, ticketCanvass, &dataOffset);

    if(pixcount) {
        tmpHasilOlah->indexSumber = dataRaw.indexNum;
        tmpHasilOlah->deviceId = dataRaw.deviceId;
        tmpHasilOlah->profilePe = dataRaw.profilePe;

        int pixelScale = dataRaw.profilePe % 1000;
        if (pixelScale == 0)
            pixelScale = PNG_PIXEL_SIZE;

        ticketCanvass->GetCanvass(&tmpHasilOlah->imageHasil, pixelScale, printerDecode->deInterlace);
    }
    else {
        tmpHasilOlah->indexSumber = dataRaw.indexNum;
        tmpHasilOlah->deviceId = dataRaw.deviceId;
        tmpHasilOlah->imageHasil = dataRaw.fileContent;
        if( printerSelection == 7 ) {
            tmpHasilOlah->profilePe = DB_FLAG_SEREH;
        }
        else {
            tmpHasilOlah->profilePe = DB_FLAG_TRASH;
        }
    }

    if((dataRaw.fileContent.size() - dataOffset) < 64) {
        tmpHasilOlah->lastIndex = 0;
        tmpHasilOlah->isnotFinished = 0;
        dataOffset = 0;
    }
    else {
        tmpHasilOlah->lastIndex = dataOffset;
        tmpHasilOlah->isnotFinished = 1;
    }

    return tmpHasilOlah;
}
