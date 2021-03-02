#include <stdint.h>
#ifndef GDT_H_
#define GDT_H_
struct SegmentDescriptor {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t limit_high: 4;
  uint8_t flag: 4;
  uint8_t base_high;
} __attribute__((packed));

void init_gdt(void);
void set_idt(uint8_t num, uint64_t *interrupt_handler);
#endif
