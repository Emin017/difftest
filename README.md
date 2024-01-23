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


## 一些相关功能
1. chiseldb
```
目前在香山里的主要应用场景是缓存（l2/l3）debug，这个也会和 refill test 结合。如果 refill test 检测得到下级缓存重填的数据已经是错的，可以检测到地址；或者其他 difftest 出错，手动看到地址。这样通过这个地址索引 chiseldb，就可以看到这个地址在所有缓存中的数据和权限块变化，之后定位到产生 bug 的时间。例如在 100M cycles 的权限错误，可能 200M cycles 才会暴露出来。
其他场景绝大多数演化成了 perfdb，主要用于性能分析，例如访存流、预取有效率及时率命中率等等。
```