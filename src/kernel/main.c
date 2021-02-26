#include "font.h"

extern void io_hlt(void);

struct framebuffer {
  unsigned long long base;
  unsigned long long size;
  unsigned long long x_size;
  unsigned long long y_size;
  unsigned int       pps;
} fb;

struct Pixel {
  unsigned int blue;
  unsigned int green;
  unsigned int red;
  unsigned int alpha;
};

void clear_screen();
void draw_chr(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char c);
void draw_str(unsigned int x, unsigned int y, struct Pixel color, const char *str);
void init_gdt(void);

int kernel_init(struct framebuffer *_fb)
{
  fb.base = _fb->base;
  fb.size = _fb->size;
  fb.x_size = _fb->x_size;
  fb.y_size = _fb->y_size;
  fb.pps = _fb->pps;

  init_gdt();
  clear_screen();

  struct Pixel pixel = { 0xff, 0xff, 0xff, 0xff };
  draw_str(0, 0, pixel, "Hello World");

  for (;;) io_hlt();
}

void init_gdt(void)
{
}

void draw_str(unsigned int x, unsigned int y, struct Pixel color, const char *str)
{
  while (*str != 0) {
    draw_chr(x, y, color.red, color.green, color.blue, *(str++));
    x += 8;
  }
}

void clear_screen()
{
  unsigned int *vram = (unsigned int *)fb.base;
  for (int y = 0; y < fb.y_size; ++y) {
    for (int x = 0; x < fb.x_size; ++x) {
      vram[x + y * fb.x_size] = 0xff000000;
    }
  }
}

void draw_pixel (unsigned x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
{
  unsigned int *vram = (unsigned int *) fb.base;
  struct PIXEL {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char yokuwakarann;
  } pixel;
  pixel.red = r;
  pixel.green = g;
  pixel.blue = b;
  pixel.yokuwakarann = 0xff;

  unsigned int *color = (unsigned int *)&pixel; 

  vram[x + y * fb.x_size] = *color;
}

void draw_chr(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char c)
{
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 8; ++j) {
      if ((font[c * 16 + i] & (0x01 << (7 - j))) != 0) {
        draw_pixel(x + j, y + i, r, g, b);
      }
    }
  }

}
