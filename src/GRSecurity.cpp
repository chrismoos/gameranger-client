/*
GameRanger
GRSecurity.cpp

Copyright (C) 2004 xoclipse (xoclipse@cox.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "GRSecurity.h"
#include "memdebug.h"

wxUint32 mulhwu(wxUint32 one, wxUint32 two)
{
	wxUint64 a, b, c;
	a = one;
	b = two;
	c = a * b;
	return c >> 32;
}
wxUint32 encryptVerifyCode(wxUint32 code)
{
	wxUint32 r0, r3, r4, temp;
	r4 = 0x4dbf4623;
	r3 = code ^ 0x22356929;
	r0 = r4;
	r0 = mulhwu(r0, r3);
	r0 >>= 0x0C;
	r0 *= 0x34af;
	r0 = r3 - r0;
	memcpy(&temp, (wxUint8*)&r0, 4);
	r3 += temp;
	return r3;
}
//--------------------------------------------------------------------------------------
wxUint32 Algorithm(char *code, char *MAC, int ID, int LAN_IP, int type)
{
        
        unsigned char r4[18] = {0x22,0x35,0x9D,0x5B,0xff,0xff,0xff,0xff,0x3F,0x57,0xFE,0x9B,0xff,0xff,0xff,0xff,0xff,0x54};

        r4[0] = code[0];
        r4[1] = code[1];
        r4[2] = code[2];
        r4[3] = code[3];
        r4[4] = ID >> 24;
        r4[5] = ID >> 16;
        r4[6] = ID >> 8;
        r4[7] = ID;

        r4[12] = MAC[0];
        r4[13] = MAC[1];
        r4[14] = MAC[2];
        r4[15] = MAC[3];
        r4[16] = MAC[4];
        r4[17] = MAC[5];

        if (type == 2) { //normal login type, load up the LAN IP
                r4[8] = 0xff^LAN_IP >> 24;
                r4[9] = 0xff^LAN_IP >> 16;
                r4[10] = 0xff^LAN_IP >> 8;
                r4[11] = 0xff^LAN_IP;
        }


        int inc;
        wxUint32 r0;
        wxUint32 r3;
        wxUint32 r5;
        wxUint32 r12 = 0xFFFFFFFF;


        inc = 0;

        for(r5 = 0; r5 < 4; r5++, inc++)//CHECKS PROCESS
        {
        r0 = r4[inc];
        r0 ^= r12;
        r0 &= 0x000000FF;

        r0 <<= 0x02;
        lwzx(&r0);
        r3 = r12 >> 0x08;
        r3 &= 0x00FFFFFF;
        r12 = r3 ^ r0;
        }

    r3 = r12 ^ 0xFFFF0000; //+06A4C 0C2EC78C   xoris      r3,r12,0xFFFF
    r3 ^= 0x0000FFFF;     // +06A50 0C2EC790   xori       r3,r3,0xFFFF

    r12 = r3 ^ 0xFFFF0000; //+06914 0C2EC654   xoris      r12,r3,0xFFFF
    r12 ^= 0x0000FFFF;     //+06918 0C2EC658   xori       r12,r12,0xFFFF

   // srwi r0,r5,0x03
   r0 = r5 >> 0x03;
   r0 &= 0x00FFFFFF;


    
     for(r5 = 0; r5 < 4; r5++, inc++)  //ACCOUNT ID PROCESS
        {
        r0 = r4[inc];
        r0 ^= r12;
        r0 &= 0x000000FF;

        r0 <<= 0x02;
        lwzx(&r0);
        r3 = r12 >> 0x08;
        r3 &= 0x00FFFFFF;
        r12 = r3 ^ r0;
        }

    r3 = r12 ^ 0xFFFF0000; //+06A4C 0C2EC78C   xoris      r3,r12,0xFFFF
    r3 ^= 0x0000FFFF;     // +06A50 0C2EC790   xori       r3,r3,0xFFFF

         // srwi r0,r5,0x03

    r12 = r3 ^ 0xFFFF0000; //+06914 0C2EC654   xoris      r12,r3,0xFFFF
    r12 ^= 0x0000FFFF;     //+06918 0C2EC658   xori       r12,r12,0xFFFF

    r0 = r5 >> 0x03;
    r0 &= 0x00FFFFFF;

     inc = 12;

     for(r5 = 0; r5 < 6; r5++, inc++)  //MAC address!!
        {
        r0 = r4[inc]; //should be 0x77
        r0 ^= r12;
        r0 &= 0x000000FF;

        r0 <<= 0x02;
        lwzx(&r0);
        r3 = r12 >> 0x08;
        r3 &= 0x00FFFFFF;
        r12 = r3 ^ r0;
        }
    r3 = r12 ^ 0xFFFF0000; //+06A4C 0C2EC78C   xoris      r3,r12,0xFFFF
    r3 ^= 0x0000FFFF;     // +06A50 0C2EC790   xori       r3,r3,0xFFFF

    if (type == 1) return r3; //register type process, we are done here
         // srwi r0,r5,0x03

    r12 = r3 ^ 0xFFFF0000; //+06914 0C2EC654   xoris      r12,r3,0xFFFF
    r12 ^= 0x0000FFFF;     //+06918 0C2EC658   xori       r12,r12,0xFFFF

      


    r0 = r5 >> 0x03;
    r0 &= 0x00FFFFFF;
   
    inc = 8;

       for(r5 = 0; r5 < 4; r5++, inc++) //LAN IP PROCESS!
        {
        r0 = r4[inc]; //goes back to 3F
        r0 ^= r12;
        r0 &= 0x000000FF;

        r0 <<= 0x02;
        lwzx(&r0);
        r3 = r12 >> 0x08;
        r3 &= 0x00FFFFFF;
        r12 = r3 ^ r0;
        }

    r3 = r12 ^ 0xFFFF0000; //+06A4C 0C2EC78C   xoris      r3,r12,0xFFFF
    r3 ^= 0x0000FFFF;     // +06A50 0C2EC790   xori       r3,r3,0xFFFF

   return r3;
}
//---------------------------------------------------------
void lwzx(wxUint32 * r0)
{

                unsigned char r6_memory[1536] = {
0x00,0x00,0x00,0x00,0x77,0x07,0x30,0x96,0xEE,0x0E,0x61,0x2C,0x99,0x09,0x51,0xBA,
0x07,0x6D,0xC4,0x19,0x70,0x6A,0xF4,0x8F,0xE9,0x63,0xA5,0x35,0x9E,0x64,0x95,0xA3,
0x0E,0xDB,0x88,0x32,0x79,0xDC,0xB8,0xA4,0xE0,0xD5,0xE9,0x1E,0x97,0xD2,0xD9,0x88,
0x09,0xB6,0x4C,0x2B,0x7E,0xB1,0x7C,0xBD,0xE7,0xB8,0x2D,0x07,0x90,0xBF,0x1D,0x91,
0x1D,0xB7,0x10,0x64,0x6A,0xB0,0x20,0xF2,0xF3,0xB9,0x71,0x48,0x84,0xBE,0x41,0xDE,
0x1A,0xDA,0xD4,0x7D,0x6D,0xDD,0xE4,0xEB,0xF4,0xD4,0xB5,0x51,0x83,0xD3,0x85,0xC7,
0x13,0x6C,0x98,0x56,0x64,0x6B,0xA8,0xC0,0xFD,0x62,0xF9,0x7A,0x8A,0x65,0xC9,0xEC,
0x14,0x01,0x5C,0x4F,0x63,0x06,0x6C,0xD9,0xFA,0x0F,0x3D,0x63,0x8D,0x08,0x0D,0xF5,
0x3B,0x6E,0x20,0xC8,0x4C,0x69,0x10,0x5E,0xD5,0x60,0x41,0xE4,0xA2,0x67,0x71,0x72,
0x3C,0x03,0xE4,0xD1,0x4B,0x04,0xD4,0x47,0xD2,0x0D,0x85,0xFD,0xA5,0x0A,0xB5,0x6B,
0x35,0xB5,0xA8,0xFA,0x42,0xB2,0x98,0x6C,0xDB,0xBB,0xC9,0xD6,0xAC,0xBC,0xF9,0x40,
0x32,0xD8,0x6C,0xE3,0x45,0xDF,0x5C,0x75,0xDC,0xD6,0x0D,0xCF,0xAB,0xD1,0x3D,0x59,
0x26,0xD9,0x30,0xAC,0x51,0xDE,0x00,0x3A,0xC8,0xD7,0x51,0x80,0xBF,0xD0,0x61,0x16,
0x21,0xB4,0xF4,0xB5,0x56,0xB3,0xC4,0x23,0xCF,0xBA,0x95,0x99,0xB8,0xBD,0xA5,0x0F,
0x28,0x02,0xB8,0x9E,0x5F,0x05,0x88,0x08,0xC6,0x0C,0xD9,0xB2,0xB1,0x0B,0xE9,0x24,
0x2F,0x6F,0x7C,0x87,0x58,0x68,0x4C,0x11,0xC1,0x61,0x1D,0xAB,0xB6,0x66,0x2D,0x3D,
0x76,0xDC,0x41,0x90,0x01,0xDB,0x71,0x06,0x98,0xD2,0x20,0xBC,0xEF,0xD5,0x10,0x2A,
0x71,0xB1,0x85,0x89,0x06,0xB6,0xB5,0x1F,0x9F,0xBF,0xE4,0xA5,0xE8,0xB8,0xD4,0x33,
0x78,0x07,0xC9,0xA2,0x0F,0x00,0xF9,0x34,0x96,0x09,0xA8,0x8E,0xE1,0x0E,0x98,0x18,
0x7F,0x6A,0x0D,0xBB,0x08,0x6D,0x3D,0x2D,0x91,0x64,0x6C,0x97,0xE6,0x63,0x5C,0x01,
0x6B,0x6B,0x51,0xF4,0x1C,0x6C,0x61,0x62,0x85,0x65,0x30,0xD8,0xF2,0x62,0x00,0x4E,
0x6C,0x06,0x95,0xED,0x1B,0x01,0xA5,0x7B,0x82,0x08,0xF4,0xC1,0xF5,0x0F,0xC4,0x57,
0x65,0xB0,0xD9,0xC6,0x12,0xB7,0xE9,0x50,0x8B,0xBE,0xB8,0xEA,0xFC,0xB9,0x88,0x7C,
0x62,0xDD,0x1D,0xDF,0x15,0xDA,0x2D,0x49,0x8C,0xD3,0x7C,0xF3,0xFB,0xD4,0x4C,0x65,
0x4D,0xB2,0x61,0x58,0x3A,0xB5,0x51,0xCE,0xA3,0xBC,0x00,0x74,0xD4,0xBB,0x30,0xE2,
0x4A,0xDF,0xA5,0x41,0x3D,0xD8,0x95,0xD7,0xA4,0xD1,0xC4,0x6D,0xD3,0xD6,0xF4,0xFB,
0x43,0x69,0xE9,0x6A,0x34,0x6E,0xD9,0xFC,0xAD,0x67,0x88,0x46,0xDA,0x60,0xB8,0xD0,
0x44,0x04,0x2D,0x73,0x33,0x03,0x1D,0xE5,0xAA,0x0A,0x4C,0x5F,0xDD,0x0D,0x7C,0xC9,
0x50,0x05,0x71,0x3C,0x27,0x02,0x41,0xAA,0xBE,0x0B,0x10,0x10,0xC9,0x0C,0x20,0x86,
0x57,0x68,0xB5,0x25,0x20,0x6F,0x85,0xB3,0xB9,0x66,0xD4,0x09,0xCE,0x61,0xE4,0x9F,
0x5E,0xDE,0xF9,0x0E,0x29,0xD9,0xC9,0x98,0xB0,0xD0,0x98,0x22,0xC7,0xD7,0xA8,0xB4,
0x59,0xB3,0x3D,0x17,0x2E,0xB4,0x0D,0x81,0xB7,0xBD,0x5C,0x3B,0xC0,0xBA,0x6C,0xAD,
0xED,0xB8,0x83,0x20,0x9A,0xBF,0xB3,0xB6,0x03,0xB6,0xE2,0x0C,0x74,0xB1,0xD2,0x9A,
0xEA,0xD5,0x47,0x39,0x9D,0xD2,0x77,0xAF,0x04,0xDB,0x26,0x15,0x73,0xDC,0x16,0x83,
0xE3,0x63,0x0B,0x12,0x94,0x64,0x3B,0x84,0x0D,0x6D,0x6A,0x3E,0x7A,0x6A,0x5A,0xA8,
0xE4,0x0E,0xCF,0x0B,0x93,0x09,0xFF,0x9D,0x0A,0x00,0xAE,0x27,0x7D,0x07,0x9E,0xB1,
0xF0,0x0F,0x93,0x44,0x87,0x08,0xA3,0xD2,0x1E,0x01,0xF2,0x68,0x69,0x06,0xC2,0xFE,
0xF7,0x62,0x57,0x5D,0x80,0x65,0x67,0xCB,0x19,0x6C,0x36,0x71,0x6E,0x6B,0x06,0xE7,
0xFE,0xD4,0x1B,0x76,0x89,0xD3,0x2B,0xE0,0x10,0xDA,0x7A,0x5A,0x67,0xDD,0x4A,0xCC,
0xF9,0xB9,0xDF,0x6F,0x8E,0xBE,0xEF,0xF9,0x17,0xB7,0xBE,0x43,0x60,0xB0,0x8E,0xD5,
0xD6,0xD6,0xA3,0xE8,0xA1,0xD1,0x93,0x7E,0x38,0xD8,0xC2,0xC4,0x4F,0xDF,0xF2,0x52,
0xD1,0xBB,0x67,0xF1,0xA6,0xBC,0x57,0x67,0x3F,0xB5,0x06,0xDD,0x48,0xB2,0x36,0x4B,
0xD8,0x0D,0x2B,0xDA,0xAF,0x0A,0x1B,0x4C,0x36,0x03,0x4A,0xF6,0x41,0x04,0x7A,0x60,
0xDF,0x60,0xEF,0xC3,0xA8,0x67,0xDF,0x55,0x31,0x6E,0x8E,0xEF,0x46,0x69,0xBE,0x79,
0xCB,0x61,0xB3,0x8C,0xBC,0x66,0x83,0x1A,0x25,0x6F,0xD2,0xA0,0x52,0x68,0xE2,0x36,
0xCC,0x0C,0x77,0x95,0xBB,0x0B,0x47,0x03,0x22,0x02,0x16,0xB9,0x55,0x05,0x26,0x2F,
0xC5,0xBA,0x3B,0xBE,0xB2,0xBD,0x0B,0x28,0x2B,0xB4,0x5A,0x92,0x5C,0xB3,0x6A,0x04,
0xC2,0xD7,0xFF,0xA7,0xB5,0xD0,0xCF,0x31,0x2C,0xD9,0x9E,0x8B,0x5B,0xDE,0xAE,0x1D,
0x9B,0x64,0xC2,0xB0,0xEC,0x63,0xF2,0x26,0x75,0x6A,0xA3,0x9C,0x02,0x6D,0x93,0x0A,
0x9C,0x09,0x06,0xA9,0xEB,0x0E,0x36,0x3F,0x72,0x07,0x67,0x85,0x05,0x00,0x57,0x13,
0x95,0xBF,0x4A,0x82,0xE2,0xB8,0x7A,0x14,0x7B,0xB1,0x2B,0xAE,0x0C,0xB6,0x1B,0x38,
0x92,0xD2,0x8E,0x9B,0xE5,0xD5,0xBE,0x0D,0x7C,0xDC,0xEF,0xB7,0x0B,0xDB,0xDF,0x21,
0x86,0xD3,0xD2,0xD4,0xF1,0xD4,0xE2,0x42,0x68,0xDD,0xB3,0xF8,0x1F,0xDA,0x83,0x6E,
0x81,0xBE,0x16,0xCD,0xF6,0xB9,0x26,0x5B,0x6F,0xB0,0x77,0xE1,0x18,0xB7,0x47,0x77,
0x88,0x08,0x5A,0xE6,0xFF,0x0F,0x6A,0x70,0x66,0x06,0x3B,0xCA,0x11,0x01,0x0B,0x5C,
0x8F,0x65,0x9E,0xFF,0xF8,0x62,0xAE,0x69,0x61,0x6B,0xFF,0xD3,0x16,0x6C,0xCF,0x45,
0xA0,0x0A,0xE2,0x78,0xD7,0x0D,0xD2,0xEE,0x4E,0x04,0x83,0x54,0x39,0x03,0xB3,0xC2,
0xA7,0x67,0x26,0x61,0xD0,0x60,0x16,0xF7,0x49,0x69,0x47,0x4D,0x3E,0x6E,0x77,0xDB,
0xAE,0xD1,0x6A,0x4A,0xD9,0xD6,0x5A,0xDC,0x40,0xDF,0x0B,0x66,0x37,0xD8,0x3B,0xF0,
0xA9,0xBC,0xAE,0x53,0xDE,0xBB,0x9E,0xC5,0x47,0xB2,0xCF,0x7F,0x30,0xB5,0xFF,0xE9,
0xBD,0xBD,0xF2,0x1C,0xCA,0xBA,0xC2,0x8A,0x53,0xB3,0x93,0x30,0x24,0xB4,0xA3,0xA6,
0xBA,0xD0,0x36,0x05,0xCD,0xD7,0x06,0x93,0x54,0xDE,0x57,0x29,0x23,0xD9,0x67,0xBF,
0xB3,0x66,0x7A,0x2E,0xC4,0x61,0x4A,0xB8,0x5D,0x68,0x1B,0x02,0x2A,0x6F,0x2B,0x94,
0xB4,0x0B,0xBE,0x37,0xC3,0x0C,0x8E,0xA1,0x5A,0x05,0xDF,0x1B,0x2D,0x02,0xEF,0x8D,
0x41,0xCD,0xCD,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x40,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x43,0x30,0x00,0x00,0x80,0x00,0x00,0x00,
0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xD0,0x00,0x00,0x00,0x00,0x00,0x00,
0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x43,0x30,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x3F,0xC5,0x55,0x55,0x55,0x55,0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x3F,0xAE,0xB8,0x51,0xEB,0x85,0x1E,0xB8,0x3F,0xA9,0x99,0x99,0x99,0x99,0x99,0x9A,
0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x07,0x06,0x06,0x05,0x05,0x05,0x05,
0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xC5,0x55,0x55,0x55,0x55,0x55,0x55,
0x3F,0x94,0x7A,0xE1,0x47,0xAE,0x14,0x7B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0xF0,0x00,0x00,
0x00,0x00,0x00,0x00,0x41,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x3C,
0x00,0x00,0x00,0x4C,0x49,0x4E,0x46,0x49,0x4E,0x49,0x54,0x59,0x00,0x00,0x00,0x00,
0x4E,0x41,0x4E,0x28,0x00,0x00,0x00,0x00,0x50,0x61,0x67,0x65,0x3A,0x20,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


   unsigned int range = sizeof(r6_memory);
   if (*r0+4 > range) {
        return;
   }
   *r0 =  r6_memory[*r0]*0x1000000 + r6_memory[*r0+1]*0x10000 + r6_memory[*r0+2] * 0x100 + r6_memory[*r0+3];
}
//-------------------------------------------------------------