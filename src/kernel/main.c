struct framebuffer {
  unsigned long long base;
  unsigned long long size;
  unsigned long long x_size;
  unsigned long long y_size;
  unsigned int       pps;
};

int kernel_init(struct framebuffer *fb)
{
  unsigned int pixel = 0xffffffff;
  for (int y = 0; y < fb->y_size; ++y) {
    for (int x = 0; x < fb->x_size; ++x) {
      *((unsigned int*)(fb->base + 4 * x + 4 * y * fb->pps)) = pixel;
    }
  }

  for (;;) ;
}
