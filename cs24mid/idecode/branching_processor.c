/*! \file
 *
 * This file contains the definitions for building and simulating a
 * branching processor.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "branching_processor.h"
#include "branching_control.h"



/*!
 * This constant specifies the longest that a program may execute before the
 * processor terminates.  Since this processor includes branching, we can't just
 * run until we hit the instruction store depth; instead, we set a "max execute
 * time," beyond which we assume that the program has a bug like an infinite
 * loop in it.
 */
#define MAX_EXECUTE_TIME 1000


/*!
 * Allocates and assembles a branching processor.  The result should be freed
 * by the free_processor() function, since virtually all processor state is
 * heap-allocated.
 */
Processor * build_processor() {
    Processor *proc;
    ProgramCounter *pc;
    InstructionStore *is;
    Decode *decode;
    RegisterFile *rf;
    ALU *alu;
    BranchUnit *bru;

    proc = malloc(sizeof(Processor));
    if (!proc) {
        fprintf(stderr, "Out of memory allocating processor!\n");
        exit(11);
    }
    memset(proc, 0, sizeof(Processor));

    // Build the components.
    pc     = proc->pc     = build_program_counter();
    is     = proc->is     = build_instruction_store();
    decode = proc->decode = build_decode();
    rf     = proc->rf     = build_register_file();
    alu    = proc->alu    = build_alu();
    bru    = proc->bru    = build_branch_unit();

    // Wire up the components into a processor.

    proc->pc_bus = connect(&pc->pc_pin,        &is->address_input);
    proc->instr  = connect(&is->output,        &decode->input);
    proc->cpuop  = connect3(&decode->cpuop,    &alu->op, &bru->op);

    proc->src1   = connect(&decode->src1_addr, &rf->src1_addr);
    proc->src1_isreg = connect(&decode->src1_isreg, &alu->in2_reg);
    proc->src1_const = connect(&decode->src1_const, &alu->const2);

    proc->src2   = connect(&decode->src2_addr, &rf->src2_addr);

    proc->dst       = connect(&decode->dst_addr,  &rf->dst_addr);
    proc->dst_write = connect(&decode->dst_write, &rf->writep);

    proc->rf1    = connect(&rf->src1, &alu->in2);
    proc->rf2    = connect(&rf->src2, &alu->in1);

    proc->aluout    = connect(&alu->out,    &rf->dst);
    proc->alustatus = connect(&alu->status, &bru->status);

    proc->branch_addr = connect(&decode->branch_addr, &pc->branch_addr);
    proc->branch      = connect(&bru->branch,         &pc->branch);

    // Reset the program counter to 0.
    reset(proc->pc);

    return proc;
}


/*! Deallocates the processor struct and all of its internal components. */
void free_processor(Processor *proc) {
    // Destroy the components
    free_program_counter(proc->pc);
    free_instruction_store(proc->is);
    free_decode(proc->decode);
    free_register_file(proc->rf);
    free_alu(proc->alu);
    free_branch_unit(proc->bru);

    // Destroy each bus
    free_bus(proc->pc_bus);
    free_bus(proc->instr);
    free_bus(proc->cpuop);

    free_bus(proc->src1);
    free_bus(proc->src1_isreg);
    free_bus(proc->src1_const);

    free_bus(proc->src2);

    free_bus(proc->dst);
    free_bus(proc->dst_write);

    free_bus(proc->rf1);
    free_bus(proc->rf2);

    free_bus(proc->aluout);
    free_bus(proc->alustatus);

    free_bus(proc->branch_addr);
    free_bus(proc->branch);

    // Get rid of the actual Processor struct.
    free(proc);
}


/*!
 * Starts running the processor with the current contents of the instruction
 * store and the register file.  The function terminates when the processor hits
 * the ALUOP_DONE instruction.
 */
void run(Processor *proc) {
    int t;

    printf("Running processor.\n\n");

    printf("T=0\tRegister File: ");
    rfprint(stdout, proc->rf);

    /* Since we have branching, we can't just run until we hit the instruction
     * store depth.  Instead, we set a "max execute time," beyond which we
     * assume that the program has an infinite loop in it.
     */
    for (t = 1; t < MAX_EXECUTE_TIME; t++) {
        busdata_t pc = bus_read(proc->pc_bus);
        clock(proc);

        printf("\nT=%d\tPC=%02d\tCPUOP=0x%X\tSRC1=%X (%s)\tSRC2=%X\n"
               "\tW=%X\tDST=%X\tBRA=%X\n"
               "\tRF1=%X\tRF2=%X\tALUOUT=%X\tALUSTAT=%X\tBR?=%X\n",
               t,
               pc,
               bus_read(proc->cpuop),

               (bus_read(proc->src1_isreg) ? bus_read(proc->src1) :
                                             bus_read(proc->src1_const)),
               (bus_read(proc->src1_isreg) ? "reg" : "const"),

               bus_read(proc->src2),

               bus_read(proc->dst_write),
               bus_read(proc->dst),

               bus_read(proc->branch_addr),
               bus_read(proc->rf1),
               bus_read(proc->rf2),
               bus_read(proc->aluout),
               bus_read(proc->alustatus),
               bus_read(proc->branch)
              );
        printf("\n\tRegister File: ");
        rfprint(stdout, proc->rf);

        if (bus_read(proc->cpuop) == OP_DONE)
            break;
    }

    printf("\n");
    if (t == MAX_EXECUTE_TIME) {
        printf("ERROR:  Max execute time reached.\n"
               "Does your program have an infinite loop in it?\n");
    }
    else {
        printf("Program terminated normally.\n");
    }
}


