#include "_epson_escp2.h"

/* Decoding untuk printer epson LQ310 */
int _EpsonESCP2::Decode(QByteArray *Data, TicketCanvass *ticketCanvass, int *Offset) {
    int x_iter = 0;
    int y_iter = 0;
    int x_pos = 0;
    int y_pos = 0;
    int x_offset = 0;
    int unit = 1;
    int ptr = 0;
    int n_size = 0;

    uint8_t mode, n_low, n_high, pixel, error;
    int pixcount = 0;

    mode = 0x28;
    ptr = (*Offset);
    while(ptr < Data->size()){
        // ESC * (Select bit-image mode)
        if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x2A)) {
            if(Data->at(ptr+2) == 0x40 && Data->at(ptr+3) == 0x40) {
                ptr += 6;
            }

            mode    = uint8_t(Data->at(ptr+2));
            if(Data->at(ptr+3) == 0x40 && Data->at(ptr+4) == 0x40) {
                ptr += 6;
            }

            n_low   = uint8_t(Data->at(ptr+3));
            if(Data->at(ptr+4)==0x40 && Data->at(ptr+5)==0x40) {
                ptr += 6;
            }

            n_high  = uint8_t(Data->at(ptr+4));
            if(Data->at(ptr+4)==0x40 && Data->at(ptr+5)==0x40) {
                ptr += 6;
            }

            n_size  = (n_high*256+n_low);
            ptr     = ptr+5;
            if(mode == 0x00) {
                x_pos = int(x_offset*60/3600);
            }
            else if(mode == 0x01) {
                x_pos = int(x_offset*120/3600);
            }
            else if(mode == 0x02) {
                x_pos = int(x_offset*120/3600);
            }
            else if(mode == 0x03) {
                x_pos = int(x_offset*240/3600);
            }
            else if(mode == 0x04) {
                x_pos = int(x_offset*80/3600);
            }
            else if(mode == 0x06) {
                x_pos = int(x_offset*90/3600);
            }
            else if(mode == 0x20) {
                x_pos = int(x_offset*60/3600);
            }
            else if(mode == 0x21) {
                x_pos = int(x_offset*120/3600);
            }
            else if(mode == 0x26) {
                x_pos = int(x_offset*90/3600);
            }
            else if(mode == 0x27) {
                x_pos = int(x_offset*180/3600);
            }
            else if(mode == 0x28) {
                x_pos = int(x_offset*360/3600);
            }
            x_offset = 0;
            for(x_iter = 0; x_iter < n_size; x_iter++) {
                if(mode <= 0x06) {
                    do {
                        if( Data->at(ptr)==0x00 && Data->at(ptr+1)==0x00 && Data->at(ptr+2)==0x00 &&
                            Data->at(ptr+3)==0x0D && Data->at(ptr+4)==0x01 &&
                            Data->at(ptr+5)==0x00 && Data->at(ptr+6)==0x04) {
                            ptr += 7;
                            error = 1;
                        }
                        else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                Data->at(ptr+2)==0x10 && Data->at(ptr+3)==0x06 &&
                                Data->at(ptr+4)==0x00 && Data->at(ptr+5)==0x00) {
                            ptr += 6;
                            error = 1;
                        }
                        else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                Data->at(ptr+4)==0x00 && Data->at(ptr+5)==0x01) {
                            ptr += 6;
                            error = 1;
                        }
                        else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                Data->at(ptr+2)==0x00 && uint8_t(Data->at(ptr+3))==0x80 &&
                                uint8_t(Data->at(ptr+4))==0xFF && uint8_t(Data->at(ptr+5))==0xFF) {
                            ptr += 6;
                            error = 1;
                        }
                        else {
                            error = 0;
                        }
                    } while(error == 1);

                    if(ptr >= Data->size()) {
                        break;
                    }
                    else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)){ // ESC J (printing)
                        break;
                    }
                    else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x2A)){ // ESC * (new image)
                        break;
                    }

                    pixel = uint8_t(Data->at(ptr));
                    ptr++;
                    ticketCanvass->DrawByteVertical(x_pos, y_pos, pixel, false);
                    pixcount += 8;
                    x_pos++;
                }
                else if(mode >= 0x20 && mode <= 0x28) {
                    for(y_iter = 0; y_iter < 3; y_iter++) {
                        do {
                            if( Data->at(ptr)==0x00 && Data->at(ptr+1)==0x00 && Data->at(ptr+2)==0x00 &&
                                Data->at(ptr+3)==0x0D && Data->at(ptr+4)==0x01 &&
                                Data->at(ptr+5)==0x00 && Data->at(ptr+6)==0x04) {
                                ptr += 7;
                                error = 1;
                            }
                            else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                    Data->at(ptr+2)==0x10 && Data->at(ptr+3)==0x06 &&
                                    Data->at(ptr+4)==0x00 && Data->at(ptr+5)==0x00) {
                                ptr += 6;
                                error = 1;
                            }
                            else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                    Data->at(ptr+4)==0x00 && Data->at(ptr+5)==0x01) {
                                ptr += 6;
                                error = 1;
                            }
                            else if(Data->at(ptr)==0x40 && Data->at(ptr+1)==0x40 &&
                                    Data->at(ptr+2)==0x00 && uint8_t(Data->at(ptr+3))==0x80 &&
                                    uint8_t(Data->at(ptr+4))==0xFF && uint8_t(Data->at(ptr+5))==0xFF) {
                                ptr += 6;
                                error = 1;
                            }
                            else {
                                error = 0;
                            }
                        } while(error == 1);

                        if(ptr >= Data->size()) {
                            break;
                        }
                        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)){ // ESC J (printing)
                            break;
                        }
                        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x2A)){ // ESC * (new image)
                            break;
                        }

                        pixel = uint8_t(Data->at(ptr));
                        ptr++;
                        ticketCanvass->DrawByteVertical(x_pos, y_pos+y_iter*8, pixel, false);
                        pixcount += 8;
                    }
                    x_pos++;
                }
            }
        }
        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x24)) { // ESC $
            n_low   = uint8_t(Data->at(ptr+2));
            n_high  = uint8_t(Data->at(ptr+3));
            x_offset= (n_high*256+n_low)*unit;
            ptr += 4;
        }
        else if((Data->at(ptr) == 0x1B) && (Data->at(ptr+1) == 0x4A)) {
            if(mode == 0x00) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x01) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x02) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x03) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x04) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x06) {
                y_pos += int(Data->at(ptr+2)*60/180);
            }
            else if(mode == 0x20) {
                y_pos += int(Data->at(ptr+2)*180/180);
            }
            else if(mode == 0x21) {
                y_pos += int(Data->at(ptr+2)*180/180);
            }
            else if(mode == 0x26) {
                y_pos += int(Data->at(ptr+2)*180/180);
            }
            else if(mode == 0x27) {
                y_pos += int(Data->at(ptr+2)*180/180);
            }
            else if(mode == 0x28) {
                y_pos += int(Data->at(ptr+2)*180/180);
            }
            ptr += 3;
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
