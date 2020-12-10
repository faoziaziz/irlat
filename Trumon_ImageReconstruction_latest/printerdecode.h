#pragma once

#include <QObject>
#include "ticketcanvass.h"

class PrinterDecode
{
public:
    PrinterDecode();
    ~PrinterDecode();

    virtual int Decode(QByteArray *Data, TicketCanvass *ticketCanvass, int *Offset) = 0;

    bool deInterlace;
};
