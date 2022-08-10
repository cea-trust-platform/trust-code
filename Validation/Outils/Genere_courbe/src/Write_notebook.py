#****************************************************************************
# Copyright (c) 2022, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

# -*- coding: utf-8 -*-

__version__ = '0.1'


from lib import chaine2Tex,chaine2File

from lib import GestionMessages,print_description
from lib import get_nom_cas

import sys, os, time, re
import nbformat as nbf

from Visu import dico_plots

class Write_notebook:
    def __init__(self):
        self.nb_visu=0
        self.minifigure_avt=0
        self.nb = nbf.v4.new_notebook()
        pass
    def write_text_sans_nl(self,titi):
        return chaine2Tex(titi)
    def write_text(self,titi):
        text = self.write_text_sans_nl(titi)
        text += '\n'
        return text
    def write_description_sans_nl(self,description):
        if (len(description)==0): return
        text = self.write_text_sans_nl(self.Textojupyter(description[0])) + "\n"
        for i in range(1,len(description)):
            text += self.write_text_sans_nl(self.Textojupyter(description[i]))
            text += '\n '
            pass
        text += '\n'
        return text
    def write_description(self,description):
        text = self.write_description_sans_nl(description)
        return text 
    def replace_fonction(self,fonction):
        text = fonction
        text= text.replace("cos","np.cos")
        text= text.replace("sin","np.sin")
        text= text.replace("exp","np.exp")
        return text
    def Textojupyter(self,text):
        text= text.replace("$\\bullet$","* ")
        text= text.replace("- ","\t - ")
        return text

    def verifie_existance_fichier(self,nom_fichier,objet):
        iter = 0
        while iter<2 and (not os.path.isfile(nom_fichier)):
            objet.gestMsg.ecrire(GestionMessages._DEBOG, 'Search file %s from %s' % (nom_fichier, os.getcwd()))
            time.sleep(1)
            iter += 1
            # print 'waiting %d' % iter
            pass
        if not os.path.isfile(nom_fichier):
            objet.gestMsg.ecrire(GestionMessages._ERR, 'Error, graphic file %s is not generated ' % (nom_fichier))
            pass
        pass
    # inclusion de la figure dans le fichier latex
    def inclureFigureNotebook(self,maitre,figure):
        '''Inclusion de la figure dans le fichier latex du rapport de validation.'''
        minifigure=self.minifigure_avt
        if (figure.nb_img_without_newline>0):
            minifigure=1
            pass
        title = ""
        if (figure.titre!="Undefined"):
            title = "### " + chaine2Tex(figure.titre) + "\n \n "
    
        # TODO voir pour minifigure et markers avec style + options linewidth...

        if len(figure.description)!=0: title += self.write_description(figure.description)
        
        #figure.printFichierParametres()
        if len(figure.listeCourbes)>0:  
            code = "from trustutils import plot \n \n"
            code += "fig = plot.Graph(\"" + chaine2Tex(figure.titre) + "\") \n"
            for courbe in figure.listeCourbes:
                style = ""
                if courbe.style!="Undefined":
                    if courbe.style=="points":
                        style = ",marker='+'"
                    if courbe.style =="linepoints":
                        style = ",marker='-x'"
                if (courbe.points!='Undefined'):
                    param = courbe.segment.split()
                    ficSon = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.son'
                    label = courbe.legende.replace('"','')
                    code += "fig.addPoint(\"" + ficSon + "\",label=\"" + label + "\""+ style +") \n" 
                elif (courbe.segment!='Undefined'):
                    param = courbe.segment.split()
                    ficSon = get_nom_cas(param[0]) + '_' + (param[1]).upper() + '.son'
                    
                    label = courbe.legende.replace('"','')
                    code += "fig.addSegment(\"" + ficSon + "\",label=\"" + label + "\"" + style +") \n" 
                else:
                    if (courbe.fichier!="Undefined"):
                        code += "data = plot.loadText(\"%s\")\n"%(courbe.fichier)
                        if (courbe.colonnes):
                            indices= [int(s) for s in re.findall(r'\$(\d+)',courbe.colonnes)]
                           
                            formule = courbe.colonnes
                            for j in indices:
                                original_string="$%d"%j
                                replacement_string="data[%d]"%(j-1) 
                                formule=formule.replace(original_string, replacement_string)
                            
                            formule = self.replace_fonction(formule)
                            colonnes=formule.split()
                            code += "fig.add(%s,%s,label=%s"%(colonnes[0],colonnes[1],courbe.legende) + style + ")\n" #TODO Attention ne permet pas l'existence d'espace dans la formule
                    else:
                        code += "\nimport numpy as np \n"
                        code += "x = np.linspace(min(data[0]),max(data[0]),100) \n" #TODO pb avec les limites
                        fonction = self.replace_fonction(courbe.fonction)
                        code += "y = " + fonction + " \n"
                        code += "fig.add(x,y,label=r" + chaine2Tex(courbe.legende) + style + ") \n" 
            if figure.labelX!="Undefined" or figure.labelY!="Undefined":
                code += "\nfig.label(%s,%s)\n"%(figure.labelX,figure.labelY)
            if figure.rangeX!="auto" or figure.rangeY!="auto":
                if figure.rangeX=="auto":
                    rangeY = figure.rangeY.split()
                    code += "\nfig.visu(ymin=%s,ymax=%s)\n"%(rangeY[0],rangeY[1])
                elif figure.rangeY=="auto":
                    rangeX = figure.rangeX.split()
                    code += "\nfig.visu(xmin=%s,xmax=%s)\n"%(rangeX[0],rangeX[1])
                else:
                    rangeX = figure.rangeX.split()
                    rangeY = figure.rangeY.split()
                    code += "\nfig.visu(%s,%s,%s,%s)\n"%(rangeX[0],rangeX[1],rangeY[0],rangeY[1])
            if figure.logX or figure.logY:
                if not figure.logX:
                    code += "\nfig.scale(yscale='log')\n"
                elif not figure.logY:
                    code += "\nfig.scale(xscale='log')\n"
                else:
                    code += "\nfig.scale('log','log')\n"
                pass

            self.nb['cells'] += [nbf.v4.new_markdown_cell(title)]
            self.nb['cells'] += [nbf.v4.new_code_cell(code)]
        else:
            #motcle picture/image
            path = os.path.join("src",figure.image)
            title += "![](%s)"%(path)
            self.nb['cells'] += [nbf.v4.new_markdown_cell(title)] 
        pass

    # inclusion de la visu dans le fichier latex
    def inclureVisuNotebook(self,maitre,visu):
        '''Inclusion de la visu dans le fichier latex du rapport de validation.'''
        code = "from trustutils import visit\n \n"
        title = "### " + chaine2Tex(visu.titre) + "\n \n "
        minifigure=self.minifigure_avt
        #TODO minifigure
        if len(visu.description)!=0: title += self.write_description(visu.description)
        #visu.printFichierParametres()
        show = 0
        code0 = ""
        code1 = ""
        for plot in visu.plot:
            if plot[0] in list(dico_plots.keys()): #TODO a voir pour blackvector
                if not show:
                    code0 += "visu = visit.Show(\"%s\",\"%s\",\"%s_%s_%s\",mesh=\"%s\")\n" %(plot[1],dico_plots[plot[0]],plot[3],plot[4],plot[2],plot[2])
                    show = 1
                else:
                    code1 += "visu.addField(\"%s\",\"%s\",\"%s_%s_%s\",mesh=\"%s\")\n" %(plot[1],dico_plots[plot[0]],plot[3],plot[4],plot[2],plot[2])

            if plot[0]=='mesh':
                if not show:
                    code0 += "visu = visit.Show(\"%s\",\"Mesh\",\"%s\")\n" %(plot[1],plot[2])
                    show = 1
                else:
                    code1 += "visu.addField(\"%s\",\"Mesh\",\"%s\")\n" %(plot[1],plot[2])
            
            if (plot[0].lower()=='normal3d'):
                code1 += "visu.normal3D([%s,%s,%s])\n"%(plot[1],plot[2],plot[3])
            if (plot[0].lower()=='up3d'):
                code1 += "visu.up3D([%s,%s,%s])\n"%(plot[1],plot[2],plot[3])
            if (plot[0].lower()=='zoom3d'):
                code1 += "visu.zoom3D([%s,%s,%s])\n"%(plot[1],plot[2],plot[3])
            if (plot[0].lower()=='zoom2d'):
                code1 += "visu.zoom3D([%s,%s,%s,%s])\n"%(plot[1],plot[2],plot[3],plot[4])
            if (plot[0].lower()=='zoom2d'):
                code1 += "visu.zoom2D([%s,%s,%s,%s])\n"%(plot[1],plot[2],plot[3],plot[4])
            if (plot[0].lower()=='operator') or (plot[0].lower()=='operator_to_all') : 
                if (plot[1].lower()=='slice') or (plot[1].lower()=='slice2d'):
                    code1 += "visu.slice(origin=[%s,%s,%s],normal=[%s,%s,%s],type_op=\"%s\")\n" %(plot[2],plot[3],plot[4],plot[5],plot[6],plot[7],plot[1].lower())
                else:
                    code1 += "visu.visuOptions([%s])\n"%plot[1]
            if (plot[0].lower()=='instruction_visit'):
                code1 += "visu.executeVisitCmds(%s)\n"%plot[1]
        code += code0 + code1 + "visu.plot()"
                
        self.nb['cells'] += [nbf.v4.new_markdown_cell(title),
                nbf.v4.new_code_cell(code)]
        pass
    def inclureTableauNotebook(self,maitre,tableau):
        '''Inclusion du tableau dans le fichier latex du rapport de validation.'''
        

        title = ""
        if (tableau.titre!="Undefined"):
            title = "### "+ chaine2Tex(tableau.titre) + "\n \n "
        if (len(tableau.description)!=0): title += self.write_description(tableau.description)


        if len(tableau.listeLignes)==0: return

        from Tableau import Tableau_performance, Tableau_performance_nvellevalid
        if (isinstance(tableau,Tableau_performance) or isinstance(tableau,Tableau_performance_nvellevalid)): 
            code = "run.tablePerf()"
            self.nb['cells'] += [nbf.v4.new_code_cell(code)]
            return
        
        self.nb['cells'] += [nbf.v4.new_markdown_cell(title)]

        nbc=tableau.nb_colonnes
        if (tableau.formule): nbc+=1
        #tableau.printFichierParametres()

        code = "from trustutils import plot \n \n"
        columns = chaine2Tex(tableau.label).split('|')
        code += "columns=%s \n" %(columns)
        code += "tab = plot.Table(columns)\n" 
        
        for ligne in tableau.listeLignes:
            if ligne.valeurs!='Undefined':
                valeur_f= ligne.valeurs.split()
                for i in range(len(valeur_f)):
                    try:
                        valeur_f[i]=eval(valeur_f[i])
                    except:
                        valeur_f[i]=chaine2Tex(valeur_f[i])
                        #pass
                code += "tab.addLigne([%s],\"%s\")\n"%(valeur_f,chaine2Tex(ligne.legende))
        
            elif ligne.fichier!='Undefined':
                code += "data = plot.loadText(\"%s\",transpose=False)\n"%(ligne.fichier)

            nb_colonnes_f=tableau.nb_colonnes
            if (ligne.nb_colonnes_fichier): nb_colonnes_f=ligne.nb_colonnes_fichier
            try:
                if (ligne.colonnes):
                    code += "tab.addLigne([["
                    colonnes=ligne.colonnes.split()

                    for i in range(tableau.nb_colonnes):
                        formule=colonnes[i]

                        for j in range(nb_colonnes_f):
                            original_string="$%d"%(j+1)
                            if tableau.nb_colonnes > 1:
                                replacement_string="data[-1][%d]"%(j) ##TODO parametre derniere ligne, qu'est ce qui est affiche si derniere_ligne=0 ?
                            else:
                                replacement_string="data[%d]"%(j) ##TODO parametre derniere ligne, qu'est ce qui est affiche si derniere_ligne=0 ?
                            formule=formule.replace(original_string, replacement_string)
                            pass
                        pass
                        code+=formule +","
                    pass
                    code += "]],\"%s\")\n"%(chaine2Tex(ligne.legende))
                pass
            except:
                ligne.gestMsg.ecrire(GestionMessages._ERR, 'unable to read %d values in file %s.'% (nb_colonnes_f,ligne.fichier))

        code += "display(tab)"
        self.nb['cells'] += [nbf.v4.new_code_cell(code)]
        pass
    def inclureObjet(self,maitre,figure):
        from Visu import Visu
        from Figure import Figure
        from Tableau import Tableau, Tableau_performance
        if (isinstance(figure,Visu)):
            self.inclureVisuNotebook(maitre,figure)
        elif (isinstance(figure,Figure)):
            self.inclureFigureNotebook(maitre,figure)
        elif (isinstance(figure,Tableau)):
            self.inclureTableauNotebook(maitre,figure)
        else:
            figure.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected figure type %s'%(figure))
            pass
        pass
