struct framebuffer {
  unsigned long long base;
  unsigned long long size;
  unsigned long long x_size;
  unsigned long long y_size;
  unsigned int       pps;
} fb;

int kernel_init(struct framebuffer *_fb)
{
  fb.base = _fb->base;
  fb.size = _fb->size;
  fb.x_size = _fb->x_size;
  fb.y_size = _fb->y_size;
  fb.pps = _fb->pps;

  unsigned int pixel = 0xffffffff;
  unsigned int *vram = (unsigned int *)fb.base;
  for (int y = 0; y < fb.y_size; ++y) {
    for (int x = 0; x < fb.x_size; ++x) {
      vram[x + y * fb.x_size] = pixel;
    }
  }

  for (;;) ;
}
