# Simplified difftest Submodule

## Feature
1. support ArchEvent 
```
intrNo 
cause 
excpPC 
excpInst
```
2. support TrapEvent
```
valid
code
pc
cycleCnt
instrCnt
```
3. support CSRState
```
priviledgeMode,
mstatus,       
sstatus,       
mepc,          
sepc,          
mtval,         
stval,         
mtvec,         
stvec,         
mcause,        
scause,        
satp,          
mip,           
mie,           
mscratch,      
sscratch,      
mideleg,       
medeleg        
```
4. support ArchIntRegState
```
gpr1 -> gpr31
```
5. support InstrCommit
```
index,   
valid,   
pc,      
instr,   
special, 
skip,    
isRVC,   
wen,     
wdest,   
wdata    
```
6. support AXI4 difftest with DRAMSim3




