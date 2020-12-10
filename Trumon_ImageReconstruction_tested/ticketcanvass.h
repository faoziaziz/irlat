#pragma once

#include <QImage>
#include <QPainter>
#include <QBuffer>

class TicketCanvass : public QImage
{
public:
    TicketCanvass(int width, int height);

    void PutPixel(int x, int y, int color);
    void DrawByteVertical(int x, int y, unsigned char bc, bool interlaced = false);
    void DrawByteHorizontal(int x, int y, unsigned char bc, bool interlaced = false);

    void GetCanvass(QByteArray *ba);
    void GetCanvass(QByteArray *ba, QByteArray *bb, int pembesaran = 1, int blur = 9);

private:
    int minX, minY, maxX, maxY;
    int xSize;
    int ySize;
};
