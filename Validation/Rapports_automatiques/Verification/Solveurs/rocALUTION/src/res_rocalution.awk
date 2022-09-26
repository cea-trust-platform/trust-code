# rocALUTION solveurs

# IterationControl criteria: abs tol=1.000000000000e-04; rel tol=1.000000000000e-12; div tol=1.000000000000e+03; max iter=64000
# IterationControl initial residual = 1.780728537758e-01
# IterationControl iter=1; residual=2.133450717047e-01
# ...
# IterationControl iter=298; residual=1.074098379437e-04
# IterationControl iter=299; residual=9.397004353275e-05
# IterationControl ABSOLUTE criteria has been reached: res norm=9.397004353275e-05; rel val=5.277056077905e-04; iter=299
# PCG ends
# [rocALUTION] Time to solve: 0.165943
#
/IterationControl/ && /initial/ { ns++;
    res0=$NF
    getline
    if (ns == ncomp) {
        printf("%d %g\n",0,1)
        while ($1=="IterationControl") {
           res=$NF
           gsub("residual=","",res);
           iter=$2
           gsub("iter=","",iter);
           #print res" "res0
           printf("%d %g\n",iter,res/res0)
           getline;
        }
    }
}

