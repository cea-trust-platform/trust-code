# ****************************************************************************
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
# *****************************************************************************

# -*- coding: utf-8 -*-

__version__ = "0.1"

import os
import time
import re
from lib import chaine2Tex
from lib import GestionMessages
from lib import get_nom_cas
import nbformat as nbf

from Visu import dico_plots


class Write_notebook:
    def __init__(self, disable_parallel):
        self.nb_visu = 0
        self.minifigure_avt = 0
        self.nb = nbf.v4.new_notebook()
        self.disable_parallel = disable_parallel
        pass

    def write_text_sans_nl(self, titi):
        return chaine2Tex(titi)

    def write_text(self, titi):
        text = self.write_text_sans_nl(titi)
        text += "\n"
        return text

    def write_description_sans_nl(self, description):
        if len(description) == 0:
            return
        text = f"{self.write_text_sans_nl(self.Textojupyter(description[0]))}\n"
        for i in range(1, len(description)):
            text += self.write_text_sans_nl(self.Textojupyter(description[i]))
            text += "\n "
            pass
        text += "\n"
        return text

    def write_description(self, description):
        text = self.write_description_sans_nl(description)
        return text

    def replace_fonction(self, fonction):
        text = fonction
        text = text.replace("cos", "np.cos")
        text = text.replace("sin", "np.sin")
        text = text.replace("exp", "np.exp")
        text = text.replace("log", "np.log")
        return text

    def Textojupyter(self, text):
        text = text.replace("$\\bullet$", "* ")
        text = text.replace("- ", "\t - ")
        return text

    def verifie_existance_fichier(self, nom_fichier, objet):
        iter = 0
        while iter < 2 and (not os.path.isfile(nom_fichier)):
            objet.gestMsg.ecrire(
                GestionMessages._DEBOG,
                f"Search file {nom_fichier} from {os.getcwd()}",
            )
            time.sleep(1)
            iter += 1
            # print 'waiting %d' % iter
            pass
        if not os.path.isfile(nom_fichier):
            objet.gestMsg.ecrire(
                GestionMessages._ERR,
                f"Error, graphic file {nom_fichier} is not generated ",
            )
            pass
        pass


    # inclusion de la figure dans le fichier latex
    def inclureFigureNotebook(self, maitre, figure):
        """Inclusion de la figure dans le fichier latex du rapport de validation."""
        minifigure = self.minifigure_avt
        if figure.nb_img_without_newline > 0:
            minifigure = 1
            pass
        title = ""
        # if (figure.titre!="Undefined"):
        #    title = "### " + chaine2Tex(figure.titre) + "\n \n "

        # TODO voir pour minifigure et markers avec style + options linewidth...

        if len(figure.description) != 0:
            title += self.write_description(figure.description)

        # figure.printFichierParametres()
        if len(figure.listeCourbes) > 0:
            code = "from trustutils import plot \n \n"
            titre = ""
            if figure.titre_figure != "Undefined" and len(figure.titre_figure) > 0:
                titre = figure.titre_figure
            elif figure.titre != "Undefined" and len(figure.titre) > 0:
                titre = figure.titre
            if len(titre) == 0:
                code += "fig = plot.Graph() \n"
            else:
                code += f"fig = plot.Graph(r\"{chaine2Tex(titre)}\") \n"

            for courbe in figure.listeCourbes:
                style = ""
                if courbe.style == "Undefined":
                    style = ",marker='+'"
                elif courbe.style == "points":
                    style = ",marker='+'"
                elif courbe.style == "linespoints":
                    style = ",marker='-x'"
                elif courbe.style == "lines":
                    style = ",marker='-'"
                if courbe.points != "Undefined":
                    param = courbe.segment.split()
                    ficSon = f"{get_nom_cas(param[0])}_{param[1].upper()}.son"
                    code += (
                        f"fig.addPoint(\"{ficSon}\",label=r\"{chaine2Tex(courbe.legende)}\"{style}) \n"
                    )
                elif courbe.segment != "Undefined":
                    param = courbe.segment.split()
                    ficSon = f"{get_nom_cas(param[0])}_{param[1].upper()}.son"

                    code += (
                        f"fig.addSegment(\"{ficSon}\",label=r\"{chaine2Tex(courbe.legende)}\"{style}) \n"
                    )
                else:
                    if courbe.fichier != "Undefined":
                        code += f'data = plot.loadText("{courbe.fichier}")\n'
                        if courbe.colonnes != "Undefined":
                            indices = [
                                int(s) for s in re.findall(r"\$(\d+)", courbe.colonnes)
                            ]

                            formule = courbe.colonnes
                            for j in indices:
                                original_string = f"${j}"
                                replacement_string = f"data[{j - 1}]"
                                formule = formule.replace(
                                    original_string, replacement_string
                                )
                            formule = self.replace_fonction(formule)  # TODO a finir

                            colonnes = formule.split()
                            xdata = colonnes[0]
                            ydata = colonnes[1]
                            code += (
                                f"fig.add({xdata},{ydata},label=r\"{chaine2Tex(courbe.legende)}\"{style})\n"
                            )  # TODO Attention ne permet pas l'existence d'espace dans la formule
                        else:
                            code += (
                                f"fig.add(data[0],data[1],label=r\"{chaine2Tex(courbe.legende)}\"{style})\n"
                            )  # TODO Attention ne permet pas l'existence d'espace dans la formule

                    else:
                        code += "\nimport numpy as np \n"
                        rangeX = [0, 1]
                        point_number = 1000
                        if figure.rangeX != "auto":
                            rangeX = figure.rangeX.split()
                        if figure.logX:
                            point_number = 10000
                        code += f"x = np.linspace({float(rangeX[0])},{float(rangeX[1])},{point_number}) \n"
                        fonction = self.replace_fonction(courbe.fonction)
                        code += f"y = {fonction} \n"
                        code += (
                            f"fig.add(x,y,label=r\"{chaine2Tex(courbe.legende)}\"{style}) \n"
                        )
            if figure.labelX != "Undefined" or figure.labelY != "Undefined":
                code += f'\nfig.label(r"{chaine2Tex(figure.labelX)}",r"{chaine2Tex(figure.labelY)}")\n'
            if figure.rangeX != "auto" or figure.rangeY != "auto":
                if figure.rangeX == "auto" and figure.rangeY != "auto":
                    rangeY = figure.rangeY.split()
                    code += f"\nfig.visu(ymin={rangeY[0]},ymax={rangeY[1]})\n"
                elif figure.rangeY == "auto" and figure.rangeX != "auto":
                    rangeX = figure.rangeX.split()
                    code += f"\nfig.visu(xmin={rangeX[0]},xmax={rangeX[1]})\n"
                elif figure.rangeY != "auto" and figure.rangeX != "auto":
                    rangeX = figure.rangeX.split()
                    rangeY = figure.rangeY.split()
                    code += f"\nfig.visu({rangeX[0]},{rangeX[1]},{rangeY[0]},{rangeY[1]})\n"
            if figure.logX or figure.logY:
                if not figure.logX:
                    code += "\nfig.scale(yscale='log')\n"
                elif not figure.logY:
                    code += "\nfig.scale(xscale='log')\n"
                else:
                    code += "\nfig.scale('log','log')\n"
                pass

            if len(title) > 0:
                self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]
            self.nb["cells"] += [nbf.v4.new_code_cell(code)]
        else:
            # motcle picture/image
            path = os.path.join("src", figure.image)
            title += f"![]({path})"
            self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]
        pass

    # inclusion de la visu dans le fichier latex
    def inclureVisuNotebook(self, maitre, visu):
        """Inclusion de la visu dans le fichier latex du rapport de validation."""
        code = "from trustutils import visit\n \n"
        title = ""
        if visu.titre != "Undefined":
            title = f"### {chaine2Tex(visu.titre)}\n \n "
        # minifigure = self.minifigure_avt
        # TODO minifigure
        if len(visu.description) != 0:
            title += self.write_description(visu.description)
        if len(title) != 0:
            self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]
        # visu.printFichierParametres()
        show = 0
        code0 = ""
        code1 = ""
        for plot in visu.plot:
            if plot[0] in list(dico_plots.keys()):  # TODO a voir pour blackvector
                if not show:
                    code0 += f'visu = visit.Show("{plot[1]}","{dico_plots[plot[0]]}","{plot[3]}_{plot[4]}_{plot[2]}",mesh="{plot[2]}")\n'
                    show = 1
                else:
                    code1 += f'visu.addField("{plot[1]}","{dico_plots[plot[0]]}","{plot[3]}_{plot[4]}_{plot[2]}",mesh="{plot[2]}")\n'

            if plot[0] == "mesh":
                if not show:
                    code0 += f'visu = visit.Show("{plot[1]}","Mesh","{plot[2]}")\n'
                    show = 1
                else:
                    code1 += f'visu.addField("{plot[1]}","Mesh","{plot[2]}")\n'

            if plot[0].lower() == "normal3d":
                code1 += f"visu.normal3D([{plot[1]},{plot[2]},{plot[3]}])\n"
            if plot[0].lower() == "up3d":
                code1 += f"visu.up3D([{plot[1]},{plot[2]},{plot[3]}])\n"
            if plot[0].lower() == "zoom3d":
                code1 += f"visu.zoom3D([{plot[1]},{plot[2]},{plot[3]}])\n"
            if plot[0].lower() == "zoom2d":
                code1 += f"visu.zoom2D([{plot[1]},{plot[2]},{plot[3]},{plot[4]}])\n"
            if (plot[0].lower() == "operator") or (
                plot[0].lower() == "operator_to_all"
            ):
                if (plot[1].lower() == "slice") or (plot[1].lower() == "slice2d"):
                    code1 += (
                        f'visu.slice(origin=[{plot[2]},{plot[3]},{plot[4]}],normal=[{plot[5]},{plot[6]},{plot[7]}],type_op="{plot[1].lower()}")\n'
                    )
                else:
                    code1 += f'visu.visuOptions(["{plot[1]}"])\n'
            if plot[0].lower() == "instruction_visit":
                code1 += f"visu.executeVisitCmds({plot[1]})\n"
        if len(visu.cycles) != 0:
            code1 += f"visu.iteration({visu.cycles} )\n"
        code += f"{code0 + code1}visu.plot()"

        self.nb["cells"] += [nbf.v4.new_code_cell(code)]
        pass

    def inclureTableauNotebook(self, maitre, tableau):
        """Inclusion du tableau dans le fichier latex du rapport de validation."""

        title = ""
        if tableau.titre != "Undefined":
            title += f"### {chaine2Tex(tableau.titre)}\n \n "
        if len(tableau.description) != 0:
            title += self.write_description(tableau.description)
        if len(title) != 0:
            self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]

        if len(tableau.listeLignes) == 0:
            return

        from Tableau import Tableau_performance, Tableau_performance_nvellevalid

        if isinstance(tableau, Tableau_performance) or isinstance(
            tableau, Tableau_performance_nvellevalid
        ):
            code = "run.tablePerf()"
            self.nb["cells"] += [nbf.v4.new_code_cell(code)]
            return

        nbc = 0
        if tableau.formule:
            nbc += 1
        # tableau.printFichierParametres()

        code = "from trustutils import plot \n \n"
        columns = chaine2Tex(tableau.label).split("|")
        code += f"columns={columns} \n"
        code += "tab = plot.Table(columns)\n"

        for ligne in tableau.listeLignes:
            from Ligne import Ligne, Lignes

            if not isinstance(ligne, Lignes) and ligne.valeurs != "Undefined":
                valeur_f = ligne.valeurs.split()
                for i in range(len(valeur_f)):
                    try:
                        valeur_f[i] = eval(valeur_f[i])
                    except:
                        valeur_f[i] = chaine2Tex(valeur_f[i])
                        # pass
                code += f'tab.addLine([{valeur_f}],r"{chaine2Tex(ligne.legende)}")\n'

            elif ligne.fichier != "Undefined":
                code += f'data = plot.loadText("{ligne.fichier}"'
                if not isinstance(ligne, Lignes):
                    code += ',transpose=False, dtype="str")\n'
                else:
                    code += ',transpose=False, dtype="str", skiprows={ligne.numero_premiere_ligne - 1})\n'

            nb_colonnes_f = tableau.nb_colonnes
            if ligne.nb_colonnes_fichier:
                nb_colonnes_f = ligne.nb_colonnes_fichier
            try:
                if ligne.colonnes:
                    colonnes = ligne.colonnes.split()
                    if isinstance(ligne, Lignes):
                        nb_ligne = (
                            ligne.numero_derniere_ligne
                            + 1
                            - ligne.numero_premiere_ligne
                        )
                        decal = 1
                        num_ligne = 0
                    else:
                        nb_ligne = 1
                        decal = 0
                        num_ligne = -1
                    for ll in range(nb_ligne):
                        code += "tab.addLine([["
                        for i in range(tableau.nb_colonnes):
                            formule = colonnes[i + decal]

                            size_fichier = -1
                            if ligne.fichier != "Undefined":
                                path = f"build/{ligne.fichier}"
                                if os.path.exists(path):
                                    with open(path, "r") as fp:
                                        size_fichier = len(fp.readlines())

                            for j in range(nb_colonnes_f):
                                original_string = f"${j + 1}"
                                if size_fichier == 1:
                                    replacement_string = f"data[{j}]"
                                else:
                                    replacement_string = f"data[{num_ligne}][{j}]"
                                formule = formule.replace(
                                    original_string, replacement_string
                                )
                                formule = formule.replace("rien", '""')
                                pass
                            pass
                            code += f"{formule},"
                        pass
                        if isinstance(ligne, Lignes):
                            code += f"]],data[{ll}][0])\n"
                        else:
                            code += f']],r"{chaine2Tex(ligne.legende)}")\n'
                        num_ligne += 1
                pass
            except:
                ligne.gestMsg.ecrire(
                    GestionMessages._ERR,
                    f"unable to read {nb_colonnes_f} values in file {ligne.fichier}."
                    ,
                )
        if tableau.titre != "Undefined":
            titre = tableau.titre.replace('"', "")
            code += f'tab.setTitle("{chaine2Tex(titre)}")\n'

        code += "display(tab)"
        self.nb["cells"] += [nbf.v4.new_code_cell(code)]
        pass

    def inclureObjet(self, maitre, figure):
        from Visu import Visu
        from Figure import Figure
        from Tableau import Tableau

        if isinstance(figure, Visu):
            self.inclureVisuNotebook(maitre, figure)
        elif isinstance(figure, Figure):
            self.inclureFigureNotebook(maitre, figure)
        elif isinstance(figure, Tableau):
            self.inclureTableauNotebook(maitre, figure)
        else:
            figure.gestMsg.ecrire(
                GestionMessages._ERR, f"Unexpected figure type {figure}"
            )
            pass
        pass

    #
    def inclureChapitre(self, maitre, chapitre):
        """Inclusion du chapitre dans le notebook."""
        title = ""
        if chapitre.titre != "Undefined":
            title = f"## {chaine2Tex(chapitre.titre)}\n \n"
        if len(chapitre.description) != 0:
            title += self.write_description(chapitre.description)

        if len(title) != 0:
            self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]

        # ajout des figures
        for figure in chapitre.listeFigures:
            self.inclureObjet(maitre, figure)
            pass
        pass

    #
    def inclureSSChapitreTex(self, maitre, sousChapitre):
        """Inclusion des sous-chapitres dans le fichier latex du rapport de validation."""

        if sousChapitre.titre != "Undefined":
            title = f"### {chaine2Tex(sousChapitre.titre)} \n"
            self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]
        #
        for k, i in sousChapitre.dicosschap.items():
            for k2, v2 in i.items():
                if v2 == "description":
                    desc = self.write_text_sans_nl(i["valeur"])
                    desc += "\n"
                    self.nb["cells"] += [nbf.v4.new_markdown_cell(desc)]
                if v2 == "figure":
                    self.inclureObjet(maitre, i["valeur"])

    #
    def inclurePurposeNotebook(self, maitre, purpose):
        """Inclusion du paragraphe Purpose dans le fichier latex du rapport de validation."""
        purp = "## Purpose \n \n"
        if len(purpose.description) != 0:
            purp += self.write_description(purpose.description)

        # ajout des figures
        for figure in purpose.listeFigures:
            self.inclureObjet(maitre, figure)
            pass
        #        fichier.write_Tex('\\\\')
        # fichier.write_Tex_sans_nl('\n \vspace{0.3cm}')
        suite_entete = (
            " Validation made by : __AUTEUR__ \n \n Report generated  __DATE__"
        )
        suite_entete = suite_entete.replace("__AUTEUR__", chaine2Tex(maitre.auteur))
        import time

        date = time.strftime("%d/%m/%Y")
        suite_entete = suite_entete.replace("__DATE__", chaine2Tex(date))
        purp += self.write_text(suite_entete)
        self.nb["cells"] = [nbf.v4.new_markdown_cell(purp)]

        # cas test a faire tourner ici car trop tard dans inclureResultsNotebook
        testCases = "from trustutils import run\n \n"
        testCases += "run.reset() \n"
        for cas in maitre.casTest:
            from lib import get_detail_cas

            nomcas, dir, nb_proc, comment = get_detail_cas(cas)
            ficData = f"{nomcas}.data"
            testCases += f"run.addCase(\"{dir}\",\"{ficData}"
            if nb_proc > 1:
                testCases += f"\",nbProcs={str(nb_proc)})\n"
            else:
                testCases += '")\n'
            pass
        if self.disable_parallel:
            testCases += "run.runCases(preventConcurrent=True)"
        else:
            testCases += "run.runCases()"
        self.nb["cells"] += [nbf.v4.new_code_cell(testCases)]

    #
    def inclurePbDescriptionNotebook(self, maitre, pbdescription):
        """Inclusion du paragraphe Problem Description dans le fichier latex du rapport de validation."""
        desc = "## Problem Description \n \n"
        if len(pbdescription.description) != 0:
            desc += self.write_description(pbdescription.description)

        self.nb["cells"] += [nbf.v4.new_markdown_cell(desc)]

        # ajout des figures
        for figure in pbdescription.listeFigures:
            self.inclureObjet(maitre, figure)

        # ajout des sous-chapitres
        for sousChapitre in pbdescription.listeSSChap:
            self.inclureSSChapitreTex(maitre, sousChapitre)

    #
    def inclureCaseSetupNotebook(self, maitre, casesetup):
        """Inclusion du paragraphe Case Setup dans le fichier latex du rapport de validation."""
        setup = "## Case Setup \n \n"
        if len(casesetup.description) != 0:
            setup += self.write_description(casesetup.description)
        self.nb["cells"] += [nbf.v4.new_markdown_cell(setup)]

        # ajout des figures
        for figure in casesetup.listeFigures:
            self.inclureObjet(maitre, figure)

        # ajout des sous-chapitres
        for sousChapitre in casesetup.listeSSChap:
            self.inclureSSChapitreTex(maitre, sousChapitre)

    #
    def inclureResultsNotebook(self, maitre, results):
        """Inclusion du paragraphe Results dans le fichier latex du rapport de validation."""
        res = "## Results \n"
        res += "### Validation Specific Informations \n"
        version = maitre.versionTrioU
        res += f"* Version {maitre.code} : {chaine2Tex(version)} \n"
        #
        for param in maitre.parametresTrioU:
            res += f"* {self.write_text(param)}"
            pass
        #
        if maitre.casTest:
            res += "* Generated Test cases : \n"
            for cas in maitre.casTest:
                from lib import get_detail_cas

                nomcas, dir, nb_proc, comment = get_detail_cas(cas)
                ficData = f"{nomcas}.data"
                res += f"\t- {chaine2Tex(f'{dir}/{ficData}')} : {chaine2Tex(comment)} \n"
        self.nb["cells"] += [nbf.v4.new_markdown_cell(res)]

        from Tableau import Tableau_performance_nvellevalid

        Tableau_perf = Tableau_performance_nvellevalid(verbose=0, output="")
        fichier = f"{nomcas}.perf"
        Tableau_perf.lireParametres(fichier, maitre.casTest)
        Tableau_perf.titre = "Performance Chart"
        self.inclureTableauNotebook(maitre, Tableau_perf)
        pass
        iter = 0
        data = "### Plot Data \n \n"
        self.nb["cells"] += [nbf.v4.new_markdown_cell(data)]
        for k, i in results.dicoresult.items():
            for k2, v2 in i.items():
                if v2 == "description":
                    desc = self.write_text(i["valeur"])
                    self.nb["cells"] += [nbf.v4.new_markdown_cell(desc)]
                if v2 == "figure":
                    self.inclureObjet(maitre, i["valeur"])

    #
    def inclureConclusionNotebook(self, maitre, conclusion):
        """Inclusion du paragraphe Conclusion dans le fichier latex du rapport de validation."""
        ##        if not conclusion.description:   - a faire : tester existance du paragrpahe
        ccl = "## Conclusion \n \n"
        ccl += self.write_description(conclusion.description)
        iter = 0
        self.nb["cells"] += [nbf.v4.new_markdown_cell(ccl)]

        # ajout des figures
        for figure in conclusion.listeFigures:
            self.inclureObjet(maitre, figure)
            pass
        pass

    def write_liste_cas(self, maitre):
        if maitre.casTest:
            testCases = "from trustutils import run\n \n"
            testCases += "run.reset() \n"
            for cas in maitre.casTest:
                from lib import get_detail_cas

                nomcas, dir, nb_proc, comment = get_detail_cas(cas)
                ficData = f"{nomcas}.data"
                testCases += f"run.addCase(\"{dir}\",\"{ficData}"
                if nb_proc > 1:
                    testCases += f"\",nbProcs={str(nb_proc)})\n"
                else:
                    testCases += '")\n'
                if comment:
                    testCases += f"# {comment}\n"
                pass
            testCases += "run.printCases()\n"
            if self.disable_parallel:
                testCases += "run.runCases(preventConcurrent=True)"
            else:
                testCases += "run.runCases()"
            self.nb["cells"] += [nbf.v4.new_code_cell(testCases)]
            pass
        pass

    def write_liste_ref(self, maitre):
        if maitre.reference:
            if maitre.nvellevalid == 2:
                liste_ref = "### References: \n \n"
            else:
                liste_ref = "## References: \n \n"
            for ref in maitre.reference:
                liste_ref += f"* {chaine2Tex(ref)} \n"
                pass
            self.nb["cells"] += [nbf.v4.new_markdown_cell(liste_ref)]
            pass
        pass

    def include_data(self, maitre):
        if maitre.inclureData > 0:
            first = 1

            for cas in maitre.casTest:
                from lib import get_detail_cas

                nomcas, dir, nb_proc, comment = get_detail_cas(cas)
                ficData = f"{nomcas}.data"
                ficData = os.path.join(dir, ficData)

                if (maitre.inclureData == 1) or (
                    (maitre.inclureData == 2) and (comment != "")
                ):
                    if first:
                        first = 0
                        title = "## Data Files"
                        self.nb["cells"] += [nbf.v4.new_markdown_cell(title)]
                        pass
                    subtitle = f"### {chaine2Tex(nomcas)}"
                    self.nb["cells"] += [nbf.v4.new_markdown_cell(subtitle)]
                    code = f'run.dumpDatasetMD("{ficData}")'
                    self.nb["cells"] += [nbf.v4.new_code_cell(code)]
                    pass
                pass
            pass

    def write_fichiers(self, maitre, dico):
        # generation du notebook
        self.notebook = f"{dico['nomFichierNotebook']}.ipynb"
        if maitre.nvellevalid == 2:
            # lecture des chapitres uniquement si on est a l ancien format des fiches de validation
            self.ecrire_introduction(maitre)
            self.write_liste_cas(maitre)
            self.write_liste_ref(maitre)
            for chapitre in maitre.listeChapitres:
                self.inclureChapitre(maitre, chapitre)
                pass
        else:
            for purp in maitre.purpose:
                self.inclurePurposeNotebook(maitre, purp)
            for pbdes in maitre.pbdescription:
                self.inclurePbDescriptionNotebook(maitre, pbdes)
            for casesu in maitre.casesetup:
                self.inclureCaseSetupNotebook(maitre, casesu)
            for resu in maitre.results:
                self.inclureResultsNotebook(maitre, resu)
            for conc in maitre.conclusion:
                self.inclureConclusionNotebook(maitre, conc)
            self.write_liste_ref(maitre)
        #
        ##############################
        ## inclusion des fichiers.data
        self.include_data(maitre)

        nbf.write(self.nb, self.notebook)

        pass

    #
    def ecrire_introduction(self, maitre):
        title = f"# {chaine2Tex(maitre.titre)}"

        date = time.strftime("%d/%m/%Y")

        self.nb.metadata = {"title": maitre.titre[1:-1]}
        intro = (
            f"## Introduction\n \n Validation made by: {chaine2Tex(maitre.auteur)}\n \n"
        )
        intro += f" Report generated {chaine2Tex(date)}"

        self.nb["cells"] = [
            nbf.v4.new_markdown_cell(title),
            nbf.v4.new_markdown_cell(intro),
        ]

        if maitre.description != []:
            description = f"### Description\n \n{self.write_description(maitre.description)}"
            self.nb["cells"] += [nbf.v4.new_markdown_cell(description)]

        # parameter = "from trustutils import run\nrun.TRUST_parameters()" #TODO option parametre

        parameter = "### TRUST parameters\n \n"

        version = maitre.versionTrioU
        parameter += f"* Version {maitre.code} : {chaine2Tex(version)} \n"
        try:
            binary = os.environ.get("exec", "[UNKNOWN]")
            origin = os.getcwd()
            path = os.path.join(origin, BUILD_DIRECTORY)

            version = f"{binary} (built on TRUST v{path})"
            parameter += f"* Binary: {chaine2Tex(version)} \n"
        except:
            pass

        for param in maitre.parametresTrioU:
            parameter += f"* {param} \n"
            pass
        pass

        self.nb["cells"] += [nbf.v4.new_markdown_cell(parameter)]
