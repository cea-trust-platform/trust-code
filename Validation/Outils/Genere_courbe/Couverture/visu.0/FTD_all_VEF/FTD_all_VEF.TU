Statistiques d'initialisation du calcul

Temps total                 1.77583

Statistiques de resolution du probleme

Temps total                 3.77862
Timesteps                   10
Secondes / pas de temps     0.320214
Nb solveur / pas de temps   7
Secondes / solveur          0.0183486
Iterations / solveur        22.3571
Communications avg        0.4 % of total time
Communications max        0.4 % of total time
Communications min        0.4 % of total time
Network latency min       0 ms
Network bandwidth max     0 MB/s
Total network traffic     0 MB / timestep
Average message size      0 kB
Min waiting time          0.4 % of total time
Max waiting time          0.4 % of total time
Avg waiting time          0.4 % of total time

Timesteps = nombre de pas de temps
Nb solveur = nombre de resolutions de systeme lineaire
Iterations = nombre moyen d'iterations du solveur
Communications = fraction du temps passe dans les
                 communications entre processeurs (hors io fichiers)
Network latency = minimum sur l'ensemble des processeurs
                  du temps moyen d'un mp_sum
Network bandwidth = maximum sur l'ensemble des processeurs
                    de la bande passante moyenne
Waiting time = estimation du temps d'attente des differents processeurs

Max_waiting_time grand  => probablement mauvais decoupage
Communications > 30%    => trop de processeurs ou reseau trop lent

