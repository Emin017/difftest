# Simplified Difftest Submodule

## Features
1. **Support ArchEvent**
   - `intrNo`
   - `cause`
   - `excpPC`
   - `excpInst`

2. **Support TrapEvent**
   - `valid`
   - `code`
   - `pc`
   - `cycleCnt`
   - `instrCnt`

3. **Support CSRState**
   - `privilegeMode`
   - `mstatus`
   - `sstatus`
   - `mepc`
   - `sepc`
   - `mtval`
   - `stval`
   - `mtvec`
   - `stvec`
   - `mcause`
   - `scause`
   - `satp`
   - `mip`
   - `mie`
   - `mscratch`
   - `sscratch`
   - `mideleg`
   - `medeleg`

4. **Support ArchIntRegState**
   - `gpr1` to `gpr31`

5. **Support InstrCommit**
   - `index`
   - `valid`
   - `pc`
   - `instr`
   - `special`
   - `skip`
   - `isRVC`
   - `wen`
   - `wdest`
   - `wdata`

6. **Support AXI4 Difftest with DRAMSim3**
