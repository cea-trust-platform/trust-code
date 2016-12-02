# -*- coding: utf-8 -*-
"""
Documentation
Python script to replace bookmarks in Generic_Guide.tex by their page numbers.
Use '\href[page=DOCLINK_nameofbookmark]{PDFFILE.pdf}{nameofref}' in tex file.
"""

import os, sys
import subprocess
import fileinput
from optparse import OptionParser

class Bookmarks(object):
    def __init__(self, pdf):
        p = subprocess.Popen('pdftk %s dump_data'%pdf, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()

        if p.returncode !=0 :
            printf("Erreur lors de l'execution de la commande pdftk")
            printf("stderr : ")
            for line in p.stderr:
                print line,
            sys.exit(1)
        
        self.bookmarks = dict()
        bookmarks = ''.join([l for l in p.stdout if 'Bookmark' in l])

        for bm in bookmarks.split('BookmarkTitle: '):
            if bm:
                liste = [l.strip() for l in bm.split('\n') if l]
                if len(liste)>=2:
                    titre = liste[0]
                    # on retire la numérotation
                    titre = ' '.join(titre.split()[1:])
                    # on retire les caractères bizarre (sur is221714)
                    titre = titre.replace('&#0;', '')
                    try:
                        self.bookmarks[titre] = int(liste[2].replace('BookmarkPageNumber: ', ''))
                    except:
                        # il y a des cas bizarres, on laisse tomber
                        pass

    def getPage(self, key):
        liste = sorted([bm for bm in self.bookmarks if key in bm], key=len)
        if not liste:
            print "%s not found"%key
            return 0
        return self.bookmarks[liste[0]]


def options_script(argv):

    parser = OptionParser(usage="usage: %prog [options]")

    parser.add_option("-p", "--pdf", dest="pdf", type="string",
                      metavar="<pdf>", help="input PDF file")

    parser.add_option("-t", "--tex", dest="tex", type="string",
                      metavar="<tex>", help="TEX file to be modified")

    (options, args) = parser.parse_args(argv)

    if (len(args) > 0) or not options.pdf or not options.tex :
        parser.print_help()
        sys.exit(1)

    return options.pdf, options.tex
        
def main(argv):
    """
    Main function.
    """

    pdf, tex = options_script(argv)
    mybooks = Bookmarks(pdf)

    linkkey = 'DOCLINK_'
#    f = open(tex, 'r')
#    for line in fileinput.input(tex, inplace=True, backup='.original'):
    for line in fileinput.input(tex, inplace=True):
        if linkkey in line:
            newline = line
            while linkkey in newline:
                keyword = newline.split(linkkey)[1].split(']')[0]
                newline = newline.replace(linkkey+keyword, str(mybooks.getPage(keyword)))
            print newline,
        else:
            print line,

#    for value in ['MESH', 'MESH FILES', 'PROBE']:
#        print "Le chapitre %s se trouve page %d !"%(value, mybooks.getPage(value))
    
    return

if __name__=="__main__":
    main(sys.argv[1:])
