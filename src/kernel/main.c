#include "font.h"
#include <stdint.h>
#include "gdt.h"

extern void io_hlt(void);
extern void io_out8(uint16_t port, uint8_t data);
extern uint8_t io_in8(uint16_t port);
extern void loadIDT(uint16_t limit, uint64_t offset);

struct framebuffer {
  uint64_t base;
  uint64_t size;
  uint64_t x_size;
  uint64_t y_size;
  uint32_t pps;
} fb; 

struct Pixel {
  uint32_t blue;
  uint32_t green;
  uint32_t red;
  uint32_t alpha;
};

struct InterruptFrame;
struct Pixel pixel = { 0xff, 0xff, 0xff, 0xff };

void clear_screen();
void draw_chr(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t c);
void draw_str(uint32_t x, uint32_t y, struct Pixel color, const char *str);
void init_interrupt(void);
void init_keyboard(void);
void init_pit(void);


void keyboard_handler(struct InterruptFrame *frame);
void timer_handler(struct InterruptFrame *frame);

int kernel_start(struct framebuffer *_fb)
{
  fb.base = _fb->base;
  fb.size = _fb->size;
  fb.x_size = _fb->x_size;
  fb.y_size = _fb->y_size;
  fb.pps = _fb->pps;

  clear_screen();
  init_interrupt();
  draw_str(0, 0, pixel, "Hello World");

  draw_str(0, 16, pixel, "finish initialization");

  for (;;) io_hlt();
}


#define GDT_SIZE 16
struct SegmentDescriptor GDT[GDT_SIZE];
void set_gdt(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flag)
{
  GDT[index].base_low = base & 0x0000ffff;
  GDT[index].base_mid = (base >> 16) & 0x0000ff;
  GDT[index].base_high = (base >> 24) & 0x0000ff;

  GDT[index].limit_low = limit & 0x0000ffff;
  GDT[index].limit_high = limit >> 16;

  GDT[index].access = access;
  GDT[index].flag = flag & 0x0000000f;
} 

struct {
  uint16_t size;
  uint64_t offset;
} __attribute__((packed)) gdtr;

void init_gdt(void)
{
  // null descriptor
  set_gdt(0, 0, 0, 0, 0);
  // code descriptor for kernel
  // 1010
  set_gdt(1, 0, 0xffffffff, 0x9A, 6);
  // data descriptor for kernel
  set_gdt(2, 0, 0xffffffff, 0x92, 0);
  // TODO:TSS

  gdtr.size = sizeof(GDT) - 1;
  gdtr.offset = (uint64_t)&GDT;

  asm volatile (
      ".intel_syntax noprefix\n"
      "lgdt     gdtr\n"
      "mov      ds, ax\n"
      "mov      es, ax\n"
      "mov      ss, ax\n" 
      ::"a"(0x10):"memory");

  unsigned short selector = 8;
  unsigned long long dummy;
  __asm__ ("pushq %[selector];"
      "leaq ret_label(%%rip), %[dummy];"
      "pushq %[dummy];"
      "lretq;"
      "ret_label:"
      : [dummy]"=r"(dummy)
      : [selector]"m"(selector));
}

// INTERRUPT
// 

struct IDTGateDescriptor { 
  uint16_t offset_low; 
  uint16_t selector;
  uint8_t zero1;
  uint8_t type_attr; 
  uint16_t offset_mid;
  uint32_t offset_high;
  uint32_t zero2;
} __attribute__((packed));

struct {
  uint16_t limit;
  uint64_t offset;
} __attribute__((packed)) idtr;

struct IDTGateDescriptor IDT[256];

void init_idt(void)
{
  loadIDT(sizeof(IDT) - 1, (uint64_t)&IDT);
}

void init_interrupt(void)
{
  io_out8(0x20, 0x11);
  io_out8(0xA0, 0x11);
  io_out8(0x21, 0x20);
  io_out8(0xA1, 0x28);
  io_out8(0x21, 0x04);
  io_out8(0xA1, 0x02);
  io_out8(0x21, 0x01);
  io_out8(0xA1, 0x01);

  // mask all interrupts
  io_out8(0x21, 0xff);
  io_out8(0xA1, 0xff);

  //pit
  init_pit();

  init_idt();

  set_idt(32, (uint64_t *)&timer_handler, 0x38);
  set_idt(33, (uint64_t *)&keyboard_handler, 0x38);

  uint8_t mask = io_in8(0x21);
  mask &= ~0x03;
  io_out8(0x21, mask);
  asm ("sti");
}

void set_idt(uint8_t num, uint64_t *handler, uint16_t cs)
{
  uint64_t p = (uint64_t) handler;
  IDT[num].offset_low = (uint16_t)p;
  IDT[num].offset_mid = (uint16_t)(p >> 16);
  IDT[num].offset_high = (uint32_t)(p >> 32);

  IDT[num].selector = cs;
  // type=0xe, S=0, DPL=00, P=1
  IDT[num].type_attr = 0x8e;

  IDT[num].zero1 = 0;
  IDT[num].zero2 = 0;
}

void draw_str(uint32_t x, uint32_t y, struct Pixel color, const char *str)
{
  while (*str != 0) {
    draw_chr(x, y, color.red, color.green, color.blue, *(str++));
    x += 8;
  }
}

void clear_screen()
{
  uint32_t *vram = (uint32_t *)fb.base;
  for (int y = 0; y < fb.y_size; ++y) {
    for (int x = 0; x < fb.x_size; ++x) {
      vram[x + y * fb.x_size] = 0xff000000;
    }
  }
}

void draw_pixel (uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t *vram = (uint32_t *) fb.base;
  struct PIXEL {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t yokuwakarann;
  } pixel;

  pixel.red = r;
  pixel.green = g;
  pixel.blue = b;
  pixel.yokuwakarann = 0xff;

  uint32_t *color = (uint32_t *)&pixel; 

  vram[x + y * fb.x_size] = *color;
}

void draw_chr(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t c)
{
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 8; ++j) {
      if ((font[c * 16 + i] & (0x01 << (7 - j))) != 0) {
        draw_pixel(x + j, y + i, r, g, b);
      }
    }
  }
}

void init_pit()
{
  io_out8(0x43, 0x34);
  io_out8(0x40, 0x9c);
  io_out8(0x40, 0x2e);
}

int x = 0;
__attribute__((interrupt))
void timer_handler(struct InterruptFrame *frame)
{
  io_out8(0x20, 0x20);
  draw_str(x, 64, pixel, "B");
  x += 8;
}

void init_keyboard()
{
  io_out8(0x60, 0xF0);
  uint8_t data = io_in8(0x60);
  io_out8(0x60, 0x2);

  data = io_in8(0x64);
  data &= 0x3f;
  io_out8(0x64, 0x60);
  io_out8(0x64, data); 

  io_out8(0x60,0xF0);
  data = io_in8(0x60);
  io_out8(0x60, 0x0);

  if (data == 0x02) {
    draw_str(x, 48, pixel, "scan code:02");
  }

}

int y = 0;
__attribute__((interrupt))
void keyboard_handler(struct InterruptFrame *frame)
{
  io_out8(0x20, 0x20);
  uint8_t data = io_in8(0x60);
  if (data == 0x1C) {
    draw_str(y, 32, pixel, "A");
    y += 8;
  }
}
