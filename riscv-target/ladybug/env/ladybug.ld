OUTPUT_ARCH("riscv")

MEMORY {
  lspm (rw)   : ORIGIN = 0x00000000, LENGTH = 2k
  aram (rw)   : ORIGIN = 0x00200000, LENGTH = 32
  mmio (rx)   : ORIGIN = 0x00300000, LENGTH = 2k
  cram (rx)   : ORIGIN = 0x00400000, LENGTH = 2m
  sigram (rw) : ORIGIN = 0x00700000, LENGTH = 4k
}
SECTIONS
{
  .lspm : { *(.lsmp) } > lspm
  .text.init : { *(.text.init) } > cram
  . = ALIGN(0x0004);
  .text : { *(.text) } > cram
  . = ALIGN(0x0004);
  .data : { *(.data) } > cram
  . = ALIGN(0x0004);
  .data.string : { *(.data.string) } > cram
  .tohost : { *(.tohost) } > mmio
  .bss : { *(.bss) } > aram
  .signature : { *(.signature) } > sigram
  _end = .;
}
