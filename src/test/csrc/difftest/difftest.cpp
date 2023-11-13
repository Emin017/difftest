/***************************************************************************************
* Copyright (c) 2020-2021 Institute of Computing Technology, Chinese Academy of Sciences
* Copyright (c) 2020-2021 Peng Cheng Laboratory
*
* XiangShan is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "difftest.h"
#include "ram.h"

#ifndef FIRST_INST_ADDRESS
#define FIRST_INST_ADDRESS 0x80000000
#endif

static const char *reg_name[DIFFTEST_NR_REG+1] = {
  "$0",  "ra",  "sp",   "gp",   "tp",  "t0",  "t1",   "t2",
  "s0",  "s1",  "a0",   "a1",   "a2",  "a3",  "a4",   "a5",
  "a6",  "a7",  "s2",   "s3",   "s4",  "s5",  "s6",   "s7",
  "s8",  "s9",  "s10",  "s11",  "t3",  "t4",  "t5",   "t6",
  "this_pc",
  "mstatus", "mcause", "mepc",
  "sstatus", "scause", "sepc",
  "satp",
  "mip", "mie", "mscratch", "sscratch", "mideleg", "medeleg",
  "mtval", "stval", "mtvec", "stvec", "mode",
};

Difftest **difftest = NULL;

int difftest_init() {
  difftest = new Difftest*[NUM_CORES];
  difftest[0] = new Difftest(i);
  return 0;
}

int init_nemuproxy() {
  difftest[0]->update_nemuproxy(0);
  return 0;
}

int difftest_state() {

  if (difftest[0]->get_trap_valid()) 
    return difftest[0]->get_trap_code();
    
  return -1;
}

int difftest_step() {
    int ret = difftest[0]->step();
    if (ret) {
      return ret;
    }
  return 0;
}

Difftest::Difftest(int coreid) : id(coreid) {
  state = new DiffState();
  clear_step();
}

void Difftest::update_nemuproxy(int coreid) {
  proxy = new DIFF_PROXY(coreid);
}

int Difftest::step() {
  progress = false;
  ticks++;

  // TODO: update nemu/xs to fix this_pc comparison
  dut.csr.this_pc = dut.commit[0].pc;

  if (check_timeout()) {
    return 1;
  }

  if (!has_commit) {
    return 0;
  }

  num_commit = 0; // reset num_commit this cycle to 0
  // interrupt has the highest priority
  if (dut.event.interrupt) {
    dut.csr.this_pc = dut.event.exceptionPC;
    do_interrupt();
  } else if(dut.event.exception) {
    // We ignored instrAddrMisaligned exception (0) for better debug interface
    // XiangShan should always support RVC, so instrAddrMisaligned will never happen
    // TODO: update NEMU, for now, NEMU will update pc when exception happen
    dut.csr.this_pc = dut.event.exceptionPC;
    do_exception();
  } else {
    // TODO: is this else necessary?
    for (int i = 0; i < DIFFTEST_COMMIT_WIDTH && dut.commit[i].valid; i++) {
      do_instr_commit(i);
      dut.commit[i].valid = 0;

      num_commit++;
      // TODO: let do_instr_commit return number of instructions in this uop
      if (dut.commit[i].fused) {
        num_commit++;
      }
    }
  }

  if (!progress) {
    return 0;
  }

  proxy->regcpy(ref_regs_ptr, REF_TO_DUT);

  if (num_commit > 0) {
    state->record_group(dut.commit[0].pc, num_commit);
  }

  // swap nemu_pc and ref.csr.this_pc for comparison
  uint64_t nemu_next_pc = ref.csr.this_pc;
  ref.csr.this_pc = nemu_this_pc;
  nemu_this_pc = nemu_next_pc;
  if (memcmp(dut_regs_ptr, ref_regs_ptr, DIFFTEST_NR_REG * sizeof(uint64_t))) {
    display();
    for (int i = 0; i < DIFFTEST_NR_REG; i ++) {
      if (dut_regs_ptr[i] != ref_regs_ptr[i]) {
        printf("%7s different at pc = 0x%010lx, right= 0x%016lx, wrong = 0x%016lx\n",
            reg_name[i], ref.csr.this_pc, ref_regs_ptr[i], dut_regs_ptr[i]);
      }
    }
    return 1;
  }

  return 0;
}

void Difftest::do_interrupt() {
  state->record_abnormal_inst(dut.commit[0].pc, dut.commit[0].inst, RET_INT, dut.event.interrupt);
  proxy->raise_intr(dut.event.interrupt | (1ULL << 63));
  progress = true;
}

void Difftest::do_exception() {
  state->record_abnormal_inst(dut.event.exceptionPC, dut.commit[0].inst, RET_EXC, dut.event.exception);
  if (dut.event.exception == 12 || dut.event.exception == 13 || dut.event.exception == 15) {
    // printf("exception cause: %d\n", dut.event.exception);
    struct ExecutionGuide guide;
    guide.exceptionNo = dut.event.exception;
    guide.mtval = dut.csr.mtval;
    guide.stval = dut.csr.stval;
    proxy->guided_exec(&guide);
  } else {
    proxy->exec(1);
  }
  progress = true;
}

void Difftest::do_instr_commit(int i) {
  progress = true;
  last_commit = ticks;

  // store the writeback info to debug array
  state->record_inst(dut.commit[i].pc, dut.commit[i].inst, dut.commit[i].wen, dut.commit[i].wdest, dut.commit[i].wdata, dut.commit[i].skip != 0);



  // MMIO accessing should not be a branch or jump, just +2/+4 to get the next pc
  // to skip the checking of an instruction, just copy the reg state to reference design
  if (dut.commit[i].skip) {
    proxy->regcpy(ref_regs_ptr, REF_TO_DIFFTEST);
    ref.csr.this_pc += dut.commit[i].isRVC ? 2 : 4;
    if (dut.commit[i].wen && dut.commit[i].wdest != 0) {
      // TODO: FPR
      ref_regs_ptr[dut.commit[i].wdest] = dut.commit[i].wdata;
    }
    proxy->regcpy(ref_regs_ptr, DIFFTEST_TO_REF);
    return;
  }

  // single step exec
  proxy->exec(1);
  // when there's a fused instruction, let proxy execute one more instruction.
  if (dut.commit[i].fused) {
    proxy->exec(1);
  }

}

void Difftest::do_first_instr_commit() {
  if (!has_commit && dut.commit[0].valid && dut.commit[0].pc == FIRST_INST_ADDRESS) {
    printf("The first instruction of core %d has commited. Difftest enabled. \n", id);
    has_commit = 1;
    nemu_this_pc = dut.csr.this_pc;

    proxy->memcpy(0x80000000, get_img_start(), get_img_size(), DIFFTEST_TO_REF);
    proxy->regcpy(dut_regs_ptr, DIFFTEST_TO_REF);
  }
}

// int Difftest::do_store_check() {
//   for (int i = 0; i < DIFFTEST_STORE_WIDTH; i++) {
//     if (!dut.store[i].valid) {
//       return 0;
//     }
//     auto addr = dut.store[i].addr;
//     auto data = dut.store[i].data;
//     auto mask = dut.store[i].mask;
//     if (proxy->store_commit(&addr, &data, &mask)) {
//       display();
//       printf("Mismatch for store commits %d: \n", i);
//       printf("  REF commits addr 0x%lx, data 0x%lx, mask 0x%x\n", addr, data, mask);
//       printf("  DUT commits addr 0x%lx, data 0x%lx, mask 0x%x\n",
//         dut.store[i].addr, dut.store[i].data, dut.store[i].mask);
//       return 1;
//     }
//     dut.store[i].valid = 0;
//   }
//   return 0;
// }





int Difftest::check_timeout() {
  // check whether there're any commits since the simulation starts
  if (!has_commit && ticks > last_commit + firstCommit_limit) {
    eprintf("No instruction commits for %lu cycles of core %d. Please check the first instruction.\n",
      firstCommit_limit, id);
    eprintf("Note: The first instruction may lie in 0x10000000 which may executes and commits after 500 cycles.\n");
    eprintf("   Or the first instruction may lie in 0x80000000 which may executes and commits after 2000 cycles.\n");
    display();
    return 1;
  }

  // check whether there're any commits in the last 5000 cycles
  if (has_commit && ticks > last_commit + stuck_limit) {
    eprintf("No instruction of core %d commits for %lu cycles, maybe get stuck\n"
        "(please also check whether a fence.i instruction requires more than %lu cycles to flush the icache)\n",
        id, stuck_limit, stuck_limit);
    eprintf("Let REF run one more instruction.\n");
    proxy->exec(1);
    display();
    return 1;
  }

  return 0;
}

void Difftest::raise_trap(int trapCode) {
  dut.trap.valid = 1;
  dut.trap.code = trapCode;
}

void Difftest::clear_step() {
  dut.trap.valid = 0;
  for (int i = 0; i < DIFFTEST_COMMIT_WIDTH; i++) {
    dut.commit[i].valid = 0;
  }
  dut.sbuffer.resp = 0;
  for (int i = 0; i < DIFFTEST_STORE_WIDTH; i++) {
    dut.store[i].valid = 0;
  }
  for (int i = 0; i < DIFFTEST_COMMIT_WIDTH; i++) {
    dut.load[i].valid = 0;
  }
  dut.atomic.resp = 0;
  dut.ptw.resp = 0;
}

void Difftest::display() {
  state->display(this->id);

  printf("\n==============  REF Regs  ==============\n");
  fflush(stdout);
  proxy->isa_reg_display();
  printf("priviledgeMode: %lu\n", dut.csr.priviledgeMode);
}


