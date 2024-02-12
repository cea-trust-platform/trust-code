      SUBROUTINE MFFTC4(C,FAC)
*
*   PURPOSE:
*       ELEMENTARY GENTLEMAN-SANDE RADIX 5 STEP APPLIED TO A VECTOR-OF
*       VECTORS-OF-COMPLEX C[IVS,NVE[IES,NE]].
*       SEE REF.[1] FOR NOTATIONS.
*       THIS ROUTINE CAN BE USED ONLY BY ROUTINE MFFTDV, WHICH CONTROLS
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
      DOUBLE COMPLEX C(0:NUSTEP-1,0:4),FAC(0:*)
      DOUBLE COMPLEX T1,T2,T3,T4,T5,F1,F2,F3,F4
      REAL *8 SIN72,RAD5D4,S36D72
      PARAMETER (
     $ SIN72 =  9.51056516295153572116439333E-1,
     $ RAD5D4 =  5.59016994374947424102293417E-1,
     $ S36D72 =  6.18033988749894848204586834E-1 )
*..  MU > 1
            MUF=LX
            DO 200 MU=MSTEP,MLIM,MSTEP
              F1=CONJG(FAC(MUF))
              F2=CONJG(FAC(2*MUF))
              F3=CONJG(FAC(3*MUF))
              F4=CONJG(FAC(4*MUF))
              DO 190 LAM=MU,MU+LLIM,LSTEP
                DO 180 I=LAM,LAM+ILIM,IES
                  T1=C(I,1)+C(I,4)
                  T2=C(I,2)+C(I,3)
                  T3=(C(I,1)-C(I,4))*SIN72
                  T4=(C(I,2)-C(I,3))*SIN72
                  T5=T1+T2
                  T1=RAD5D4*(T1-T2)
                  T2=C(I,0)-0.25*T5
                  C(I,0)=C(I,0)+T5
                  T5=T2+T1
                  T2=T2-T1
                  T1=T3+S36D72*T4
                  T3=S36D72*T3-T4
                  C(I,1)=(T5-DCMPLX(-DIMAG(T1),DREAL(T1)))*F1
                  C(I,4)=(T5+DCMPLX(-DIMAG(T1),DREAL(T1)))*F4
                  C(I,2)=(T2-DCMPLX(-DIMAG(T3),DREAL(T3)))*F2
                  C(I,3)=(T2+DCMPLX(-DIMAG(T3),DREAL(T3)))*F3
  180           CONTINUE
  190         CONTINUE
              MUF=MUF+LX
200         CONTINUE
*..  MU=0
1000        DO 193 LAM=0,LLIM,LSTEP
              DO 183 I=LAM,LAM+ILIM,IES
                T1=C(I,1)+C(I,4)
                T2=C(I,2)+C(I,3)
                T3=(C(I,1)-C(I,4))*SIN72
                T4=(C(I,2)-C(I,3))*SIN72
                T5=T1+T2
                T1=RAD5D4*(T1-T2)
                T2=C(I,0)-0.25*T5
                C(I,0)=C(I,0)+T5
                T5=T2+T1
                T2=T2-T1
                T1=T3+S36D72*T4
                T3=S36D72*T3-T4
                C(I,1)=T5-DCMPLX(-DIMAG(T1),DREAL(T1))
                C(I,4)=T5+DCMPLX(-DIMAG(T1),DREAL(T1))
                C(I,2)=T2-DCMPLX(-DIMAG(T3),DREAL(T3))
                C(I,3)=T2+DCMPLX(-DIMAG(T3),DREAL(T3))
183           CONTINUE
193         CONTINUE
      END