#
    def inclureChapitre(self,maitre,chapitre):
        '''Inclusion du chapitre dans le notebook.'''
        title = "## " + chaine2Tex(chapitre.titre) + "\n \n"
        if len(chapitre.description)!=0: title += self.write_description(chapitre.description)
        iter = 0
        self.nb['cells'] += [nbf.v4.new_markdown_cell(title)]

        #ajout des figures
        for figure in chapitre.listeFigures:
            self.inclureObjet(maitre,figure)
            pass
        pass
#
    def inclureSSChapitreTex(self,maitre,sousChapitre):
        '''Inclusion des sous-chapitres dans le fichier latex du rapport de validation.'''
        from SousChapitre import SousChapitre
        fin = False

        if sousChapitre.titre!='Undefined':
            title = "### %s \n" % chaine2Tex(sousChapitre.titre)
            self.nb['cells'] += [nbf.v4.new_markdown_cell(title)]
#
        for k,i in sousChapitre.dicosschap.items():
            for k2, v2 in i.items():
                if v2 == 'description': 
                    desc = self.write_text_sans_nl(i["valeur"])
                    desc += '\n'
                    self.nb['cells'] += [nbf.v4.new_markdown_cell(desc)]
                if v2 == 'figure':
                    self.inclureObjet(maitre,i["valeur"])
