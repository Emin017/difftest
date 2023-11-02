# NEMU
REF := NEMU
REF_SO := $(NEMU_HOME)/build/riscv64-nemu-interpreter-so
$(REF_SO):
	$(MAKE) -C $(NEMU_HOME) riscv64-xs-ref_defconfig
	$(MAKE) -C $(NEMU_HOME)

# DRAMsim3
$(DRAMSIM3_LIB):
	cd $(DRAMSIM3_HOME) && mkdir -p build && cd build && cmake -D COSIM=1 ..
	$(MAKE) -C $(DRAMSIM3_HOME)/build -j4

emu-env: $(if WITH_DRAMSIM3, $(DRAMSIM3_LIB)) $(if NO_DIFF,,$(REF_SO))

.PHONY: emu-env