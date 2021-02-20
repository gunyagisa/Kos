OUTPUT_FORMAT("binary");

MEMORY
{
  HEAD(r) : ORIGIN = 0x00000000, LENGTH = 1m
  RAM(wx) : ORIGIN = 0x12000000, LENGTH = 960k
}

SECTIONS
{
  .header : {
    QUAD(__bss_start)
    QUAD(__bss_end - __bss_start)
  } > HEAD

  .body : {
    *(.text)
    *(.rodata)
    *(.data)
    __bss_start = .;
    *(.bss)
    __bss_end = .;

    *(.eh_frame)
  } > RAM AT> HEAD

  /DISCARD/ : {
    *(.note.gnu.property)
  }
}
