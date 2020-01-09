
# -*- coding: latin-1 -*-



from Write_tex import Write_tex

from lib import chaine2Tex,chaine2File

from lib import GestionMessages

#import sys, os, time


class User_Write_tex(Write_tex):
    def __init__(self):
        Write_tex.__init__(self)
        self.nb_visu=0
        pass
    # inclusion de la visu dans le fichier latex
    def inclureVisuTex_oll(self, visu,fichier):
        '''Inclusion de la visu dans le fichier latex du rapport de validation.'''
        fichier.write_Tex('\n% Debut visu')

        if visu.titre!='Undefined':
            fichier.write_Tex('\subsection{%s}' % chaine2Tex(visu.titre))
            pass

        fichier.write_Tex('\begin{minipage}{%s}{'%visu.width)
        fichier.write_description(visu.description)
        fichier.write_text('')
        for i in range(max(len(visu.cycles.split()),1)):
            if (i>0):
                fichier.write_Tex('\begin{minipage}{%s}{'%visu.width)
                pass
            fichiercomplet=visu.fichierGraphiqueComplet+'_'+str(i)+'.'+visu.format
            fichiergraphique=visu.fichierGraphique+'_'+str(i)+'.'+visu.format

            self.verifie_existance_fichier(fichiercomplet,visu)
            width="".join(visu.width.split())
            if (width!="0cm") and (width[0]!="-"):
                self.nb_visu+=1
                if visu.format=='ps' or visu.format=='png':
                    fichier.write_Tex('\includegraphics[width=%s]{\orig/.tmp/%s}' % (visu.width,fichiergraphique))
                else:
                    fichier.write_Tex('\input{%s}' % fichiercomplet)
                    pass
                pass
            else:
                print("La visu n'inclue pas vraiment l'image ",fichiergraphique," car width <= 0 :",visu.width)
                pass
            fichier.write_Tex('} \end{minipage}')

            if (self.nb_visu==visu.nb_img_without_newline):
                fichier.write_Tex('\n\n')
                self.nb_visu=0
            pass

        pass
    pass