#
    def inclurePurposeNotebook(self,maitre,purpose):
        '''Inclusion du paragraphe Purpose dans le fichier latex du rapport de validation.'''
        purp = "## Purpose \n \n" 
        if len(purpose.description)!=0: purp += self.write_description(purpose.description)
        iter = 0

        #ajout des figures
        for figure in purpose.listeFigures:
            self.inclureObjet(maitre,figure)
            pass
#        fichier.write_Tex('\\\\')
        # fichier.write_Tex_sans_nl('\n \vspace{0.3cm}')
        suite_entete=' Validation made by : __AUTEUR__ \n \n Report generated  __DATE__'
        suite_entete = suite_entete.replace('__AUTEUR__',chaine2Tex(maitre.auteur))
        import time
        date = time.strftime('%d/%m/%Y')
        suite_entete = suite_entete.replace('__DATE__',chaine2Tex(date))
        purp += self.write_text(suite_entete)
        self.nb['cells'] = [nbf.v4.new_markdown_cell(purp)]
#
    def inclurePbDescriptionNotebook(self,maitre,pbdescription):
        '''Inclusion du paragraphe Problem Description dans le fichier latex du rapport de validation.'''
        desc = '## Problem Description \n \n'
        if len(pbdescription.description)!=0: desc += self.write_description(pbdescription.description)
        iter = 0
        
        self.nb['cells'] += [nbf.v4.new_markdown_cell(desc)]

        #ajout des figures
        for figure in pbdescription.listeFigures:
            self.inclureObjet(maitre,figure) 

        #ajout des sous-chapitres
        for sousChapitre in pbdescription.listeSSChap:
            self.inclureSSChapitreTex(maitre,sousChapitre)
