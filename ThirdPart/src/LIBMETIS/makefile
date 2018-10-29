lib=$(METIS_ROOT)/lib/libmetis.a
metis=metis-5.1.0-p5
tar=$(metis).tar.gz
all:$(lib)
$(lib):  makefile Install
	make clean;
	./Install $(metis);
	
clean:
	rm -r -f $(metis) $(METIS_ROOT)/bin $(METIS_ROOT)/include $(METIS_ROOT)/lib
