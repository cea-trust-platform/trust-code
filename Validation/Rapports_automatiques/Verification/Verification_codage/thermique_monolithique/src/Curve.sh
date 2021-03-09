for dis in VDF PolyMAC CoviMAC; do for type in mono seq; do for iter in avec sans; do
paste ${dis}/${type}_${iter}_iteration_pb1_Diffusion_chaleur.out ${dis}/${type}_${iter}_iteration_pb2_Diffusion_chaleur.out > ${dis}/${type}_${iter}_iteration_pb1pb2_Diffusion_chaleur.out
grep 'The Probleme_Couple problem pbc has converged after' ${dis}/${type}_${iter}_iteration.out | awk '{print $8}'          > ${dis}/${type}_${iter}_iteration.txt
done; done; done
