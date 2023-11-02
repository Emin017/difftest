REF_SO := $(NEMU_HOME)/build/riscv64-nemu-interpreter-so
$(REF_SO):
	$(MAKE) -C $(NEMU_HOME) riscv64-xs-ref_defconfig
	$(MAKE) -C $(NEMU_HOME)
