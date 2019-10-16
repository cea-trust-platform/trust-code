# -*- coding: utf-8 -*-

import sys

def traite_fichier(args):

    if not args:
        return

    for nom in args:
        with open(nom, 'r') as infile:
            lines = infile.readlines()

        data = lines[1].split()
        y = data[5::4]
        t_0 = lines[5].split()[1:]
        t_fin = lines[-1].split()[1:]

        with open(nom.replace('.son', '.dat1'), 'w') as outfile:
            outfile.write('\n'.join([' '.join(x) for x in zip(y, t_0)]))

        with open(nom.replace('.son', '.dat2'), 'w') as outfile:
            outfile.write('\n'.join([' '.join(x) for x in zip(y, t_fin)]))



if __name__ == "__main__":
    print "\nUsage : python traite_sonde.py fichier1 fichier2 ... fichierN\n"
    traite_fichier(sys.argv[1:])
