triou.py: objet_u.py objet_u_spec.py gencl.py gentriopy.py syno.py
	python gentriopy.py
	rm dico.py triouint.py triouint2.py
	sort -u trace > traces
	rm -f trace sed2 sedfile sedfile2 existepas.stop gentriopy.py2 gencles.py run.log
syno.py : $(exec) cree_syno
	@./cree_syno $(exec)
TRAD_2 : TRAD_2.org $(exec)
	@./build_TRAD2.sh
gencl.py: syno.py GENCLASS2 TRAD_2 sort_gen_cl.py
	./GENCLASS2
triou_old.py: objet_u.py objet_u_spec.py genclold.py gentriopy.py
	python gentriopy.py 1
doc: doc.pdf
doc.pdf: triou.py 
	mkdir -p doc
	(cd doc; ln -sf ../doc_ressources/* .)
	python -c "from triou import *;gen_doc_base(1)"
	mv *.tex doc
	cp *.sty doc/.
	@grep -v batchmode  doc/doc.tex  > doc/doc_inter.tex
	(cd doc ;touch doc.ind;pdflatex -interaction=nonstopmode doc.tex 1>/dev/null ;makeindex doc;pdflatex -interaction=nonstopmode  doc.tex 1>/dev/null;makeindex doc;pdflatex -interaction=nonstopmode doc.tex && mv doc.pdf ..)
	./create_Keywords.sh
doc_reduit: doc_r.pdf
doc_r.pdf: triou.py tests/ref/lutil.py
	mkdir -p doc_r
	(cd doc_r; ln -sf ../doc_ressources/* .)
	cp tests/ref/lutil.py .
	python -c "from triou import *;gen_doc_base(3)"
	mv *.tex doc_r
	cp *.sty doc_r/.
	(cd doc_r ;touch doc.ind;pdflatex -interaction=nonstopmode doc.tex 1>/dev/null ;makeindex doc;pdflatex -interaction=nonstopmode  doc.tex 1>/dev/null;pdflatex -interaction=nonstopmode doc.tex && mv doc.pdf ../doc_r.pdf)
test:	triou.py
	(mkdir -p tests; cat tests_src/makefile > tests/makefile; cd tests; time $(MAKE))
	touch test
test_ref: test
	(cd tests; $(MAKE) test_ref);
test_ref_trace: test
	(cd tests; $(MAKE) test_ref_trace);
happydoc: objet_u.py objet_u_spec.py gentriopy.py 
	happydoc -d doc_python objet_u.py objet_u_spec.py gentriopy.py 
update_xd: test
	python testgui.py
	cp menu/menu_*.py triou.py test_*.py ../TRIOU/TRIOU_SRC/src/.
	chmod +w ../TRIOU/TRIOU_SRC/src/test_*.py
	#(cd ../TRIOU/build; make install)
	touch update_xd
	(cd ..; make)
clean:
	rm -f existepas.stop test doc.pdf doc_r.pdf triou.py *.pyc  syno.py gencl.py gencles.py gentriopy.py2 *.ver sed2 sedfile sedfile2 trace traces update_xd
	rm -f Keywords.txt TRAD_2
	rm -rf doc_r doc menu
	(rm -rf tests) || exit 0 #(cd tests &&  make clean) || exit 0
