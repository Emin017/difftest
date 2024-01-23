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

## software version
1. verilator 5.020
2. DRAMSim3  master
3. chisel 6.0.0
4. scala 2.13.12
5. mill 0.11.6

refer [xs-difftest](https://github.com/OpenXiangShan/difftest)
