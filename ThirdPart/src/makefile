include $(TRUST_ENV)/makefile
SUBDIRS= LIBMETIS LIBMFFT LIBMFFT_C LIBMPI LIBSILO LIBSKIT LIBNP LIBMED 
all:
	 @for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) ) ; done

