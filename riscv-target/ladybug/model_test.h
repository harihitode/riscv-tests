#ifndef _COMPLIANCE_MODEL_H
#define _COMPLIANCE_MODEL_H

#define LADYBUG_CSR_WORK_ITEM_X 0xcd6
#define LADYBUG_CSR_FORCE_QUIT  0xcd9
#define LADYBUG_CSR_DUMP        0xcda
#define LADYBUG_CSR_HEXDUMP     0xcdb
#define MEMORY_MAPPED_O_ADDR    0x00300000

#define RVMODEL_DATA_SECTION                                            \
        .pushsection .tohost,"aw",@progbits;                            \
        .align 8; .global tohost; tohost: .dword 0;                     \
        .align 8; .global fromhost; fromhost: .dword 0;                 \
        .popsection;                                                    \
        .pushsection .lspm;                                             \
        .align 8; .global begin_regstate; begin_regstate:               \
        .word 128;                                                      \
        .align 8; .global end_regstate; end_regstate:                   \
        .word 4;                                                        \
        .popsection;

#define RVMODEL_HALT                                              \
  csrrsi x0, LADYBUG_CSR_FORCE_QUIT, 0x1;

//TODO: declare the start of your signature region here. Nothing else to be used here.
// The .align 4 ensures that the signature ends at a 16-byte boundary
#define RVMODEL_DATA_BEGIN                                              \
  .pushsection .signature,"aw",@progbits;                               \
  .align 4; .global begin_signature; begin_signature:

//TODO: declare the end of the signature region here. Add other target specific contents here.
// The .align 4 ensures that the signature begins at a 16-byte boundary
#define RVMODEL_DATA_END                                                      \
  .global end_signature; end_signature:                                 \
  .popsection;                                                          \
  RVMODEL_DATA_SECTION

// There is no initialization for ladybug (empty)
#define RVMODEL_IO_INIT

// For code that has a split rom/ram area
#define RVMODEL_BOOT      \
  .section .text.init;    \
  csrrw x1, LADYBUG_CSR_WORK_ITEM_X, x0;        \
  csrrw x0, LADYBUG_CSR_DUMP, x1;                               \
  LOCAL_IO_WRITE_STR("RISC-V Compliance Test for Ladybug.\n");

// _SP = (volatile register)
#define LOCAL_IO_WRITE_STR(_STR) RVMODEL_IO_WRITE_STR(x31, _STR)
#define RVMODEL_IO_WRITE_STR(_SP, _STR)                                 \
    .section .data.string;                                              \
20001:                                                                  \
    .string _STR;                                                       \
    .section .text.init;                                                \
    la a0, 20001b;                                                      \
    la a1, FN_WriteStr;                                                 \
    jalr a1;

#define RSIZE 4
// _SP = (volatile register)
#define LOCAL_IO_PUSH(_SP)                                              \
    la      _SP,  begin_regstate;                                       \
    sw      ra,   (1*RSIZE)(_SP);                                       \
    sw      t0,   (2*RSIZE)(_SP);                                       \
    sw      t1,   (3*RSIZE)(_SP);                                       \
    sw      t2,   (4*RSIZE)(_SP);                                       \
    sw      t3,   (5*RSIZE)(_SP);                                       \
    sw      t4,   (6*RSIZE)(_SP);                                      \
    sw      s0,   (7*RSIZE)(_SP);                                      \
    sw      a0,   (8*RSIZE)(_SP);

// _SP = (volatile register)
#define LOCAL_IO_POP(_SP)                                               \
    la      _SP,   begin_regstate;                                      \
    lw      ra,   (1*RSIZE)(_SP);                                       \
    lw      t0,   (2*RSIZE)(_SP);                                       \
    lw      t1,   (3*RSIZE)(_SP);                                       \
    lw      t2,   (4*RSIZE)(_SP);                                       \
    lw      t3,   (5*RSIZE)(_SP);                                       \
    lw      t4,   (6*RSIZE)(_SP);                                       \
    lw      s0,   (7*RSIZE)(_SP);                                       \
    lw      a0,   (8*RSIZE)(_SP);

//RVMODEL_IO_ASSERT_GPR_EQ
// _SP = (volatile register)
// _R = GPR
// _I = Immediate
// This code will check a test to see if the results
// match the expected value.
// It can also be used to tell if a set of tests is still running or has crashed
// Test to see if a specific test has passed or not.  Can assert or not.
#define RVMODEL_IO_ASSERT_GPR_EQ(_SP, _R, _I)                                 \
    LOCAL_IO_PUSH(_SP)                                                  \
    mv          s0, _R;                                                 \
    li          t5, _I;                                                 \
    beq         s0, t5, 20002f;                                         \
    LOCAL_IO_WRITE_STR("Test Failed ");                              \
    LOCAL_IO_WRITE_STR(": ");                                        \
    LOCAL_IO_WRITE_STR(# _R);                                        \
    LOCAL_IO_WRITE_STR("( ");                                        \
    mv      a0, s0;                                                     \
    jal FN_WriteNmbr;                                                   \
    LOCAL_IO_WRITE_STR(" ) != ");                                    \
    mv      a0, t5;                                                     \
    jal FN_WriteNmbr;                                                   \
    j 20003f;                                                           \
20002:                                                                  \
    LOCAL_IO_WRITE_STR("Test Passed ");                              \
20003:                                                                  \
    LOCAL_IO_WRITE_STR("\n");                                        \
    LOCAL_IO_POP(_SP)

.section .text
// FN_WriteStr: Add code here to write a string to IO
// FN_WriteNmbr: Add code here to write a number (32/64bits) to IO
FN_WriteStr:                             \
csrrw t0, LADYBUG_CSR_WORK_ITEM_X, x0;   \
bne t0, x0, FN_WriteStr_END;             \
li a1, MEMORY_MAPPED_O_ADDR;             \
FN_WriteStr_BEGIN:                       \
lbu t0, 0x0(a0);                         \
beq t0, x0, FN_WriteStr_END;             \
sb t0, 0x0(a1);                          \
addi a0, a0, 0x1;                        \
j FN_WriteStr_BEGIN;                     \
FN_WriteStr_END:                         \
addi a0, x0, 0x0;                        \
ecall;                                   \
wfi;                                     \
ret;                                     \
FN_WriteNmbr:                            \
csrrw x0, LADYBUG_CSR_DUMP, a0;          \
ret;

//RVTEST_IO_ASSERT_SFPR_EQ
#define RVMODEL_IO_ASSERT_SFPR_EQ(_F, _R, _I)
//RVTEST_IO_ASSERT_DFPR_EQ
#define RVMODEL_IO_ASSERT_DFPR_EQ(_D, _R, _I)

// TODO: specify the routine for setting machine software interrupt
#define RVMODEL_SET_MSW_INT

// TODO: specify the routine for clearing machine software interrupt
#define RVMODEL_CLEAR_MSW_INT

// TODO: specify the routine for clearing machine timer interrupt
#define RVMODEL_CLEAR_MTIMER_INT

// TODO: specify the routine for clearing machine external interrupt
#define RVMODEL_CLEAR_MEXT_INT

#endif // _COMPLIANCE_MODEL_H
