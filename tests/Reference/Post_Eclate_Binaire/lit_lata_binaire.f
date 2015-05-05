       PARAMETER(N=10098,NN=21200)
       REAL*4 X(N)
       INTEGER M(NN)
       PRINT*,N
       PRINT*,NN
       OPEN(UNIT=1,FILE='Post_Eclate_Binaire.lata.dom',
     & FORM='UNFORMATTED')
       READ(1) X
       READ(1) M
       CLOSE(1)
       PRINT*,X(N)
       PRINT*,M(NN)
       PRINT*,'OK.'
       END
