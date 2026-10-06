/*
 * isa.h - the instruction set of the mini computer.
 *
 * The compiler writes these numbers into bytecode and the processor
 * reads them back, so both include this one file.
 *
 * Every instruction is 4 bytes:   | opcode | dest | src1 | src2 |
 *
 * How the three fields are used:
 *
 *   Math          dest = result register
 *                 src1 = first operand (register)
 *                 src2 = second operand (register number, or the
 *                        constant when the opcode ends in _CONST)
 *   Move          dest = target register, src1 = 0,
 *                 src2 = source register or constant
 *   Read          dest = target register, src1 = 0,
 *                 src2 = address (register number, or constant address)
 *   Write         dest = address (register number, or constant address)
 *                 src1 = 0, src2 = register holding the value to store
 *   Print         dest = 0, src1 = 0, src2 = register to print
 *   Branch        dest = 0, src1 = 0, src2 = jump distance in
 *                 instructions (a signed number, can be negative)
 *   Vector math   like Math, but dest and src1 are vector registers.
 *                 src2 depends on the opcode name: _VEC = vector
 *                 register, _SCALAR = integer register, _CONST = constant.
 *   Vector memory like Read and Write, but the value register is a
 *                 vector register.
 *
 * Rule: for every opcode that has a _CONST form,
 *       the _CONST opcode = the register opcode + 0x08.
 */

#ifndef ISA_H
#define ISA_H

#include <stdint.h>

/* ---------- Sizes of the machine ---------- */

#define NUM_INT_REGS      256   /* integer registers x0 .. x255          */
#define NUM_VECTOR_REGS    32   /* vector registers  v0 .. v31           */
#define VECTOR_LANES        8   /* numbers held by one vector register   */
#define WORD_SIZE           4   /* bytes in one number (32 bits)         */
#define INSTRUCTION_SIZE    4   /* bytes in one instruction              */
#define MAX_CONSTANT      255   /* biggest constant that fits one byte   */

/* ---------- Opcodes ---------- */

typedef enum {
    OP_HALT         = 0x00,   /* stop the program */

    /* Integer math: result = src1 (operator) src2 */
    OP_ADD_REG      = 0x01,
    OP_SUB_REG      = 0x02,
    OP_MUL_REG      = 0x03,
    OP_DIV_REG      = 0x04,

    /* Integer memory and data movement */
    OP_READ_REG     = 0x05,   /* x1 = [x2]  */
    OP_WRITE_REG    = 0x06,   /* [x2] = x1  */
    OP_MOVE_REG     = 0x07,   /* x1 = x2    */
    OP_PRINT        = 0x08,   /* print x1   */

    /* Same operations when the second operand is a constant */
    OP_ADD_CONST    = 0x09,
    OP_SUB_CONST    = 0x0A,
    OP_MUL_CONST    = 0x0B,
    OP_DIV_CONST    = 0x0C,
    OP_READ_CONST   = 0x0D,   /* x1 = [40]  */
    OP_WRITE_CONST  = 0x0E,   /* [40] = x1  */
    OP_MOVE_CONST   = 0x0F,   /* x1 = 25    */

    /* Branches: the real opcode is OP_BRANCH_BASE + a condition code */
    OP_BRANCH_BASE  = 0x10,   /* 0x10 .. 0x1E */

    /* Vector math, second operand is a vector register */
    OP_VADD_VEC     = 0x21,
    OP_VSUB_VEC     = 0x22,
    OP_VMUL_VEC     = 0x23,

    /* Vector memory */
    OP_VREAD_REG    = 0x25,   /* v1 = [x2]  */
    OP_VWRITE_REG   = 0x26,   /* [x2] = v1  */

    /* Vector math, second operand is a constant */
    OP_VADD_CONST   = 0x29,
    OP_VSUB_CONST   = 0x2A,
    OP_VMUL_CONST   = 0x2B,

    /* Vector memory with a constant address */
    OP_VREAD_CONST  = 0x2D,   /* v1 = [32]  */
    OP_VWRITE_CONST = 0x2E,   /* [32] = v1  */

    /* Vector math, second operand is an integer register */
    OP_VADD_SCALAR  = 0x31,
    OP_VSUB_SCALAR  = 0x32,
    OP_VMUL_SCALAR  = 0x33
} Opcode;

/* ---------- Branch conditions ---------- */

typedef enum {
    COND_EQ = 0,    /* Z set            equal                    */
    COND_NE,        /* Z clear          not equal                */
    COND_CS,        /* C set            unsigned higher or same  */
    COND_CC,        /* C clear          unsigned lower           */
    COND_MI,        /* N set            negative                 */
    COND_PL,        /* N clear          positive or zero         */
    COND_VS,        /* V set            overflow                 */
    COND_VC,        /* V clear          no overflow              */
    COND_HI,        /* C set, Z clear   unsigned higher          */
    COND_LS,        /* C clear or Z set unsigned lower or same   */
    COND_GE,        /* N equals V       signed greater or equal  */
    COND_LT,        /* N differs from V signed less than         */
    COND_GT,        /* Z clear, N == V  signed greater than      */
    COND_LE,        /* Z set or N != V  signed less or equal     */
    COND_AL         /* always                                    */
} Condition;

/* ---------- One decoded instruction ---------- */

typedef struct {
    uint8_t opcode;
    uint8_t dest;
    uint8_t src1;
    uint8_t src2;
} Instruction;

#endif /* ISA_H *
