lib=../../../lib/src/LIBVC/lib/libVc.a
Vc=Vc-1.4.2
all:$(lib)
$(lib):  makefile Install
	make clean;
	./Install $(Vc);

clean:
	rm -r -f $(Vc) ../../../lib/src/LIBVC
