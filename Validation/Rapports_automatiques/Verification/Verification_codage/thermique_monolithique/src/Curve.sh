paste VDF/mono_avec_iteration_pb1_Diffusion_chaleur.out VDF/mono_avec_iteration_pb2_Diffusion_chaleur.out 		> VDF/mono_avec_iteration_pb1pb2_Diffusion_chaleur.out
paste VDF/mono_sans_iteration_pb1_Diffusion_chaleur.out VDF/mono_sans_iteration_pb2_Diffusion_chaleur.out		> VDF/mono_sans_iteration_pb1pb2_Diffusion_chaleur.out
paste VDF/seq_avec_iteration_pb1_Diffusion_chaleur.out VDF/seq_avec_iteration_pb2_Diffusion_chaleur.out			> VDF/seq_avec_iteration_pb1pb2_Diffusion_chaleur.out
paste VDF/seq_sans_iteration_pb1_Diffusion_chaleur.out VDF/seq_sans_iteration_pb2_Diffusion_chaleur.out			> VDF/seq_sans_iteration_pb1pb2_Diffusion_chaleur.out
paste PolyMAC/mono_avec_iteration_pb1_Diffusion_chaleur.out PolyMAC/mono_avec_iteration_pb2_Diffusion_chaleur.out	> PolyMAC/mono_avec_iteration_pb1pb2_Diffusion_chaleur.out
paste PolyMAC/mono_sans_iteration_pb1_Diffusion_chaleur.out PolyMAC/mono_sans_iteration_pb2_Diffusion_chaleur.out	> PolyMAC/mono_sans_iteration_pb1pb2_Diffusion_chaleur.out
paste PolyMAC/seq_avec_iteration_pb1_Diffusion_chaleur.out PolyMAC/seq_avec_iteration_pb2_Diffusion_chaleur.out 	> PolyMAC/seq_avec_iteration_pb1pb2_Diffusion_chaleur.out
paste PolyMAC/seq_sans_iteration_pb1_Diffusion_chaleur.out PolyMAC/seq_sans_iteration_pb2_Diffusion_chaleur.out		> PolyMAC/seq_sans_iteration_pb1pb2_Diffusion_chaleur.out
grep 'The Probleme_Couple problem pbc has converged after' VDF/mono_avec_iteration.out | awk '{print $8}'	> VDF/mono_avec_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' VDF/mono_sans_iteration.out | awk '{print $8}'	> VDF/mono_sans_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' VDF/seq_avec_iteration.out | awk '{print $8}'	> VDF/seq_avec_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' VDF/seq_sans_iteration.out | awk '{print $8}'	> VDF/seq_sans_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' PolyMAC/mono_avec_iteration.out | awk '{print $8}'	> PolyMAC/mono_avec_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' PolyMAC/mono_sans_iteration.out | awk '{print $8}'	> PolyMAC/mono_sans_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' PolyMAC/seq_avec_iteration.out | awk '{print $8}'	> PolyMAC/seq_avec_iteration.txt
grep 'The Probleme_Couple problem pbc has converged after' PolyMAC/seq_sans_iteration.out | awk '{print $8}'	> PolyMAC/seq_sans_iteration.txt

