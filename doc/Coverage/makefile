FILES=list_methodes_non_appelees list_methodes dico.pkl
HTML=html

all:$(FILES) $(HTML)

$(FILES): list_methodes_non_appelees.gz list_methodes.gz dico.pkl.gz
	gunzip -c list_methodes_non_appelees.gz > list_methodes_non_appelees
	gunzip -c list_methodes.gz > list_methodes
	gunzip -c dico.pkl.gz > dico.pkl

$(HTML): html.tgz makefile cpp.gcov.tar.gz
	rm -r -f html;mkdir -p html;cd html;tar xfz ../html.tgz;rm -r -f cpp.gcov;mkdir -p cpp.gcov;cd cpp.gcov;tar zxf ../../cpp.gcov.tar.gz

clean:
	rm -r -f $(FILES) $(HTML)

