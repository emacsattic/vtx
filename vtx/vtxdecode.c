/*
 * vtxdecode.c: Routines to decode VTX-pages & convert from magic-cookie-VTX-attributes to
 *              'normal' attributes usable by x_vtx_redraw()
 *
 * $Id: vtxdecode.c,v 1.2 1997/03/26 00:16:54 mb Exp mb $
 *
 * Copyright (c) 1994-96 Martin Buck  <martin-2.buck@student.uni-ulm.de>
 * Read COPYING for more information
 *
 */

#include <stdio.h>
#include "vtx_assert.h"
#include "cct.h"
#include "vtxtools.h"
#include "misc.h"
#include "vtxdecode.h"


/* Translation tables to convert VTX font-layout (with different national charsets) to
 * X-Window font layout
 */
const chr_t cct2vtx_table[8][96] = {
  /* English */
  { 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f },
  /* French */
  { 0x20,0x21,0x22,0Xc0,0Xc1,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0Xc2,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0Xc3,0Xc4,0Xc5,0Xc6,0x5f,
    0Xc7,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0Xc8,0Xc9,0Xca,0Xcb,0x7f },
  /* Swedish */
  { 0x20,0x21,0x22,0X5f,0Xcc,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0Xcd,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0Xce,0Xcf,0Xd0,0Xd1,0Xd2,
    0Xc0,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0Xd3,0Xd4,0Xd5,0Xd6,0x7f },
  /* Unknown */
  { 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f },
  /* German */
  { 0x20,0x21,0x22,0X5f,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0Xd7,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0Xce,0Xcf,0Xd1,0Xd8,0Xd2,
    0Xd9,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0Xd3,0Xd4,0Xd6,0Xda,0x7f },
  /* Spanish */
  { 0x20,0x21,0x22,0Xcb,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0Xcb,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0Xde,0Xc0,0Xdf,0Xe2,0Xe3,
    0Xdc,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0Xd6,0Xdd,0Xc7,0Xc2,0x7f },
  /* Italian */
  { 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0Xc0,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0Xd9,0Xcb,0x5d,0x5e,0x5f,
    0Xc5,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0Xc2,0Xe0,0Xc7,0Xe1,0x7f },
  /* Unknown */
  { 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f }
};


/* Translation table to convert VideoteXt-X-Window font-layout to ISO 8859-1.
 * You should have a complete ISO-Font when editing the following table!
 */
const char vtx2iso_table[256] =
  "                                "
  " !\"�$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ<�>^#"
  "-abcdefghijklmnopqrstuvwxyz�|��#"
  "                                "
  "                                "
  "�����������������_�����^�ߡ����"
  "����                          ? ";
/* The same table with all characters mapped to lowercase */
const char vtx2iso_lc_table[256] =
  "                                "
  " !\"�$%&'()*+,-./0123456789:;<=>?"
  "@abcdefghijklmnopqrstuvwxyz<�>^#"
  "-abcdefghijklmnopqrstuvwxyz�|��#"
  "                                "
  "                                "
  "�����������������_�����^�ߡ����"
  "����                          ? ";
/* All word delimiters in videotext's subset of ISO 8859-1 */
const char vtxiso_worddelim[] = " !\"�$%&'()*+,-./:;<=>?@<�>^#-�|���_�^���";


/* Decode lines y1 - y2 of original VTX-page in pgbuf to vtxpage usable by VideoteXt using
 * national charset lang
 * Return VTXOK if OK, VTXEINVAL if start-/end-line invalid
 */
