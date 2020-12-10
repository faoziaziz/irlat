#pragma once

#include <QDebug>

#include "printerdecode.h"


class _EpsonESCP9PIN : public PrinterDecode
{
public:
    int Decode(QByteArray *Data, TicketCanvass* ticketCanvass, int *Offset);
};
