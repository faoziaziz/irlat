#pragma once

#include <QDebug>

#include "printerdecode.h"


class _EpsonESCP2 : public PrinterDecode
{
public:
    int Decode(QByteArray *Data, TicketCanvass* ticketCanvass, int *Offset);
};
