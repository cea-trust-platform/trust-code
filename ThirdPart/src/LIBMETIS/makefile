lib=../../../lib/src/LIBMETIS/lib/libmetis.a
metis=metis-5.1.0-p5
tar=$(metis).tar.gz
all:$(lib)
$(lib):  makefile Install
	make clean;
	./Install $(metis);
	
clean:
	rm -r -f $(metis) ../../../lib/src/LIBMETIS/bin ../../../lib/src/LIBMETIS/include ../../../lib/src/LIBMETIS/lib
