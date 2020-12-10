#include "ticketcanvass.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

/* Constructor */
TicketCanvass::TicketCanvass(int width, int height) : QImage(width, height, QImage::Format_Mono) {
    this->fill(1);
    ySize = height;
    xSize = width;

    minX = 0;
    minY = 0;
    maxX = 0;
    maxY = 0;
}

void TicketCanvass::PutPixel(int x, int y, int color) {
    if(((x+64) <= xSize) && ((y+64) <= ySize)) {
        this->setPixel(x+32, y+32, uint(color));
        maxX = ((x+64) > maxX)? (x+64) : maxX;
        maxY = ((y+64) > maxY)? (y+64) : maxY;
    }
}

void TicketCanvass::DrawByteHorizontal(int x, int y, unsigned char bc, bool interlaced) {
    int intMul = (interlaced)? 2 : 1;

    if (bc&128) PutPixel(x+intMul*0, y, 0);
    if (bc&64)  PutPixel(x+intMul*1, y, 0);
    if (bc&32)  PutPixel(x+intMul*2, y, 0);
    if (bc&16)  PutPixel(x+intMul*3, y, 0);
    if (bc&8)   PutPixel(x+intMul*4, y, 0);
    if (bc&4)   PutPixel(x+intMul*5, y, 0);
    if (bc&2)   PutPixel(x+intMul*6, y, 0);
    if (bc&1)   PutPixel(x+intMul*7, y, 0);
}

void TicketCanvass::DrawByteVertical(int x, int y, unsigned char bc, bool interlaced)
{
    int intMul = (interlaced)? 2 : 1;

    if (bc&128) PutPixel(x, y+intMul*0, 0);
    if (bc&64)  PutPixel(x, y+intMul*1, 0);
    if (bc&32)  PutPixel(x, y+intMul*2, 0);
    if (bc&16)  PutPixel(x, y+intMul*3, 0);
    if (bc&8)   PutPixel(x, y+intMul*4, 0);
    if (bc&4)   PutPixel(x, y+intMul*5, 0);
    if (bc&2)   PutPixel(x, y+intMul*6, 0);
    if (bc&1)   PutPixel(x, y+intMul*7, 0);
}


void TicketCanvass::GetCanvass(QByteArray *ba, QByteArray *bb, int pembesaran, int blur)
{
    if (maxX <= minX)
    {
        PutPixel(32, 32, 1);
    }

    QBuffer buffer(ba);
    buffer.open(QIODevice::WriteOnly);

    QImage tmpImage = this->copy(minX, minY, maxX, maxY);

    tmpImage.scaled(maxX*pembesaran, maxY*pembesaran, Qt::KeepAspectRatio);
    QImage rawImage = tmpImage.convertToFormat(QImage::Format_Grayscale8);
    cv::Mat tmp(rawImage.height(), rawImage.width(), CV_8UC1, rawImage.bits(), static_cast<size_t>(rawImage.bytesPerLine()));
    cv::resize(tmp, tmp, cv::Size(0,0), 1.0, 1.618);
    QImage finImage(tmp.data, tmp.cols, tmp.rows, static_cast<int>(tmp.step), QImage::Format_Grayscale8);
    finImage.save(&buffer,"PNG");
    cv::GaussianBlur(tmp, tmp, cv::Size(blur,blur), 0);

    QBuffer buff(bb);
    buff.open(QIODevice::WriteOnly);

    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    ocr->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    ocr->SetImage(tmp.data, tmp.cols, tmp.rows, 1, static_cast<int>(tmp.step));
    buff.write(ocr->GetUTF8Text());
}
