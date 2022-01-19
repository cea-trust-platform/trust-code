# AmgX solveurs
#
#           iter      Mem Usage (GB)       residual           rate
#         --------------------------------------------------------------
#            Ini            0.624084   6.437832e-09
#              0            0.624084   9.323002e-10         0.1448
#              1              0.6241   1.739630e-09         1.8660
#              
#             17              0.6241   1.585329e-13         0.3371
#             18              0.6241   6.780941e-14         0.4277
#         --------------------------------------------------------------
#         Total Iterations: 19
#         Avg Convergence Rate: 		         0.5471
#         Final Residual: 		   6.780941e-14
#         Total Reduction in Residual: 	   1.053296e-05
#         Maximum Memory Usage: 		          0.624 GB
#         --------------------------------------------------------------
#Total Time: 0.0235337
#    setup: 0.00923546 s
#    solve: 0.0142982 s
#    solve(per iteration): 0.000752537 s
#[AmgX] Time to solve on GPU: 0.052976
#
/iter/ && /residual/ { ns++;
    getline;getline
    if (ns == ncomp) {
        res0 = $3
        printf("%d %g\n",0,1) > res;
        getline;
        while (NF>=3) {
             printf("%d %g\n",$1+1,$3/res0) > res;
             getline;
        }
    }
}

