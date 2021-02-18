OUTPUT_FORMAT("binary");

SECTIONS
{
  . = 0x12000;
  .text : { *(.text) }

  .rodata : {
    *(.rodata)
    *(.rodata.*)
  }

  .data : { *(.data) }
}
