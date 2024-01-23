#***************************************************************************************
# Copyright (c) 2020-2023 Institute of Computing Technology, Chinese Academy of Sciences
# Copyright (c) 2020-2021 Peng Cheng Laboratory
#
# DiffTest is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#
# See the Mulan PSL v2 for more details.
#***************************************************************************************

ifndef NANSHAN_HOME
$(error NANSHAN_HOME is not set)
endif

SIM_TOP    ?= SimTop


DESIGN_DIR ?= $(NANSHAN_HOME)
BUILD_DIR  = $(DESIGN_DIR)/build
RTL_DIR = $(BUILD_DIR)/rtl
RTL_SUFFIX ?= v
SIM_TOP_V = $(RTL_DIR)/$(SIM_TOP).$(RTL_SUFFIX)

# generate difftest files for non-chisel design.
difftest_verilog:
	mill -i difftest.test.runMain difftest.DifftestMain -td $(RTL_DIR)

# co-simulation with DRAMsim3
ifeq ($(WITH_DRAMSIM3),1)
ifndef DRAMSIM3_HOME
$(error DRAMSIM3_HOME is not set)
endif
override SIM_ARGS += --with-dramsim3
endif

TIMELOG = $(BUILD_DIR)/time.log
TIME_CMD = time -a -o $(TIMELOG)

# simulation
SIM_CONFIG_DIR = $(abspath ./config)
SIM_CSRC_DIR = $(abspath ./src/test/csrc/common)
SIM_CXXFILES = $(shell find $(SIM_CSRC_DIR) -name "*.cpp")
SIM_CXXFLAGS = -I$(SIM_CSRC_DIR) -I$(SIM_CONFIG_DIR)
SIM_CXXFLAGS += -DNANSHAN_HOME=\\\"$(NANSHAN_HOME)\\\"


# generated-src
GEN_CSRC_DIR  = $(BUILD_DIR)/generated-src
SIM_CXXFILES += $(shell find $(GEN_CSRC_DIR) -name "*.cpp")
SIM_CXXFLAGS += -I$(GEN_CSRC_DIR)

GEN_VSRC_DIR = $(BUILD_DIR)/generated-src
VSRC_DIR   = $(abspath ./src/test/vsrc/common)
SIM_VSRC = $(shell find $(VSRC_DIR) -name "*.v" -or -name "*.sv")

# DiffTest support
DIFFTEST_CSRC_DIR = $(abspath ./src/test/csrc/difftest)
DIFFTEST_CXXFILES = $(shell find $(DIFFTEST_CSRC_DIR) -name "*.cpp")
ifeq ($(NO_DIFF), 1)
SIM_CXXFLAGS += -DCONFIG_NO_DIFFTEST
else
SIM_CXXFILES += $(DIFFTEST_CXXFILES)
SIM_CXXFLAGS += -I$(DIFFTEST_CSRC_DIR)
endif


ifeq ($(WITH_IPC), 1)
SIM_CXXFLAGS += -I$(BUILD_DIR) -DENABLE_IPC
endif

# REF SELECTION
ifneq ($(REF),)
ifneq ($(wildcard $(REF)),)
SIM_CXXFLAGS += -DREF_PROXY=LinkedProxy -DLINKED_REFPROXY_LIB=\\\"$(REF)\\\"
SIM_LDFLAGS  += $(REF)
else
SIM_CXXFLAGS += -DREF_PROXY=$(REF)Proxy
REF_HOME_VAR = $(shell echo $(REF)_HOME | tr a-z A-Z)
ifneq ($(origin $(REF_HOME_VAR)), undefined)
SIM_CXXFLAGS += -DREF_HOME=\\\"$(shell echo $$$(REF_HOME_VAR))\\\"
endif
endif
endif

# co-simulation with DRAMsim3
ifeq ($(WITH_DRAMSIM3),1)
SIM_CXXFLAGS += -I$(DRAMSIM3_HOME)/src
SIM_CXXFLAGS += -DWITH_DRAMSIM3 -DDRAMSIM3_CONFIG=\\\"$(DRAMSIM3_HOME)/configs/XiangShan.ini\\\" -DDRAMSIM3_OUTDIR=\\\"$(BUILD_DIR)\\\"
SIM_LDFLAGS  += $(DRAMSIM3_HOME)/build/libdramsim3.a
endif


#TODO: (maybe useful) FIRRTL Coverage support
ifneq ($(FIRRTL_COVER),)
SIM_CXXFLAGS += -DFIRRTL_COVER
endif

#TODO: (maybe useful) LLVM Sanitizer Coverage support
ifneq ($(LLVM_COVER),)
SIM_CXXFLAGS += -DLLVM_COVER
SIM_LDFLAGS  += -fsanitize-coverage=trace-pc-guard -fsanitize-coverage=pc-table
endif

include verilator.mk

clean: 
	rm -rf $(BUILD_DIR)
	
.PHONY: sim-verilog emu difftest_verilog clean