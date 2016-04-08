name=python
install=$(TRUST_ROOT)/exec/$(name)_modules
all: matplotlib PySide


matplotlib: ./install_addon.sh makefile
	@./install_addon.sh $(package) $(install)
PySide:  ./install_PySide.sh matplotlib
	@./install_PySide.sh $(package) $(install)
clean:
	rm -r -f $(install)
