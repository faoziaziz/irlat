#include "ticketcanvass.h"


/* Constructor */
TicketCanvass::TicketCanvass(int width, int height) : QImage(width, height, QImage::Format_Mono)
{
    this->fill(1);
    ySize = height;
    xSize = width;

    minX = width;
    minY = height;
    maxX = 0;
    maxY = 0;

    ganjil = false;
    genap = false;
}

void TicketCanvass::PutPixel(int x, int y, int color)
{
    if((x<xSize) && (y<ySize))
    {
        this->setPixel(x,y,uint(color));
        minX = (x<minX)?x:minX;
        minY = (y<minY)?y:minY;
        maxX = (x>maxX)?x:maxX;
        maxY = (y>maxY)?y:maxY;
    }
    (y % 2 == 0)? genap=true : ganjil=true;
}

void TicketCanvass::DrawByteHorizontal(int x, int y, unsigned char bc, bool interlaced)
{
    int intMul;
    (interlaced)?intMul=2:intMul=1;
    if (bc&128) PutPixel(x  ,y,0);
    if (bc&64)  PutPixel(x+intMul*1,y,0);
    if (bc&32)  PutPixel(x+intMul*2,y,0);
    if (bc&16)  PutPixel(x+intMul*3,y,0);
    if (bc&8)   PutPixel(x+intMul*4,y,0);
    if (bc&4)   PutPixel(x+intMul*5,y,0);
    if (bc&2)   PutPixel(x+intMul*6,y,0);
    if (bc&1)   PutPixel(x+intMul*7,y,0);
}

void TicketCanvass::DrawByteVertical(int x, int y, unsigned char bc, bool interlaced)
{
    int intMul;
    (interlaced)?intMul=2:intMul=1;
    if (bc&128) PutPixel(x,y  ,0);
    if (bc&64)  PutPixel(x,y+intMul*1,0);
    if (bc&32)  PutPixel(x,y+intMul*2,0);
    if (bc&16)  PutPixel(x,y+intMul*3,0);
    if (bc&8)   PutPixel(x,y+intMul*4,0);
    if (bc&4)   PutPixel(x,y+intMul*5,0);
    if (bc&2)   PutPixel(x,y+intMul*6,0);
    if (bc&1)   PutPixel(x,y+intMul*7,0);
}


void TicketCanvass::GetCanvass(QByteArray *ba, int pembesaran, bool deInterlace)
{

    if (maxX <= minX)
    {
        PutPixel(100, 100, 1);
    }

    QBuffer buffer(ba);
    buffer.open(QIODevice::WriteOnly);
    int mX = (minX > 5)? minX-4:0;
    int mY = (minY > 5)? minY-4:0;
    int nX = (maxX > xSize-5)? xSize:maxX+4;
    int nY = (maxY > ySize-5)? ySize:maxY+4;

    QImage tmpImage = this->copy(mX, mY, (nX-mX)+1, (nY-mY)+1);

    if (deInterlace && !(!ganjil && !genap))
    {
        for (int Y = 1; Y<tmpImage.height(); Y+=2)
            for (int X = 0; X<tmpImage.width(); X++)
            {
                char p,q;
                (tmpImage.pixel(X,Y-1)==-1)?p=1:p=0;
                (tmpImage.pixel(X,Y)==-1)?q=1:q=0;
                tmpImage.setPixel(X,Y, p & q);
            }
    }

    tmpImage.scaled((nX-mX+1)*pembesaran, (nY-mY+1)*pembesaran, Qt::KeepAspectRatio).save(&buffer,"PNG");
}
