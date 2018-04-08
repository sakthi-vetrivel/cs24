/*! \file
 *
 * This file contains definitions for an Arithmetic/Logic Unit of an
 * emulated processor.
 */


#include <stdio.h>
#include <stdlib.h>   /* malloc(), free() */
#include <string.h>   /* memset() */

#include "alu.h"
#include "instruction.h"


/*!
 * This function dynamically allocates and initializes the state for a new ALU
 * instance.  If allocation fails, the program is terminated.
 */
ALU * build_alu() {
    /* Try to allocate the ALU struct.  If this fails, report error then exit. */
    ALU *alu = malloc(sizeof(ALU));
    if (!alu) {
        fprintf(stderr, "Out of memory building an ALU!\n");
        exit(11);
    }

    /* Initialize all values in the ALU struct to 0. */
    memset(alu, 0, sizeof(ALU));
    return alu;
}


/*! This function frees the dynamically allocated ALU instance. */
void free_alu(ALU *alu) {
    free(alu);
}


/*!
 * This function implements the logic of the ALU.  It reads the inputs and
 * opcode, then sets the output accordingly.  Note that if the ALU does not
 * recognize the opcode, it should simply produce a zero result.
 */
void alu_eval(ALU *alu) {
    uint32_t A, B, aluop;
    uint32_t result;

    A = pin_read(alu->in1);
    B = pin_read(alu->in2);
    aluop = pin_read(alu->op);

    result = 0;

    switch (aluop) {

      case ALUOP_ADD:
        result = A + B;
        break;

      case ALUOP_INV:
        result = ~A;
        break;

      case ALUOP_SUB:
        result = A - B;
        break;

      case ALUOP_AND:
        result = A & B;
        break;

      case ALUOP_OR:
        result = A | B;
        break;

      case ALUOP_XOR:
        result = A ^ B;
        break;

      case ALUOP_SLL:
      case ALUOP_SLA:
        result = A << 1;
        break;

      case ALUOP_SRL:
        result = A >> 1;
        break;

      case ALUOP_SRA:
        result = (uint32_t) ((int32_t) A >> 1);
        break;

      case ALUOP_INCR:
        result = A + 1;
        break;

      default:
        result = 0;
        break;
    }

    pin_set(alu->out, result);
}
