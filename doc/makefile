HTML=$(TRUST_ROOT)/doc/html
TAGS=$(TRUST_ROOT)/tags
ETAGS=$(TRUST_ROOT)/TAGS
TRUST_FILES=tmp_files

all:depend subdir

subdir:
	(if [ -d TRUST ] && [ "`ls TRUST/*akefile 2>/dev/null`" != "" ]; then cd TRUST; $(MAKE); fi)
	(if [ -d Coverage ] && [ "`ls Coverage/*akefile 2>/dev/null`" != "" ]; then cd Coverage; $(MAKE); fi)
doc:
	@echo "HTML documentation under $(HTML) will be updated in few minutes..."
	@make_Doxyfile 

ctags:
	@if [ ! -f $(TRUST_FILES) ]; then make depend;fi
	@if [ "`ctags --version 2>&1 | grep Exuberant`" != "" ]; then ctags -L $(TRUST_FILES) -f $(TAGS) \
	&& ctags -e -L $(TRUST_FILES) -f $(ETAGS) \
	&& echo "Files $(TAGS) and $(ETAGS) updated.";\
	fi

depend:
	@rm -f tmp $(TRUST_FILES)
	@echo 'html: \' >> tmp
	@for rep in `cat $(TRUST_ROOT)/env/rep.TRUST | sed s/^\\\.\\\///g`;\
	do for file in `ls $(TRUST_ROOT)/$$rep/*.cpp $(TRUST_ROOT)/$$rep/*.h`;\
	do echo $$file' \' >> tmp;\
	echo $$file >> $(TRUST_FILES);\
	done;\
	done;
	@echo "$(TRUST_ROOT)/bin/KSH/make_Doxyfile makefile" >> tmp
	@#echo "	@make ctags" >> tmp
	@echo "	@make doc" >> tmp
	@make -f tmp
	@rm -f tmp $(TRUST_FILES)

clean:
	@echo "Cleaning..."
	@rm -r -f $(HTML) *.tag doxygen0.out doxygen0.err
	(if [ -d TRUST ] && [ "`ls TRUST/*akefile 2>/dev/null`" != "" ]; then cd TRUST; $(MAKE) clean; fi)
	(if [ -d Coverage ] && [ "`ls Coverage/*akefile 2>/dev/null`" != "" ]; then cd Coverage; $(MAKE) clean; fi)
