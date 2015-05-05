      SUBROUTINE MFFTA7(C,FAC)
*
*   PURPOSE:
*       ELEMENTARY COOLEY-TUKEY RADIX 2 STEP APPLIED TO A VECTOR-OF
*       2-VECTORS-OF-COMPLEX C[IMS,NM [IVS,NV [IES,NE]]].
*       SEE REF.[1] FOR NOTATIONS.
*       THIS ROUTINE CAN BE USED ONLY BY ROUTINE MFFTIM, WHICH CONTROLS
*       ITS OPERATION THROUGH THE MFFTPA COMMON
*
*   DUMMY ARGUMENTS :
*
*   C   ARRAY BEING FOURIER  TRANSFORMED
*   FAC PHASE FACTORS, PREPARED BY MFFTP; NOT MODIFIED IN OUTPUT
*
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON /MFFTPA/  IMS,IVS,IES,NM,NV,NE,MX,LX,MLIM,MSTEP,LLIM,LSTEP,
     $ NUSTEP,IVLIM,ILIM,MD2LIM,LD2LIM
      INTEGER NUSTEP
      DOUBLE COMPLEX C(0:NUSTEP-1,0:1)
      COMPLEX FAC(0:*)
      DOUBLE COMPLEX T0,F
 
        IF(2*MX.GT.LX)THEN
          IF(LX.EQ.1)GOTO 1000
*   ELSE HERE
          LAMF=MX
          DO 100 LAM=LSTEP,LD2LIM,LSTEP
            F=FAC(LAMF)
            DO 90 MU=LAM,LAM+MLIM,MSTEP
              DO 80 IV=MU,MU+IVLIM,IVS
              DO 80 I=IV,IV+ILIM,IES
                T0=C(I,1)*F
                C(I,1)=C(I,0)-T0
                C(I,0)=C(I,0)+T0
80            CONTINUE
90          CONTINUE
            LAMF=LAMF+MX
100       CONTINUE
          LAMF=LAMF-MX
          DO 101 LAM=LD2LIM+2*LSTEP,LLIM,LSTEP
            F=-CONJG(FAC(LAMF))
            DO 91 MU=LAM,LAM+MLIM,MSTEP
              DO 81 IV=MU,MU+IVLIM,IVS
              DO 81 I=IV,IV+ILIM,IES
                T0=C(I,1)*F
                C(I,1)=C(I,0)-T0
                C(I,0)=C(I,0)+T0
81            CONTINUE
91          CONTINUE
            LAMF=LAMF-MX
101       CONTINUE
          DO 93 MU=LD2LIM+LSTEP,LD2LIM+LSTEP+MLIM,MSTEP
 
            DO 83 IV=MU,MU+IVLIM,IVS
            DO 83 I=IV,IV+ILIM,IES
              T0=DCMPLX(-DIMAG(C(I,1)),DREAL(C(I,1)))
              C(I,1)=C(I,0)-T0
              C(I,0)=C(I,0)+T0
83          CONTINUE
93        CONTINUE
1000      DO 92 MU=0,MLIM,MSTEP
 
            DO 82 IV=MU,MU+IVLIM,IVS
            DO 82 I=IV,IV+ILIM,IES
              T0=C(I,1)
              C(I,1)=C(I,0)-T0
              C(I,0)=C(I,0)+T0
82          CONTINUE
92        CONTINUE
        ELSE
          DO 200 MU=0,MLIM,MSTEP
            LAMF=MX
            DO 190 LAM=MU+LSTEP,MU+LD2LIM,LSTEP
              F=FAC(LAMF)
              DO 180 IV=LAM,LAM+IVLIM,IVS
              DO 180 I=IV,IV+ILIM,IES
                T0=C(I,1)*F
                C(I,1)=C(I,0)-T0
                C(I,0)=C(I,0)+T0
180           CONTINUE
              LAMF=LAMF+MX
190         CONTINUE
            LAMF=LAMF-MX
            DO 191 LAM=MU+LD2LIM+2*LSTEP,MU+LLIM,LSTEP
              F=-CONJG(FAC(LAMF))
              DO 181 IV=LAM,LAM+IVLIM,IVS
              DO 181 I=IV,IV+ILIM,IES
                T0=C(I,1)*F
                C(I,1)=C(I,0)-T0
                C(I,0)=C(I,0)+T0
181           CONTINUE
              LAMF=LAMF-MX
191         CONTINUE
            DO 182 IV=MU,MU+IVLIM,IVS
            DO 182 I=IV,IV+ILIM,IES
              T0=C(I,1)
              C(I,1)=C(I,0)-T0
              C(I,0)=C(I,0)+T0
182         CONTINUE
            DO 183 IV=MU+LD2LIM+LSTEP,MU+LD2LIM+LSTEP+IVLIM,IVS
            DO 183 I=IV,IV+ILIM,IES
              T0=DCMPLX(-DIMAG(C(I,1)),DREAL(C(I,1)))
              C(I,1)=C(I,0)-T0
              C(I,0)=C(I,0)+T0
183         CONTINUE
200       CONTINUE
        ENDIF
      END
