      SUBROUTINE MFFTC8(C,FAC)
*
*   PURPOSE:
*       ELEMENTARY GENTLEMAN-SANDE RADIX 5 STEP APPLIED TO A VECTOR-OF
*       2-VECTORS-OF-COMPLEX C[IMS,NM [IVS,NV [IES,NE]]].
*       SEE REF.[1] FOR NOTATIONS.
*       THIS ROUTINE CAN BE USED ONLY BY ROUTINE MFFTDS, WHICH CONTROLS
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
      DOUBLE COMPLEX T1,T2,T3,T4,T5
      REAL *8 SIN72,RAD5D4,S36D72
      PARAMETER (
     $ SIN72 =  9.51056516295153572116439333E-1,
     $ RAD5D4 =  5.59016994374947424102293417E-1,
     $ S36D72 =  6.18033988749894848204586834E-1 )
      IF(MX.NE.1)THEN
            DO 200 LAM=0,LLIM,LSTEP
              DO 190 IV=LAM,LAM+IVLIM,IVS
                IMUF=0
                IMUF2=NUSTEP
                IMUF3=2*NUSTEP
                IMUF4=3*NUSTEP
                DO 180 IMU=IV,IV+ILIM
                  T1=C(IMU,1)+C(IMU,4)
                  T2=C(IMU,2)+C(IMU,3)
                  T3=(C(IMU,1)-C(IMU,4))*SIN72
                  T4=(C(IMU,2)-C(IMU,3))*SIN72
                  T5=T1+T2
                  T1=RAD5D4*(T1-T2)
                  T2=C(IMU,0)-0.25*T5
                  C(IMU,0)=C(IMU,0)+T5
                  T5=T2+T1
                  T2=T2-T1
                  T1=T3+S36D72*T4
                  T3=S36D72*T3-T4
                  C(IMU,1)=(T5-DCMPLX(-DIMAG(T1),DREAL(T1)))*FAC(IMUF )
                  C(IMU,4)=(T5+DCMPLX(-DIMAG(T1),DREAL(T1)))*FAC(IMUF4)
                  C(IMU,2)=(T2-DCMPLX(-DIMAG(T3),DREAL(T3)))*FAC(IMUF2)
                  C(IMU,3)=(T2+DCMPLX(-DIMAG(T3),DREAL(T3)))*FAC(IMUF3)
                  IMUF=IMUF+1
                  IMUF2=IMUF2+1
                  IMUF3=IMUF3+1
                  IMUF4=IMUF4+1
  180           CONTINUE
  190         CONTINUE
200         CONTINUE
        ELSE
            DO 400 LAM=0,LLIM,LSTEP
              DO 390 IV=LAM,LAM+IVLIM,IVS
                DO 380 IMU=IV,IV+ILIM
                  T1=C(IMU,1)+C(IMU,4)
                  T2=C(IMU,2)+C(IMU,3)
                  T3=(C(IMU,1)-C(IMU,4))*SIN72
                  T4=(C(IMU,2)-C(IMU,3))*SIN72
                  T5=T1+T2
                  T1=RAD5D4*(T1-T2)
                  T2=C(IMU,0)-0.25*T5
                  C(IMU,0)=C(IMU,0)+T5
                  T5=T2+T1
                  T2=T2-T1
                  T1=T3+S36D72*T4
                  T3=S36D72*T3-T4
                  C(IMU,1)=T5-DCMPLX(-DIMAG(T1),DREAL(T1))
                  C(IMU,4)=T5+DCMPLX(-DIMAG(T1),DREAL(T1))
                  C(IMU,2)=T2-DCMPLX(-DIMAG(T3),DREAL(T3))
                  C(IMU,3)=T2+DCMPLX(-DIMAG(T3),DREAL(T3))
380             CONTINUE
390           CONTINUE
400         CONTINUE
        ENDIF
      END
