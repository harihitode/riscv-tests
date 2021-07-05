OUTPUT_ARCH("riscv")

MEMORY {
  lspm (rw)   : ORIGIN = 0x00000000, LENGTH = 2k
  mmio (rx)   : ORIGIN = 0x00300000, LENGTH = 2k
  cram (rx)   : ORIGIN = 0x00400000, LENGTH = 2m
  sigram (rw) : ORIGIN = 0x00700000, LENGTH = 4k
}
SECTIONS
{
  .lspm : { *(.lsmp) } > lspm
  .text.init : { *(.text.init) } > cram
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) } > mmio
  .text : { *(.text) } > cram
  . = ALIGN(0x1000);
  .data : { *(.data) } > cram
  .data.string : { *(.data.string) } > cram
  .bss : { *(.bss) } > cram
  .signature : { *(.signature) } > sigram
  _end = .;
}
