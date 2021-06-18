#include "vga.h"
#include <kernel/ports.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t count) {
  const char *sp = (const char *)src;
  char *dp = (char *)dest;
  for (; count != 0; count--)
    *dp++ = *sp++;
  return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val,
                        size_t count) {
  unsigned short *temp = (unsigned short *)dest;
  for (; count != 0; count--)
    *temp++ = val;
  return dest;
}

size_t strlen(const char *string) {
  size_t length = 0;

  while (string[length]) {
    length++;
  }

  return length;
}

unsigned short *textmemptr;
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void scroll() {
  unsigned blank, temp;

  blank = 0x20 | (attrib << 8);

  if (csr_y >= 25) {
    temp = csr_y - 25 + 1;
    memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);
    memsetw(textmemptr + (25 - temp) * 80, blank, 80);
    csr_y = 25 - 1;
  }
}

void move_csr() {
  unsigned temp;

  temp = csr_y * 80 + csr_x;

  outp(0x3D4, 14);
  outp(0x3D5, temp >> 8);
  outp(0x3D4, 15);
  outp(0x3D5, temp);
}

void cls() {
  unsigned blank;
  int i;

  blank = 0x20 | (attrib << 8);

  for (i = 0; i < 25; i++) {
    memsetw(textmemptr + i * 80, blank, 80);
  }

  csr_x = 0;
  csr_y = 0;
  move_csr();
}

void putch(unsigned char c) {
  unsigned short *where;
  unsigned att = attrib << 8;

  if (c == 0x08) {
    if (csr_x != 0) {
      csr_x--;
    }
  }

  else if (c == 0x09) {
    csr_x = (csr_x + 8) & ~(8 - 1);
  }

  else if (c == '\r') {
    csr_x = 0;
  }

  else if (c == '\n') {
    csr_x = 0;
    csr_y++;
  }

  else if (c >= ' ') {
    where = textmemptr + (csr_y * 80 + csr_x);
    *where = c | att;
    csr_x++;
  }

  if (csr_x >= 80) {
    csr_x = 0;
    csr_y++;
  }

  scroll();
  move_csr();
}

void writestr(const char *fmt, ...) {
  va_list ap;
  char *p, *sval;
  int ival;

  va_start(ap, fmt);

  for (p = fmt; *p; p++) {
    if (*p != '%') {
      putch(*p);
      continue;
    }

    switch (*++p) {
    case 'd':
      ival = va_arg(ap, int);
      putch(ival);
      break;
    case 's':
      for (sval = va_arg(ap, char *); *sval; sval++) {
        putch(*sval);
      }
      break;
    default:
      putch(*p);
      break;
    }
  }
}

void writeint(int num) {
  int a = num;
  int i = 0;
  char c[30];

  if (num == 0) {
    putch("0");
    return;
  }

  while (a > 0) {
    c[i] = '0' + a % 10;
    a /= 10;
    i++;
  }

  c[i] = 0;

  char c2[32];
  c2[i--] = 0;
  int j = 0;

  while (i >= 0) {
    c2[i--] = c[j++];
  }
  writestr(c2);
}

void settextcolor(unsigned char forecolor, unsigned char backcolor) {
  attrib = (backcolor << 4) | (forecolor & 0x0F);
}

void init_vga() {
  textmemptr = (unsigned short *)0xB8000;
  cls();
}
