nvc++ -mp -target=gpu condition_reduction.cpp -o test && ./test
# Erreur qui se produit avec le HPC SDK 23.5 et pas avec le 22.1:
# NVC++-S-0155-Duplicate name in reduction clause - sum (condition_reduction.cpp: 8)
# NVC++/x86-64 Linux 23.5-0: compilation completed with severe errors
# Si on supprime le if (kernelOnDevice) cela fonctionne. En attendant on dupplique le code ! 