int
decode_page(const byte_t *pgbuf, vtxpage_t *page, int y1, int y2) {
  int line, col, pos, graphics, grhold, doubleht, nextattr = 0;
  static int lang;
  attrib_t *lastattr, default_attrib = 7, next_attrib;
  chr_t chr, *lastchr, default_chr = ' ';
  
  if (y1 > y2 || y1 < 0 || y2 > 23)
    return VTXEINVAL;

  if (!page->info.hamming)
    lang = page->info.charset;

  pos = y1 * 40;
  doubleht = 0;
  for (line = y1; line <= y2; line++) {
    lastchr = &default_chr;
    lastattr = &default_attrib;
    graphics = grhold = 0;
    if (doubleht) {
      for (col = 0; col <= 39; col++) {
        if (page->attrib[pos - 40] & VTX_DOUBLE1) {
          page->chr[pos] = page->chr[pos - 40];
          page->chr[pos - 40] = ' ';
          page->attrib[pos] = (page->attrib[pos - 40] & ~VTX_DOUBLE1) | VTX_DOUBLE2;
        } else {
          page->chr[pos] = ' ';
          page->attrib[pos] = page->attrib[pos - 40];
        }
        pos++;
      }
      doubleht = 0;
    } else {
      for (col = 0; col <= 39; col++) {
        chr = pgbuf[pos];
        if (!vtx_chkparity(&chr)) {
          page->chr[pos] = 254;					/* Parity error */
          page->attrib[pos] = 7;
        } else if (chr >= 32 && chr <= 127) {			/* Normal character */
          page->attrib[pos] = *lastattr;
          if (!graphics || (chr >= 64 && chr <= 95)) {
            page->chr[pos] = cct2vtx_table[lang][chr - 32];
          } else {
            page->chr[pos] = chr + (chr >= 96 ? 64 : 96);
          }
        } else {
          page->chr[pos] = ((grhold && graphics ) ? *lastchr : ' ');
          if (chr <= 7) {					/* Set alphanumerics-color */
            page->attrib[pos] = *lastattr;
            next_attrib = (*lastattr & ~(VTX_COLMASK | VTX_HIDDEN)) + chr;
            nextattr = 1;
            graphics = 0;
          } else if (chr == 8 || chr == 9) {			/* Flash on/off */
            page->attrib[pos] = (*lastattr & ~VTX_FLASH) + VTX_FLASH * (chr == 8);
          } else if (chr == 10 || chr == 11) {			/* End/start box */
            page->attrib[pos] = (*lastattr & ~VTX_BOX) + VTX_BOX * (chr == 11);
          } else if (chr == 12 || chr == 13) {			/* Normal/double height */
            page->attrib[pos] = (*lastattr & ~VTX_DOUBLE1) + VTX_DOUBLE1 * (chr == 13);
            if (chr == 13)
              doubleht = 1;
          } else if (chr == 14 || chr == 15 || chr == 27) {	/* SO, SI, ESC (ignored) */
            page->attrib[pos] = *lastattr;
          } else if (chr >= 16 && chr <= 23) {			/* Set graphics-color */
            page->attrib[pos] = *lastattr;
            next_attrib = (*lastattr & ~(VTX_COLMASK | VTX_HIDDEN)) + chr - 16;
            nextattr = 1;
            graphics = 1;
          } else if (chr == 24) {				/* Conceal display */
            page->attrib[pos] = *lastattr | VTX_HIDDEN;
          } else if (chr == 25 || chr == 26) {			/* Contiguous/separated graphics */
            page->attrib[pos] = (*lastattr & ~VTX_GRSEP) + VTX_GRSEP * (chr == 26);
          } else if (chr == 28) {				/* Black background */
            page->attrib[pos] = *lastattr & ~VTX_BGMASK;
          } else if (chr == 29) {				/* Set background */
            page->attrib[pos] = (*lastattr & ~VTX_BGMASK) + ((*lastattr & VTX_COLMASK) << 3);
          } else if (chr == 30 || chr == 31) {			/* Hold/release graphics */
            page->attrib[pos] = *lastattr;
            grhold = (chr == 30);
            if (grhold && graphics)
              page->chr[pos] = *lastchr;
          } else {
            assert(0);
          }
        }
        lastchr = page->chr + pos;
        if (nextattr) {
          lastattr = &next_attrib;
          nextattr = 0;
        } else {
          lastattr = page->attrib + pos;
        }
        pos++;
      }
    }
  }
  return VTXOK;
}