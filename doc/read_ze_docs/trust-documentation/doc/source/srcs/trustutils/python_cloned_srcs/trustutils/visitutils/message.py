"""
class GestionMessage recovered from  Validation/Outils/Genere_courbe/src/lib.py
"""
import sys, traceback


class GestionMessages:
    # drapeau des messages
    _ERR = 0
    _AVERT = 1
    _INFO = 2
    _DEBOG = 3
    __drapeau = ["ERROR", "WARNING", "INFO", "DEBOG"]

    def __init__(self, verbose=10, output="", ecran=True):
        self.setNiveauMessage(verbose)
        self.setOutputFile(output)
        self.ecran = ecran

    def setNiveauMessage(self, verbose):
        self.NiveauMax = verbose

    def setOutputFile(self, output):
        self.OutputFile = output
        if output != "":
            self.OutputFileF = open(self.OutputFile, "w")

    def ecrire(self, criticite, message, arret=False, usage=False, niveau=0, texteUsage="", fichier=None):
        """Procedure d ecriture d un message."""
        # print 'ecrire %s criticite=%d, niveau=%d, niv max=%d' % (message, criticite, niveau, self.NiveauMax)
        if criticite > len(GestionMessages.__drapeau):
            criticite = 0
        # en cas d'erreur : demande l'arret du code
        if criticite == GestionMessages._ERR:
            arret = True
        # test l'affichage du message, selon le niveau de verbosite
        if (criticite in (GestionMessages._ERR, GestionMessages._AVERT)) or niveau <= self.NiveauMax:
            if self.ecran:
                print("%s : %s" % (GestionMessages.__drapeau[criticite], message))
            if self.OutputFile != "":
                self.OutputFileF.write("%s : %s\n" % (GestionMessages.__drapeau[criticite], message))
            if criticite == GestionMessages._ERR:
                callStack = traceback.extract_stack()
                frame = callStack[-2]
                if frame[2] == "ecrire_usage" or frame[2] == "verifie_accolade_suivante":
                    frame = callStack[-3]
                if self.ecran:
                    print("%s  :  Localisation = fonction %s (fichier %s, ligne %d) : \n\t%s" % (GestionMessages.__drapeau[criticite], frame[2], frame[0], frame[1], frame[3]))
                if self.OutputFile != "":
                    self.OutputFileF.write("%s  :  Localisation = fonction %s (fichier %s, ligne %d) : \n\t%s\n" % (GestionMessages.__drapeau[criticite], frame[2], frame[0], frame[1], frame[3]))
                    if fichier:
                        toto = fichier.readlines()
                        nb_l = len(toto)
                        fichier_0 = open(self.parametersFile, "r")
                        toto = fichier_0.readlines()
                        nb_l0 = len(toto)
                        curentline = nb_l0 - nb_l
                        message_info = "Error at the line %s of %s" % (curentline, self.parametersFile)
                        if self.ecran:
                            print("%s : %s" % (GestionMessages.__drapeau[criticite], message_info))
                        if self.OutputFile != "":
                            self.OutputFileF.write("%s : %s\n" % (GestionMessages.__drapeau[criticite], message_info))
                            pass
                        pass
        # imprime l'usage si besoin
        if texteUsage != "":
            if self.ecran:
                print(texteUsage)
            if self.OutputFile != "":
                self.OutputFileF.write("%s\n" % (texteUsage))
        if self.OutputFile != "":
            self.OutputFileF.flush()
        sys.stdout.flush()
        # provoque l'arret du code si besoin
        if arret:
            sys.exit(2)
            pass
        pass