#
    def inclureCaseSetupNotebook(self,maitre,casesetup):
        '''Inclusion du paragraphe Case Setup dans le fichier latex du rapport de validation.'''
        setup = "## Case Setup \n \n"
        if len(casesetup.description)!=0: setup += self.write_description(casesetup.description)
        self.nb['cells'] += [nbf.v4.new_markdown_cell(setup)]

        #ajout des figures
        for figure in casesetup.listeFigures:
            self.inclureObjet(maitre,figure)

        #ajout des sous-chapitres
        for sousChapitre in casesetup.listeSSChap:
            self.inclureSSChapitreTex(maitre,sousChapitre)
#
    def inclureResultsTex(self,maitre,results):
        '''Inclusion du paragraphe Results dans le fichier latex du rapport de validation.'''
        res = "## Results \n"
        res += "### Validation Specific Informations \n"
        version=maitre.versionTrioU
        res += '* Version %s : %s \n' % (maitre.code,chaine2Tex(version))
#
        for param in maitre.parametresTrioU:
            res += '* ' + self.write_text(param)
            pass
#
        if maitre.casTest:
            res += '* Generated Test cases : \n'
            for cas in maitre.casTest:
                from lib import get_detail_cas
                nomcas,dir,nb_proc,comment=get_detail_cas(cas)
                ficData=nomcas+'.data'
                res += '\t- %s : %s \n' % (chaine2Tex(dir+"/"+ficData), chaine2Tex(comment))
        self.nb['cells'] += [nbf.v4.new_markdown_cell(res)]

        testCases = "from trustutils import run\n \n"
        for cas in maitre.casTest:
            from lib import get_detail_cas
            nomcas,dir,nb_proc,comment=get_detail_cas(cas)
            ficData=nomcas+'.data'
            testCases += "run.addCase(\""+ dir + "\",\"" + ficData
            if nb_proc>1:
                testCases += "\",nbProcs=" + str(nb_proc) + ")\n"
            else:
                testCases += "\")\n"
            pass
        testCases += "run.runCases()"
        self.nb['cells'] += [nbf.v4.new_code_cell(testCases)]

        from Tableau import Tableau_performance_nvellevalid
        Tableau_perf=Tableau_performance_nvellevalid(verbose=0, output='')
        fichier=nomcas+'.perf'
        Tableau_perf.lireParametres(fichier,maitre.casTest)
        Tableau_perf.titre='Performance Chart'
        self.inclureTableauNotebook(maitre,Tableau_perf)
        pass
        iter = 0
        data = "### Plot Data \n \n"
        self.nb['cells'] += [nbf.v4.new_markdown_cell(data)]
        for k,i in results.dicoresult.items():
            for k2, v2 in i.items():
                if v2 == 'description': 
                    desc = self.write_text(i["valeur"])
                    self.nb['cells'] += [nbf.v4.new_markdown_cell(desc)]
                if v2 == 'figure':
                    self.inclureObjet(maitre,i["valeur"])
