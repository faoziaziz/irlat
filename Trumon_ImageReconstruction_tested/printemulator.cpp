#include "printemulator.h"

/* Melakukan pemrosesan data dari tabel FTS2 */
void PrintEmulator::ProcessData(void) {
    if (!dbConnection.isOpen()) {
        qDebug()<< "Opening DB Connection";
        if(!dbConnection.open()) {
            this->ReconnectDatabase();
        }
        RefreshDeviceList();
        SaveResult();
    }

    queryLimit = PE_MAX_THREAD_QUEUE - taskCreated + taskFinished;

    if(queryLimit > 0) {
        int newTaskCount = 0;
        int countResult = 0;

        QSqlQuery queryFileRaw("SELECT idFileTransferStage2, DeviceId, FileName, FileSize, FileData FROM FileTransferStage2 WHERE idFileTransferStage2>? AND FlagPE=0 ORDER BY idFileTransferStage2 ASC LIMIT ?",dbConnection);
        queryFileRaw.bindValue(0, queryLastIndex);
        queryFileRaw.bindValue(1, queryLimit);

        if(queryFileRaw.exec()) {
            while(queryFileRaw.next()) {
                countResult++;

                //cek profilePE
                int indexNum = queryFileRaw.value(0).toInt();
                int profilePe = deviceList.value(queryFileRaw.value(1).toByteArray()).profilePE;
                if(profilePe > 0) {
                    decodeTask.insert(indexNum, new DecodeTask(&queryFileRaw, profilePe, &hasilImage));
                    threadPool->start(decodeTask.value(indexNum));
                    taskCreated++;
                    newTaskCount++;
                }
                else {
                    //UpdateFlagPE(indexNum, DB_FLAG_SKIP); --> update task diserahkan ke SaveResult();
                    {
                        HasilOlah tmpHasilOlah;
                        tmpHasilOlah.deviceId=queryFileRaw.value(1).toByteArray();
                        tmpHasilOlah.indexSumber=indexNum;
                        tmpHasilOlah.profilePe = DB_FLAG_SKIP;
                        tmpHasilOlah.imageHasil= nullptr;

                        while(hasilImage.mutex.tryLock(10));
                        hasilImage.data.enqueue(tmpHasilOlah);
                        hasilImage.mutex.unlock();
                    }
                    taskSkipped++;
                }

                queryLastIndex = indexNum;
            }
        }

        if(countResult>0) {
            timerProcessData->start(MS_PROCESS_DATA);
            noDataCounter=0;
        }
        else {
            timerProcessData->start(MS_PROCESS_NODATA_DELAY);
            noDataCounter++;
            qDebug()<< "....." << queryLastIndex << noDataCounter;
        }
    }
    else {
        timerProcessData->start(MS_PROCESS_DATA);
    }
}


/* Melakukan update terhadap data row di FTS2 yang telah selesai diproses */
void PrintEmulator::UpdateFlagPE(int indexToUpdate, int nilaiFlag) {
    QSqlQuery queryUpdateFlag("UPDATE FileTransferStage2 SET FlagPE=? WHERE idFileTransferStage2=?");
    queryUpdateFlag.bindValue(0, nilaiFlag);
    queryUpdateFlag.bindValue(1, indexToUpdate);
    while(!queryUpdateFlag.exec()) {
        ReconnectDatabase();
    }
}

/* Menjalankan query untuk insert data Image yang telah selesai diproses ke tabel Image */
void PrintEmulator::InsertImage(int indexSumber, QByteArray deviceId, QByteArray imageToInsert) {
    QSqlQuery querySaveImage("INSERT INTO Image (DeviceId, RefSN, Data) VALUES (?, ?, 0x" + imageToInsert.toHex() + ")");
    querySaveImage.bindValue(0, deviceId);
    querySaveImage.bindValue(1, indexSumber);

    qDebug()<< "Inserting Image";
    while(!querySaveImage.exec()) {
        ReconnectDatabase();
    }
}

void PrintEmulator::InsertIbase(int indexSumber, QByteArray deviceId, QByteArray base64_data) {
    QSqlQuery queryStoreIbase("INSERT INTO Ibase (DeviceId, RefSN, Data) VALUES (?, ?, ?)");
    queryStoreIbase.bindValue(0,deviceId);
    queryStoreIbase.bindValue(1,indexSumber);
    queryStoreIbase.bindValue(2,base64_data);

    qDebug()<< "Inserting Ibase";
    while(!queryStoreIbase.exec()) {
        ReconnectDatabase();
    }
}

void PrintEmulator::InsertTeks(int indexSumber, QByteArray deviceId, QByteArray Teks) {
    QSqlQuery queryStoreHasil("INSERT INTO Teks (DeviceId, RefSN, Data) VALUES (?, ?, ?)");
    queryStoreHasil.bindValue(0,deviceId);
    queryStoreHasil.bindValue(1,indexSumber);
    queryStoreHasil.bindValue(2,Teks);

    qDebug()<< "Inserting Text";
    while(!queryStoreHasil.exec()) {
        ReconnectDatabase();
    }
}

/* Melakukan penyimpanan data yang selesai diproses ke tabel Image
 (Di dalamnya akan dijalankan fungsi InsertImage) */
