#pragma once

#include <QDebug>

#include "printerdecode.h"


class _EpsonThermalFull : public PrinterDecode
{
public:
    int Decode(QByteArray *Data, TicketCanvass* ticketCanvass, int *Offset);
};
