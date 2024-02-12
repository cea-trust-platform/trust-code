      SUBROUTINE MFFTA6(C,FAC)
*
*   PURPOSE:
*       ELEMENTARY GENTLEMAN-SANDE RADIX 2 STEP APPLIED TO A VECTOR-OF
*       2-VECTORS-OF-COMPLEX C[IMS,NM [IVS,NV [IES,NE]]].
*       SEE REF.[1] FOR NOTATIONS.
*       THIS ROUTINE CAN BE USED ONLY BY ROUTINE MFFTDM, WHICH CONTROLS
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
 
 
        IF(MX.GT.2*LX)THEN
          DO 100 LAM=0,LLIM,LSTEP
            MUF=LX
            DO 90 MU=LAM+MSTEP,LAM+MD2LIM,MSTEP
              F=CONJG(FAC(MUF))
              DO 80  IV=MU,MU+IVLIM,IVS
              DO 80 I=IV,IV+ILIM,IES
                T0=C(I,0)
                C(I,0)=T0+C(I,1)
                C(I,1)=(T0-C(I,1))*F
80            CONTINUE
              MUF=MUF+LX
90          CONTINUE
            MUF=MUF-LX
            DO 91 MU=LAM+MD2LIM+2*MSTEP,LAM+MLIM,MSTEP
              F=-FAC(MUF)
              DO 81 IV=MU,MU+IVLIM,IVS
              DO 81 I=IV,IV+ILIM,IES
                T0=C(I,0)
                C(I,0)=T0+C(I,1)
                C(I,1)=(T0-C(I,1))*F
81            CONTINUE
              MUF=MUF-LX
91          CONTINUE
            DO 82 IV=MD2LIM+MSTEP+LAM,MD2LIM+MSTEP+LAM+IVLIM,IVS
            DO 82 I=IV,IV+ILIM,IES
               T0=C(I,0)
              C(I,0)=T0+C(I,1)
              C(I,1)=DCMPLX(DIMAG(T0-C(I,1)),-DREAL(T0-C(I,1)))
82          CONTINUE
            DO 83 IV=LAM,LAM+IVLIM,IVS
            DO 83 I=IV,IV+ILIM,IES
                T0=C(I,0)
                C(I,0)=T0+C(I,1)
                C(I,1)=T0-C(I,1)
83          CONTINUE
100       CONTINUE
        ELSE
          IF(MX.EQ.1)GOTO 1000
* IF MX > 1 COME HERE
            MUF=LX
            DO 200 MU=MSTEP,MD2LIM,MSTEP
              F=CONJG(FAC(MUF))
              DO 190 LAM=MU,MU+LLIM,LSTEP
                DO 180 IV=LAM,LAM+IVLIM,IVS
                DO 180 I=IV,IV+ILIM,IES
                  T0=C(I,0)
                  C(I,0)=T0+C(I,1)
                  C(I,1)=(T0-C(I,1))*F
  180           CONTINUE
  190         CONTINUE
              MUF=MUF+LX
200         CONTINUE
            MUF=MUF-LX
            DO 201 MU=MD2LIM+2*MSTEP,MLIM,MSTEP
              F=-FAC(MUF)
              DO 191 LAM=MU,MU+LLIM,LSTEP
                DO 181 IV=LAM,LAM+IVLIM,IVS
                DO 181 I=IV,IV+ILIM,IES
                  T0=C(I,0)
                  C(I,0)=T0+C(I,1)
                  C(I,1)=(T0-C(I,1))*F
  181           CONTINUE
  191         CONTINUE
              MUF=MUF-LX
201         CONTINUE
             DO 192 LAM=MD2LIM+MSTEP,MD2LIM+MSTEP+LLIM,LSTEP
             DO 182 IV=LAM,LAM+IVLIM,IVS
             DO 182 I=IV,IV+ILIM,IES
               T0=C(I,0)
               C(I,0)=T0+C(I,1)
               C(I,1)=DCMPLX(DIMAG(T0-C(I,1)),-DREAL(T0-C(I,1)))
182           CONTINUE
192         CONTINUE
1000        DO 193 LAM=0,LLIM,LSTEP
              DO 183 IV=LAM,LAM+IVLIM,IVS
              DO 183 I=IV,IV+ILIM,IES
                T0=C(I,0)
                C(I,0)=T0+C(I,1)
                C(I,1)=T0-C(I,1)
183           CONTINUE
193         CONTINUE
        ENDIF
      END
