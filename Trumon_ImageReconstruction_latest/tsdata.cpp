#include "tsdata.h"


/* Cosntructor */
DataRaw::DataRaw()
{
    isDecompressed = false;
    isDecrypted = false;
}


/* Melakukan decompressing untuk file ekstensi CMP dan CRS */
void DataRaw::Decompress(void)
{
    if (!isDecompressed && (fileName.contains(".CMP") || fileName.contains(".CRS")))
    {
        int iterJ = 0;
        QByteArray olah;

        olah.clear();

        while(iterJ<fileContent.size()-3)
        {
            char p = fileContent.at(iterJ);
            if (p=='/'){
                int q = uint8_t(fileContent.at(iterJ+2));
                char s = fileContent.at(iterJ+1);
                QByteArray tmpByteArray(q, s);
                olah.append(tmpByteArray);
                iterJ+=2;
            } else {
                olah.append(p);
            }
            iterJ++;
        }
        while(iterJ<fileContent.size())
        {
            olah.append(fileContent.at(iterJ));
            iterJ++;
        }

        fileContent = olah;
        isDecompressed = true;
    }
}
