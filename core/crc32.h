//
// Created by nelson on 6/29/19.
//

#ifndef CANON_CRC32_H
#define CANON_CRC32_H


#include <zconf.h>

/*
 * Sources:
 *      http://www.ross.net/crc/download/crc_v3.txt
 *      https://docs.microsoft.com/en-us/openspecs/office_protocols/ms-abs/06966aa2-70da-4bf9-8448-3355f277cd77
 * */

u_int32_t crc32_text(unsigned  char* text, ssize_t len);

u_int32_t crc32_bin(unsigned char* text, ssize_t len);

#endif //CANON_CRC32_H
