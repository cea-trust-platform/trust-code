import pandas as pd
import sys, os

jdd = sys.argv[1]

probes = {"TMIN" : 458.429373, "TMOY" : 875.088208, "TMOYP" : 875.088208, "TMAX" : 1291.74704}
balances = {"haut" : -1416.84953, "droite" : 1749.83282, "bas" : -83.5036506, "gauche" : -250.186005}

for b, val in probes.items():
    data = pd.read_csv(f"{jdd}_{b}.son", comment="#", sep='\\s+')
    final_value = data[data.columns[-1]].tolist()[-1]
    if abs(val - final_value) > 1e-2:
        print(f"Probes are not coherent! : {val:.2f} vs {final_value:.2f}")
        os.system(f"touch probes_ko")

for b, val in balances.items():
    data = pd.read_csv(f"{jdd}_pb_Diffusion_chaleur.out", comment="#", sep='\\s+')
    final_value = data[b].tolist()[-1]
    if abs(val - final_value) > 1e-2:
        print(f"Balances are not coherent! : {val:.2f} vs {final_value:.2f}")
        os.system(f"touch balances_ko")
