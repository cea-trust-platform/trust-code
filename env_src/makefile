include $(TRUST_ENV)/make.$(TRUST_ARCH_CC)$(OPT)
include $(TRUST_ENV)/make.all_Includes.TRUST

# Multirepertoire donc share libs
LIB_EXTENSION=$(SL_EXTENSION)
include $(TRUST_ENV)/make.lib$(LIB_EXTENSION)

.SUFFIXES: .cpp .C .o
.cpp.o:
	@echo $< $(Defines); $(TRUST_CC) -c $< $(CppFlags)
.C.o:
	@echo $< $(Defines); $(TRUST_cc) -c $< $(ccFLAGS)
.c.o:
	@echo $< $(Defines); $(TRUST_cc) -c $< $(ccFLAGS)
.f.o:
	@echo $< $(Defines); $(TRUST_F77) -c $< $(F77Flags)

PROGRAM=$(TRUST_ROOT)/exec/TRUST$(COMM)$(OPT)

tout : all

clean:
	@rm -f *.o 
	@rm -f $(Lib).${SL_EXTENSION}
	@rm -f .check_sources.ok	
depend:
	@make_depend_TRUST $(Includes) $(SYSINC) $(SYSINC_DEP) $(Defines)
depend_mini:
	@make_depend_TRUST_mini $(Includes) $(SYSINC) $(SYSINC_DEP) $(Defines)
all_depend : 
	@make_all_depend_TRUST $(all_Includes) $(SYSINC) $(SYSINC_DEP) $(Defines)