#
    def inclureConclusionTex(self,maitre,conclusion):
        '''Inclusion du paragraphe Conclusion dans le fichier latex du rapport de validation.'''
##        if not conclusion.description:   - a faire : tester existance du paragrpahe
        ccl = "## Conclusion \n \n"
        ccl += self.write_description(conclusion.description)
        iter = 0
        self.nb['cells'] += [nbf.v4.new_markdown_cell(ccl)]

        #ajout des figures
        for figure in conclusion.listeFigures:
            self.inclureObjet(maitre,figure)
            pass
        pass

    def write_liste_cas(self,maitre):
        if maitre.casTest:
            testCases = "from trustutils import run\n \n"
            for cas in maitre.casTest:
                from lib import get_detail_cas
                nomcas,dir,nb_proc,comment=get_detail_cas(cas)
                ficData=nomcas+'.data'
                testCases += "run.addCase(\""+ dir + "\",\"" + ficData
                if nb_proc>1:
                    testCases += "\",nbProcs=" + str(nb_proc) + ")\n"
                else:
                    testCases += "\")\n"
                if comment: testCases += "# " + comment +"\n"
                pass
            testCases += "run.printCases()\n"
            testCases += "run.runCases()"
            self.nb['cells'] += [nbf.v4.new_code_cell(testCases)]
            pass
        pass

    def write_liste_ref(self,maitre):
        if maitre.reference:
            if maitre.nvellevalid==2:
                liste_ref = "### References: \n \n"
            else:
                liste_ref = "## References: \n \n"
            for ref in maitre.reference:
                liste_ref += "* : %s \n"% (chaine2Tex(ref))
                pass
            self.nb['cells'] += [nbf.v4.new_markdown_cell(liste_ref)]
            pass
        pass

    def include_data(self,maitre):
        if maitre.inclureData>0:
            first=1
            title = "## Data Files"
            self.nb['cells'] += [nbf.v4.new_markdown_cell(title)]
            
            for cas in maitre.casTest:
                from lib import get_detail_cas
                nomcas,dir,nb_proc,comment=get_detail_cas(cas)
 
                if ((maitre.inclureData==1) or ((maitre.inclureData==2) and (comment!="") )) :
                    if (first):
                        first=0
                        code = "run.dumpDataset(\"%s\")"%nomcas
                        self.nb['cells'] += [nbf.v4.new_code_cell(code)]
                        pass
                    pass
                pass
            pass
    def write_fichiers(self,maitre,dico):
        # generation du notebook
        self.notebook=dico['nomFichierNotebook'] + '.ipynb'
        if maitre.nvellevalid==2:
        # lecture des chapitres uniquement si on est a l ancien format des fiches de validation
            self.ecrire_introduction(maitre)
            self.write_liste_cas(maitre)
            self.write_liste_ref(maitre)
            for chapitre in maitre.listeChapitres:
                self.inclureChapitre(maitre,chapitre)
                pass
        else:
            for purp in maitre.purpose:
                self.inclurePurposeNotebook(maitre,purp)
            for pbdes in maitre.pbdescription:
                self.inclurePbDescriptionNotebook(maitre,pbdes)
            for casesu in maitre.casesetup:
                self.inclureCaseSetupNotebook(maitre,casesu)
            for resu in maitre.results:
                self.inclureResultsTex(maitre,resu)
            for conc in maitre.conclusion:
                self.inclureConclusionTex(maitre,conc)
            self.write_liste_ref(maitre)
