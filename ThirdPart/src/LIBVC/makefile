OPT=_opt
include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
include $(TRUST_ENV)/make.lib$(LIB_EXTENSION)
include $(TRUST_ENV)/make.all_Includes.TRUST

lib=../../../lib/src/LIBVC/lib/libVc.a
Vc=Vc-1.4.2
all:$(lib)
$(lib): makefile Install 
	make clean;
	./Install $(Vc);
	make test;

test: test_VC.cpp
	cp test_VC.cpp $(TRUST_TMP)/build/lib/Vc_build; cd $(TRUST_TMP)/build/lib/Vc_build; \
	$(TRUST_CC) -o $@ $< $(CppFLAGS) $(Libs) $(all_Includes) || exit -1; pwd; \
	./test && echo "VC Library OK" && rm -rf $(TRUST_TMP)/build/lib/Vc_build && exit 0 || (echo "VC Library KO"; exit -1)


clean:
	rm -r -f $(Vc) ../../../lib/src/LIBVC
