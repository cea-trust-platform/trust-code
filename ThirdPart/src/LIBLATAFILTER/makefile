latafilter=$(TRUST_LATAFILTER)/lib/liblatafilter.a

all: $(latafilter)

$(latafilter): ./build.sh makefile trust_commun/LataFilter.h
	make clean
	./build.sh

clean:
	rm -rf lib include build  $(TRUST_LATAFILTER)
