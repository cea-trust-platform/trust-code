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
/Implicited diffusion algorithm/ { ns++;
    if (ns == ncomp)
    {
        while ($1!="iter" && $1!="Norm") getline;
        if ($1=="iter") {
           getline;getline
           res0 = $3
           printf("%d %g\n",0,1) > res;
           getline;
           while (NF>=3) {
             printf("%d %g\n",$1+1,$3/res0) > res;
             getline;
           }
        }
        if ($1=="Norm") {
           res0 = $5
           it=0
           printf("%d %g %g\n",it,1,res0) > res;
           getline;
           while ($1!="Final") {
              col=1
              while (col<=NF) {
                 it++
                 printf("%d %g %g\n",it,$(col)/res0,$(col)) > res;
                 col++
              }
              getline;
           }
        }
    }
}
# Petsc Solvers
# Implicited diffusion algorithm applied on Navier_Stokes_standard equation:
# Order of the PETSc matrix : 180792 (~ 91347 unknowns per PETSc process ) New stencil.
# Norm of the residue: 2.97413e-13 (1)
# 5.95948e-13 1.01269e-12 1.57401e-12 1.41132e-12 1.3106e-12 1.27972e-12 1.16134e-12 1.19008e-12 1.16874e-12 1.08752e-12 1.10956e-12 1.1296e-12 1.04405e-12 9.81617e-13 9.30865e-13
# 9.00661e-13 7.89028e-13 7.10318e-13 6.55742e-13 5.64923e-13 5.1123e-13 4.64233e-13 4.163e-13 3.64017e-13 3.2344e-13 2.99908e-13 2.71321e-13 2.48366e-13 2.30064e-13 2.06451e-13
# 1.88387e-13 1.65358e-13 1.50962e-13 1.44909e-13 1.44848e-13 1.48563e-13 1.51428e-13 1.45143e-13 1.32265e-13 1.19427e-13 1.07073e-13 9.55038e-14 9.0478e-14 8.79144e-14 8.8164e-14
# 8.80405e-14 8.52936e-14 8.20782e-14 7.69269e-14 6.96535e-14 6.14864e-14 5.43284e-14 4.84805e-14 4.28766e-14 3.80256e-14 3.38584e-14 3.05043e-14 3.02244e-14 2.86386e-14 2.84127e-14
# 2.7968e-14 2.76897e-14 2.75798e-14 2.58823e-14 2.54063e-14 2.48844e-14 2.34159e-14 2.15773e-14 1.92805e-14 1.7211e-14 1.53116e-14 1.35037e-14 1.21322e-14 1.10134e-14 1.01993e-14
# 9.51352e-15 9.33784e-15 9.1599e-15 8.97889e-15 8.65861e-15 7.9133e-15 7.42674e-15 7.14131e-15 6.90284e-15 6.52983e-15 6.27059e-15 5.848e-15 5.36843e-15 5.04936e-15 4.60218e-15
# 4.05603e-15 3.66056e-15 3.33357e-15 3.01644e-15 2.74817e-15 2.52933e-15 2.3461e-15 2.1566e-15 2.03352e-15 1.92011e-15 1.78942e-15 1.68356e-15 1.57206e-15 1.45081e-15 1.35633e-15
# 1.28767e-15 1.19226e-15 1.04892e-15 9.52372e-16 8.48944e-16 7.43914e-16 6.59753e-16 5.8355e-16 5.16505e-16 4.57381e-16 4.12041e-16 3.65699e-16 3.206e-16 2.84904e-16
# Final residue: 2.84904e-16 ( 0.000957941 )
# Convergence in 119 iterations for solveur_diffusion_implicite

# Implicited diffusion algorithm applied on Navier_Stokes_standard equation:
# Residu(0)=0.000248197
# Iteration n=1 Residu(n)/Residu(0)=0.802738
# Iteration n=2 Residu(n)/Residu(0)=1.22128
# Iteration n=3 Residu(n)/Residu(0)=1.72741

