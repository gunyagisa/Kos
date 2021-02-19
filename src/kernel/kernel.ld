OUTPUT_FORMAT("binary");

SECTIONS
{
  . = 0x12000;
  .text : { *(.text) }

  .rodata : {
    *(.strings)
    *(.rodata)
    *(.rodata.*)
  }

  .data : { *(.data) }
  .bss : { *(.bss) }
}
