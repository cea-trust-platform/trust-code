# Diffusion implicite (residu relatif)

# Implicited diffusion algorithm applied on Navier_Stokes_standard equation:
# Norm of the residue: 1.90398 (1)
# 0.00123019 5.42438e-06 1.42766e-08
# Final residue: 1.42766e-08 ( 7.49829e-09 )

# Implicited diffusion algorithm applied on Navier_Stokes_standard equation:
# Residu(0)=35998.9
# Iteration n=1 Residu(n)/Residu(0)=0.00519422
# Iteration n=2 Residu(n)/Residu(0)=5.23398e-05
# Iteration n=3 Residu(n)/Residu(0)=6.3433e-07
# Diffusion operator implicited for

/Implicited diffusion algorithm/ { ns++;
    getline;
    if (ns == ncomp) {
        if ($1 == "Norm")
        {
            res0=$5
            printf("%d %g\n",0,1) > res;
            getline;
            it=0;
            while ($1 != "Final") {
                for (i=1; i<NF+1; i++) printf("%d %g\n",it+i,$i/res0) > res;
                it=it+NF;
                getline;
            }
        }
        else
        {
            printf("%d %g\n",0,1) > res;
            getline;
            it=1;
            while ($1 != "Diffusion") {
                split($0,a,"=");
                printf("%d %g\n",it,a[3]) > res;
                it=it+1;
                getline;
            }
        }
    }
}

