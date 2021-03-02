#include "font.h"
#include <stdint.h>
#include "gdt.h"

// PIC port number
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC0_OCW1               0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1
#define PIC1_OCW1               0x00a1

extern void io_hlt(void);
extern void io_out8(uint16_t port, uint8_t data);
extern uint8_t io_in8(uint16_t port);
extern uint32_t intr1();

struct framebuffer {
  uint64_t base;
  uint64_t size;
  uint64_t x_size;
  uint64_t y_size;
  uint32_t pps;
} fb; struct Pixel {
  uint32_t blue;
  uint32_t green;
  uint32_t red;
  uint32_t alpha;
};

void clear_screen();
void draw_chr(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t c);
void draw_str(uint32_t x, uint32_t y, struct Pixel color, const char *str);
void init_interrupt(void);
void init_keyboard(void);

int kernel_start(struct framebuffer *_fb)
{
  fb.base = _fb->base;
  fb.size = _fb->size;
  fb.x_size = _fb->x_size;
  fb.y_size = _fb->y_size;
  fb.pps = _fb->pps;

  struct Pixel pixel = { 0xff, 0xff, 0xff, 0xff };
  clear_screen();
  draw_str(0, 0, pixel, "Hello World");

  init_gdt();
  set_idt(33, (uint64_t *)&intr1);
  init_interrupt();

  uint8_t mask = io_in8(PIC0_OCW1);
  mask &= ~0x02;
  io_out8(PIC0_OCW1, mask);

  asm ("sti");

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
  idtr.limit = sizeof(IDT) - 1;
  idtr.offset = (uint64_t)IDT;

  asm volatile ("lidt %0" ::"m" (idtr));
}

void init_interrupt(void)
{
  io_out8(PIC0_ICW1, 0x11);
  io_out8(PIC0_ICW2, 0x20);
  io_out8(PIC0_ICW3, 0x04);
  io_out8(PIC0_ICW4, 0x01);
  io_out8(PIC0_OCW1, 0xff);

  io_out8(PIC1_ICW1, 0x11);
  io_out8(PIC1_ICW2, 0x28);
  io_out8(PIC1_ICW3, 0x02);
  io_out8(PIC1_ICW4, 0x01);
  io_out8(PIC1_OCW1, 0xff);

  init_idt();
}

void set_idt(uint8_t num, uint64_t *handler)
{
  uint64_t p = (uint64_t) handler;
  IDT[num].offset_low = (uint16_t)p;
  IDT[num].offset_mid = (uint16_t)(p >> 16);
  IDT[num].offset_high = (uint32_t)(p >> 32);

  IDT[num].selector = 0x08;
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

void intr1_handler(void)
{
  struct Pixel pixel = { 0xff, 0xff, 0xff, 0xff };
  draw_str(0, 8, pixel, "A");
}
