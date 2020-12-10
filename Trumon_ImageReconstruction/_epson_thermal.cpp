#include "_epson_thermal.h"


/* Decoding untuk printer epson thermal 1 */
int _EpsonThermal::Decode(QByteArray *Data, TicketCanvass *ticketCanvass, int *Offset) {
    int x_iter = 0;
    int y_iter = 0;
    int x_pos = 0;
    int y_pos = 0;
    int x_offset = 0;
    int x_unit = 1;
    int y_unit = 1;
    int ptr = 0;
    int x_size = 0;
    int y_size = 0;
    int length = 0;

    uint8_t mode, x_low, x_high, y_low, y_high, pixel;
    int pixcount=0;
    ptr = (*Offset);
    while(ptr < Data->size()){
        if( (Data->at(ptr) == 0x1D) && (Data->at(ptr+1) == 0x38) && (Data->at(ptr+2) == 0x4C) &&  // GS 8 L
            (Data->at(ptr+7) == 0x30) && (Data->at(ptr+8) == 0x70)) {  //function 112
            x_low   = uint8_t(Data->at(ptr+3));
            x_high  = uint8_t(Data->at(ptr+4));
            y_low   = uint8_t(Data->at(ptr+5));
            y_high  = uint8_t(Data->at(ptr+6));
            length  = x_low + x_high * 256 + y_low * 65536 + y_high * 16777216;
            x_low   = uint8_t(Data->at(ptr+13));
            x_high  = uint8_t(Data->at(ptr+14));
            y_low   = uint8_t(Data->at(ptr+15));
            y_high  = uint8_t(Data->at(ptr+16));
            x_size  = x_high * 256 + x_low;
            y_size  = y_high * 256 + y_low;
            ptr     = ptr + 17;
            length  = length - 10;
            for(y_iter = 0; y_iter < y_size; y_iter++) {
                x_pos = x_offset;
                for(x_iter = 0; x_iter < x_size; x_iter += 8) {
                    if(ptr >= Data->size()){
                        break;
                    }
                    else if((Data->at(ptr) == 0x1D) && (Data->at(ptr+1) == 0x28) && (Data->at(ptr+2) == 0x4C)) {
                        break;
                    }

                    pixel = uint8_t(Data->at(ptr));
                    ptr++;
                    length--;
                    ticketCanvass->DrawByteHorizontal(x_pos,y_pos,pixel);
                    pixcount += 8;
                    x_pos += 8;
                }
                y_pos += 1;
            }
            x_offset = 0;
        }
        else if((Data->at(ptr) == 0x1D) && (Data->at(ptr+1) == 0x76) && (Data->at(ptr+2) == 0x30)) {  // GS v 0
            mode    = uint8_t(Data->at(ptr+3));
            x_low   = uint8_t(Data->at(ptr+4));
            x_high  = uint8_t(Data->at(ptr+5));
            y_low   = uint8_t(Data->at(ptr+6));
            y_high  = uint8_t(Data->at(ptr+7));
            x_size  = x_high * 256 + x_low;
            y_size  = y_high * 256 + y_low;
            ptr     = ptr + 8;
            for(y_iter = 0; y_iter < y_size; y_iter++) {
                x_pos = x_offset;
                for(x_iter = 0; x_iter < x_size; x_iter++) {
                    if(ptr >= Data->size()) {
                        break;
                    }

                    pixel = uint8_t(Data->at(ptr));
                    ptr++;
                    ticketCanvass->DrawByteHorizontal(x_pos,y_pos,pixel);
                    pixcount += 8;
                    x_pos += 8;
                }
                y_pos += 1;
            }
            x_offset = 0;
        }
        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x2A)) { // ESC *
            mode    = uint8_t(Data->at(ptr+2));
            x_low   = uint8_t(Data->at(ptr+3));
            x_high  = uint8_t(Data->at(ptr+4));
            x_size  = (x_high * 256 + x_low);
            ptr     = ptr+5;
            x_pos   = x_offset;
            for(x_iter = 0; x_iter < x_size; x_iter++) {
                if(mode <= 0x01) {
                    if(ptr >= Data->size()) {
                        break;
                    }
                    else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)){ // ESC J (printing)
                        break;
                    }

                    pixel = uint8_t(Data->at(ptr));
                    ptr++;
                    if(mode == 0x01) {
                        ticketCanvass->DrawByteVertical(x_pos, y_pos, pixel, true);
                    }
                    else {
                        ticketCanvass->DrawByteVertical(x_pos, y_pos, pixel, false);
                    }
                    pixcount += 8;
                    x_pos++;
                }
                else if(mode >= 0x20 && mode <= 0x21) {
                    for(y_iter = 0; y_iter < 3; y_iter++) {
                        if(ptr >= Data->size()) {
                            break;
                        }
                        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)){ // ESC J (printing)
                            break;
                        }

                        pixel = uint8_t(Data->at(ptr));
                        ptr++;
                        if(mode == 0x21) {
                            ticketCanvass->DrawByteVertical(x_pos, y_pos+y_iter*16, pixel, true);
                        }
                        else {
                            ticketCanvass->DrawByteVertical(x_pos, y_pos+y_iter*8, pixel, false);
                        }
                        pixcount += 8;
                    }
                    x_pos++;
                }
            }
            x_offset = 0;
        }
        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)) {
            y_pos  += (Data->at(ptr+2))*y_unit;
            ptr += 3;
        }
        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x24)) {
            x_low   = uint8_t(Data->at(ptr+2));
            x_high  = uint8_t(Data->at(ptr+3));
            x_offset= (x_high * 256 + x_low)*x_unit;
            ptr += 4;
        }
        else if((Data->at(ptr) == 0x1D) && (Data->at(ptr+1) == 0x50)) {
            x_unit  = uint8_t(Data->at(ptr+2));
            if(x_unit == 0) {
                x_unit = 1;
            }
            y_unit  = uint8_t(Data->at(ptr+3));
            if(y_unit == 0) {
                y_unit = 1;
            }
            ptr += 4;
        }
        else if((Data->at(ptr) == 0x1D) && (Data->at(ptr+1) == 0x56)) { // GS V
            ptr += 4;
            if(pixcount != 0) {
                (*Offset) = ptr;
                break;
            }
        }
        else {
            ptr++;
        }
    }

    if((*Offset) == 0) {
        (*Offset) = Data->size();
    }
    if(pixcount==0) {
        ticketCanvass->PutPixel(10,10,1);
    }

    return pixcount;
}
