/**
 * ************************************************************************************* Copyright (c) 2020-2021
 * Institute of Computing Technology, Chinese Academy of Sciences Copyright (c) 2020-2021 Peng Cheng Laboratory
 *
 * XiangShan is licensed under Mulan PSL v2. You can use this software according to the terms and conditions of the
 * Mulan PSL v2. You may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 */

package difftest

import chisel3._
import chisel3.util._
import Chisel.BlackBox

trait DifftestParameter {}

trait DifftestWithClock {
  val clock = Input(Clock())
}

abstract class DifftestBundle extends Bundle with DifftestParameter with DifftestWithClock

class DiffArchEventIO extends DifftestBundle {
  val intrNO        = Input(UInt(32.W))
  val cause         = Input(UInt(32.W))
  val exceptionPC   = Input(UInt(64.W))
  val exceptionInst = Input(UInt(32.W))
}

class DiffInstrCommitIO extends DifftestBundle with DifftestWithIndex {
  val valid    = Input(Bool())
  val pc       = Input(UInt(64.W))
  val instr    = Input(UInt(32.W))
  val special  = Input(UInt(8.W))
  val skip     = Input(Bool())
  val isRVC    = Input(Bool())
  val scFailed = Input(Bool())
  val wen      = Input(Bool())
  val wdata    = Input(UInt(64.W))
  val wdest    = Input(UInt(8.W))
}

class DiffTrapEventIO extends DifftestBundle {
  val valid    = Input(Bool())
  val code     = Input(UInt(3.W))
  val pc       = Input(UInt(64.W))
  val cycleCnt = Input(UInt(64.W))
  val instrCnt = Input(UInt(64.W))
}

class DiffCSRStateIO extends DifftestBundle {
  val priviledgeMode = Input(UInt(2.W))
  val mstatus        = Input(UInt(64.W))
  val sstatus        = Input(UInt(64.W))
  val mepc           = Input(UInt(64.W))
  val sepc           = Input(UInt(64.W))
  val mtval          = Input(UInt(64.W))
  val stval          = Input(UInt(64.W))
  val mtvec          = Input(UInt(64.W))
  val stvec          = Input(UInt(64.W))
  val mcause         = Input(UInt(64.W))
  val scause         = Input(UInt(64.W))
  val satp           = Input(UInt(64.W))
  val mip            = Input(UInt(64.W))
  val mie            = Input(UInt(64.W))
  val mscratch       = Input(UInt(64.W))
  val sscratch       = Input(UInt(64.W))
  val mideleg        = Input(UInt(64.W))
  val medeleg        = Input(UInt(64.W))
}

class DiffArchIntRegStateIO extends DifftestBundle {
  val gpr = Input(Vec(32, UInt(64.W)))
}

class DifftestArchEvent extends BlackBox {
  val io = IO(new DiffArchEventIO)
}

class DifftestInstrCommit extends BlackBox {
  val io = IO(new DiffInstrCommitIO)
}

class DifftestTrapEvent extends BlackBox {
  val io = IO(new DiffTrapEventIO)
}

class DifftestCSRState extends BlackBox {
  val io = IO(new DiffCSRStateIO)
}

class DifftestArchIntRegState extends BlackBox {
  val io = IO(new DiffArchIntRegStateIO)
}

// Difftest emulator top

package object difftest {}
