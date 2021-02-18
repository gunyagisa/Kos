int main(void)
{
  unsigned char *vram = (unsigned char *) 0xb8000;
  for (int i = 0; i < 640; ++i) {
    for (int j = 0; j < 480; ++i) {
      vram[i + j * 640] = 0x00;
    }
  }
}