#
##############################
        ## inclusion des fichiers.data
        self.include_data(maitre)

        nbf.write(self.nb, self.notebook)

        pass
#
    def ecrire_introduction(self,maitre):
        
        title = "# " + chaine2Tex(maitre.titre)

        date = time.strftime('%d/%m/%Y')
        
        intro = "## Introduction\n \n Validation made by: " + chaine2Tex(maitre.auteur) + "\n \n"
        intro += " Report generated " + chaine2Tex(date)

        description = "### Description\n \n" + self.write_description(maitre.description)

        #parameter = "from trustutils import run\nrun.TRUST_parameters()" #TODO option parametre
       
        parameter = "### TRUST parameters\n \n"

        version=maitre.versionTrioU
        parameter += ('* Version %s : %s \n' % (maitre.code,chaine2Tex(version)))
        try:
            binary = os.environ.get("exec", "[UNKNOWN]")
            origin = os.getcwd()
            path = os.path.join(origin, BUILD_DIRECTORY)

            version=binary+" (built on TRUST v"+path+")"
            parameter += '* Binary: %s \n' % chaine2Tex(version)
        except:
            pass

        for param in maitre.parametresTrioU:
            parameter+= ('* %s \n')%param
            pass
        pass

        self.nb['cells'] = [nbf.v4.new_markdown_cell(title),
                nbf.v4.new_markdown_cell(intro),
                nbf.v4.new_markdown_cell(description),
                nbf.v4.new_markdown_cell(parameter)]
