tcl_tk=$(TRUST_ROOT)/exec/tcl_tk/bin/wish
old_version=8.4.14
version=8.5.13
all:$(tcl_tk)
$(tcl_tk):  install_tk
	make clean
	@./install_tk $(TRUST_ROOT) $(version)
	
clean:
	rm -r -f $(TRUST_ROOT)/exec/tcl_tk tcl$(old_version) tk$(old_version) tcl$(version) tk$(version)
