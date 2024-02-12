        SUBROUTINE R4FFT(C,ID1,ID2,N1,N2,N3,N4,W1,W2,W3,W4,ISIG,IORD,
     &                   IWORK,IERR)
C======================================================================*
C R4FFT -- 4 Dimensional Real Fast Fourier Transformation
C
C PURPOSE:
C       THIS ROUTINE PERFORMS A 4-DIMENSIONAL REAL FOURIER TRANSFORM
C       OF ORDER N1*N2*N3*N4
C
C USAGE:
C       THE USER IS EXPECTED TO PROVIDE THE DATA IN A 4-DIMENSIONAL
C       REAL ARRAY C, DIMENSIONED IN THE CALLING PROGRAM AS
C
C          REAL C(ID1,ID2,N3,N4)
C
C       THE RESULTS ARE RETURNED IN THE SAME STORAGE SPACE AS A
C       COMPLEX ARRAY CCOM
C
C         COMPLEX CCOM(ID1/2,ID2,N3,N4)
C
C       ID1 MUST BE AN EVEN INTEGER EQUAL TO N1+2; FOR OPTIMAL PERFORMANCE,
C       ID1/2  AND ID2 SHOULD BE ODD, AND ID2 SHOULD BE EQUAL TO N2 OR
C       N2+1, DEPENDING IF N2 IS ODD OR EVEN.
C
C       THE ROUTINE IS INTENDED FOR REPEATED USAGE, THUS
C       SEPARATE SET-UP AND OPERATING CALLS ARE AVAILABLE: THE USER SHOULD
C       ALLWAYS PERFORM A SET-UP CALL ( ISIG=0 ) PASSING THE
C       CORRECT VALUES OF THE ARGUMENTS, BEFORE PERFORMING THE
C       ACTUAL TRANSFORM ( ISIG= +1 OR -1 ); THE USER CAN CHOOSE
C       WHETHER TO OBTAIN THE RESULT OF A DIRECT TRANSFORM IN
C       NATURAL ORDER (ISIG=-1,IORD=1) OR IN "BIT-REVERSED" ORDER
C       (ISIG=-1,IORD=0); THIS CHOICE SAVES SOME COMPUTER TIME
C       IN CASES DISCUSSED IN THE LONG WRITE-UP. ANALOGOUSLY, THE
C       INVERSE TRANSFORM ACCEPTS INPUT IN NATURAL ORDER (ISIG=1,
C       IORD=1) OR IN "BIT-REVERSED" ORDER ( ISIG=1,IORD=-1).
C       THIS OPTION DOES NOT APPLY TO THE FIRST DIMENSION, THAT
C       IS REORDERED IN ANY CASE.
C
C ARGUMENTS:
C       C:
C          DECLARED REAL C(ID1,ID2,N3,N4) IN THE CALLING PROGRAM
C          INPUT: ARRAY TO BE TRANSFORMED;
C          OUTPUT: TRANSFORMED ARRAY IF ISIG.NE.0; UNCHANGED IF ISIG.EQ.0
C       ID1:
C          INTEGER;
C          INPUT: FIRST DIMENSION OF C, AS DECLARED IN THE CALLING PROGRAM
C       ID2:
C          INTEGER;
C          INPUT: SECOND DIMENSION OF C,AS DECLARED IN THE CALLING PROGRAM
C       N1:
C          INTEGER;
C          INPUT: ORDER OF THE TRANSFORM ALONG THE FIRST DIMENSION;
C                 N1.EQ.ID1-2 ;N1 MUST BE A PRODUCT OF POWERS OF 2,3,5
C               AND MUST BE EVEN;
C       N2:
C          INTEGER;
C          INPUT: ORDER OF THE TRANSFORM ALONG THE SECOND DIMENSION; N2.LE.
C                 N2 MUST BE A PRODUCT OF POWERS OF 2,3,5;
C       N3:
C          INTEGER;
C          INPUT: THIRD DIMENSION OF C, AS DECLARED IN THE CALLING PROGRAM,
C                 AND ORDER OF THE TRANSFORM ALONG THE THIRD DIMENSION
C                 N3 MUST BE A PRODUCT OF POWERS OF 2,3,5;
C       N4:
C         INTEGER;
C         INPUT: ORDER OF THE TRANSFORM ALONG THE FOURTH DIMENSION
C                 N1 MUST BE A PRODUCT OF POWERS OF 2,3,5;
C       W1,W2,W3,W4:
C         THEY MUST BE DECLARED IN THE CALLING PROGRAM AS
C         INTEGER W1(6*N1+14),W2(4*N2+14),W3(4*N3+14),W4(4*N4+14)
C         IF ANY OF THE N'S ARE EQUAL, THE CORRESPONDING W'S
C         DON'T NEED TO BE DISTINCT.
C         INPUT: IF ISIG.NE.0, THE W'S MUST CONTAIN THE TABLES PREPARED
C                BY A PREVIOUS CALL WITH THE SAME ARGUMENTS BUT ISIG.EQ.0.
C         OUTPUT: IF ISIG.EQ.0, THE W ARE FILLED WITH THE TABLES REQUIRED
C                 FOR THE TRANSFORM. IF ISIG.NE.0, UNCHANGED.
C       IORD:
C         INTEGER;
C         INPUT: IF IORD.EQ.0, NO REORDERING IS PERFORMED AFTER A DIRECT
C                TRANSFORM, SO THAT THE RESULTS ARE LEFT IN "BIT REVERSED"
C                ORDER; AND NO PERMUTATION IS PERFORMED BEFORE THE INVERSE
C                TRANSFORM, SO THAT THE INPUT MATRIX IS REQUIRED TO BE
C                ALREADY IN"BIT-REVERSED" ORDER; IF IORD.NE.0, OUTPUT OF
C                DIRECT TRANSFORM AND INPUT TO INVERSE TRANSFORM ARE IN
C                NATURAL ORDER; THE ABOVE OPTION APPLIES ONLY TO THE SECOND
C                THIRD AND FOURTH DIMENSION, SINCE THE FIRST ONE IS REORDER
C              IN ANY CASE
C       ISIG:
C         INTEGER;
C         INPUT: IF ISIG.LT.0  , DIRECT TRANSFORM ( C(J)EXP(-2*PI*I*J*K/N))
C                IF ISIG.GT.0  , INVERSE TRANSFORM ( C(J)EXP(2*PI*I*J*K/N))
C                IF ISIG.EQ.0  , SETUP RUN ( FILLING OF THE W'S )
C       IWORK:
C         INTEGER ARRAY, OF SIZE MAX(N1,N2,N3,N4)
C         WORK AREA;
C       IERR:
C         INTEGER:
C         OUTPUT: ERROR CODE:
C                 0 :NO ERRORS;
C                 1 :ID1.LT.N1 OR ID2.LT.N2;
C                 2 :ONE OF THE N'S CONTAINS A FACTOR DIFFERENT FROM 2,3,5
C                 3 :IN A CALL WITH ISIG.NE.0, SOME OF THE W'S CONTAINED
C                      ILLEGAL VALUES
C                 4 :N1 IS ODD
C
C For further documentation see A. Nobile and V. Roberto, 
C "MFFT: A Package for Two and Three Dimensional Vectorized
C Discrete Fourier Transforms", Computer Physics Communications vol. 42,
C pp 233-247 (1986), and "Efficient Implementation of Multidimensional
C Fast Fourier Transformation on a Cray X-MP", Computer Physics
C Communications, vol. 40, pp 189-201 (1986).
C======================================================================*
        COMPLEX C(0:*)
        INTEGER W1(-14:*),W2(-14:*),W3(-14:*),W4(-14:*)
        INTEGER IWORK(*)
        INTEGER IDERR,FACERR,TBERR
        PARAMETER(IDERR=1,FACERR=2,TBERR=3,ODDERR=4)

C BEGIN:
 
        IF(ID1.NE.N1+2.OR.ID2.LT.N2)THEN
                IERR=IDERR
                RETURN
        ENDIF
        N12=N1/2
        IF(2*N12.NE.N1)THEN
                IERR=ODDERR
                RETURN
        ENDIF
 
        IERR=0
        IF(ISIG.EQ.0)THEN
 
                CALL MFFTRP(N1,W1(4*N1))
                CALL MFFTP(N12,W1,0,IERR)
                IF(IERR.NE.0)RETURN
                IF(N2.EQ.N12)THEN
                        CALL MFFTZ0(W1,1,4*N12+14,W2,1)
                ELSE
                        CALL MFFTP(N2,W2,0,IERR)
                        IF(IERR.NE.0)RETURN
                ENDIF
                IF(N3.EQ.N12)THEN
                        CALL MFFTZ0(W1,1,4*N12+14,W3,1)
                ELSE IF(N3.EQ.N2)THEN
                        CALL MFFTZ0(W2,1,4*N2+14,W3,1)
                ELSE
                        CALL MFFTP(N3,W3,0,IERR)
                        IF(IERR.NE.0)RETURN
                ENDIF
                IF(N4.EQ.N12)THEN
                        CALL MFFTZ0(W1,1,4*N12+14,W4,1)
                ELSE IF(N4.EQ.N2)THEN
                        CALL MFFTZ0(W2,1,4*N2+14,W4,1)
                ELSE IF (N4.EQ.N3)THEN
                        CALL MFFTZ0(W3,1,4*N3+14,W4,1)
                ELSE
                        CALL MFFTP(N4,W4,0,IERR)
                        IF(IERR.NE.0)RETURN
                ENDIF
                RETURN
 
        ELSE  IF(ISIG.GT.0)THEN
 
                IF(IORD.NE.0)THEN
                        CALL MFFTOM(C,ID1/2,1,ID1/2*ID2,N2,N12+1,N3*N4,
     &                              W2(3*N2) ,IWORK)
                        CALL MFFTOM(C,ID1/2*ID2,ID1/2*ID2*N3,1,N3,N4,
     &                              ID1/2*N2,W3(3*N3),IWORK)
                        CALL MFFTOV(C,ID1/2*ID2*N3,1,N4,ID1/2*ID2*N3,
     &                              W4(3*N4),IWORK)
                ENDIF
 
                IF(IERR.NE.0)RETURN
                CALL MFFTIM(C,ID1/2,1,ID1/2*ID2,N2,N12+1,N3*N4,W2,IERR)
                IF(IERR.NE.0)RETURN
                CALL MFFTIM(C,ID1/2*ID2,ID1/2*ID2*N3,1,N3,N4,ID1/2*N2,W3
     &                      ,IERR)
                IF(IERR.NE.0)RETURN
                CALL MFFTIV(C,ID1/2*ID2*N3,1,N4,ID1/2*ID2*N3,W4,IERR)
 
                CALL MFFTRI(C,1,ID1/2,N12,ID2*N3*N4,W1(4*N1))
                CALL MFFTOV(C,1,ID1/2,N12,ID2*N3*N4,W1(3*N12),IWORK)
                CALL MFFTIV(C,1,ID1/2,N12,ID2*N3*N4,W1,IERR)
        ELSE
 
                CALL MFFTDV(C,1,ID1/2,N12,ID2*N3*N4,W1,IERR)
                CALL MFFTOV(C,1,ID1/2,N12,ID2*N3*N4,W1(2*N12),IWORK)
                CALL MFFTRD(C,1,ID1/2,N12,ID2*N3*N4,W1(4*N1))
 
                IF(IERR.NE.0)RETURN
                CALL MFFTDM(C,ID1/2,1,ID1/2*ID2,N2,N12+1,N3*N4,W2,IERR)
                IF(IERR.NE.0)RETURN
                CALL MFFTDM(C,ID1/2*ID2,ID1/2*ID2*N3,1,N3,N4,ID1/2*N2,W3
     &                      ,IERR)
                IF(IERR.NE.0)RETURN
                CALL MFFTDV(C,ID1/2*ID2*N3,1,N4,ID1/2*ID2*N3,W4,IERR)
 
                IF(IORD.NE.0)THEN
                        CALL MFFTOM(C,ID1/2,1,ID1/2*ID2,N2,N12+1,N3*N4,
     &                             W2(2*N2),IWORK)
                        CALL MFFTOM(C,ID1/2*ID2,ID1/2*ID2*N3,1,N3,N4,
     &                              ID1/2*N2,W3(2*N3),IWORK)
                        CALL MFFTOV(C,ID1/2*ID2*N3,1,N4,ID1/2*ID2*N3,
     &                              W4(2*N4),IWORK)
                ENDIF
        ENDIF
        END