void PrintEmulator::SaveResult(void) {
    while(!hasilImage.data.isEmpty()) {
        HasilOlah tmpHasilOlah;
        tmpHasilOlah = hasilImage.data.dequeue();
        if(tmpHasilOlah.profilePe != DB_FLAG_SKIP) {
            if(tmpHasilOlah.profilePe != DB_FLAG_TRASH) {
                if(tmpHasilOlah.profilePe != DB_FLAG_SEREH) {
                    InsertImage(tmpHasilOlah.indexSumber, tmpHasilOlah.deviceId, tmpHasilOlah.imageHasil);
                    QSqlQuery queryImage("SELECT SeqNum FROM Image WHERE RefSN = ? and Flag = 0",dbConnection);
                    queryImage.bindValue(0, tmpHasilOlah.indexSumber);
                    while(!queryImage.exec()) {
                        ReconnectDatabase();
                    }
                    int seqnumImage = 0;
                    if (queryImage.next()) {
                        seqnumImage = queryImage.value(0).toInt();
                    }
                    InsertTeks(seqnumImage, tmpHasilOlah.deviceId, tmpHasilOlah.teksHasil);
                    QSqlQuery queryUpdateFlag("UPDATE Image SET Flag = 1 WHERE SeqNum = ?");
                    queryUpdateFlag.bindValue(0, seqnumImage);
                    while(!queryUpdateFlag.exec()) {
                        ReconnectDatabase();
                    }
                }
                else {
                    InsertTeks(tmpHasilOlah.indexSumber, tmpHasilOlah.deviceId, tmpHasilOlah.imageHasil);
                }
            }
            taskFinished++;
        }
        UpdateFlagPE(tmpHasilOlah.indexSumber, tmpHasilOlah.profilePe);

        qDebug()<<"<<<"
                << tmpHasilOlah.indexSumber
                << tmpHasilOlah.deviceId
                << tmpHasilOlah.profilePe
                << hasilImage.data.count()
                << taskCreated - taskFinished
                ;
    }
    timerSaveResult->start(MS_SAVE_RESULT);
}

/* Constructor */
PrintEmulator::PrintEmulator(QObject *parent) : QObject(parent) {
    maxThread=PE_MAX_THREAD;
    queryLastIndex=0;

    timerProcessData = new QTimer(this);
    timerProcessData->setSingleShot(true);
    connect(timerProcessData, SIGNAL(timeout()), this, SLOT(ProcessData()));

    timerDeviceList = new QTimer(this);
    timerDeviceList->setSingleShot(true);
    connect(timerDeviceList, SIGNAL(timeout()), this, SLOT(RefreshDeviceList()));

    timerSaveResult = new QTimer(this);
    timerSaveResult->setSingleShot(true);
    connect(timerSaveResult, SIGNAL(timeout()), this, SLOT(SaveResult()));

    threadPool = new QThreadPool(this);
    threadPool->setMaxThreadCount(maxThread);
    taskCreated = 0;
    taskFinished = 0;
    taskSkipped = 0;

    noDataCounter = 0;
}

/* Destructor */
PrintEmulator::~PrintEmulator() {

}

/* Menjalankan inisialisasi Databases */
bool PrintEmulator::SetDatabase(int argc, char *argv[]) {
    if(argc<6) {
        return false;
    }

    bool isDriver = dbConnection.isDriverAvailable("QMYSQL");
    dbConnection = QSqlDatabase::addDatabase("QMYSQL");

    if(argc==6) {
        dbConnection.setHostName(argv[1]);
        dbConnection.setDatabaseName(argv[2]);
        dbConnection.setPort(QString::fromLocal8Bit(argv[3]).toInt());
        dbConnection.setUserName(argv[4]);
        dbConnection.setPassword(argv[5]);
    }

    return isDriver;
}

/* Melakukan koneksi ulang ke database apabila aplikasi putus koneksi dari DB */
bool PrintEmulator::ReconnectDatabase(void) {
    qDebug()<< "DB connection is lost!";
    dbConnection.close();
    int ReconnectCount=0;
    while(!dbConnection.isOpen()){
        QThread::sleep(10);
        qDebug()<<"Reconnect... " << ++ReconnectCount;
        dbConnection.open();
    }
    qDebug() << "DB connection is back.";

    return true;
}

/* Melakukan refresh terhadap data dari tabel DeviceList untuk mengecek adanya perubahan pada list Device */
void PrintEmulator::RefreshDeviceList(void) {
    QSqlQuery queryDevice("SELECT DeviceId, ProfilePE from DeviceTable",dbConnection);

    while(!queryDevice.exec()) {
        qDebug() << queryDevice.lastError().text();
        ReconnectDatabase();
    }

    while(queryDevice.next()) {
        DeviceProfile tmpProfile;
        tmpProfile.deviceId = queryDevice.value("DeviceId").toByteArray();
        tmpProfile.profilePE = queryDevice.value("ProfilePE").toInt();
        deviceList.insert(tmpProfile.deviceId,tmpProfile);
    }

    //todo: cleanup untuk deviceList entry yang sudah terhapus di DB
    timerDeviceList->start(MS_REFRESH_DEVICE_LIST); //refresh tiap 30 detik
}
