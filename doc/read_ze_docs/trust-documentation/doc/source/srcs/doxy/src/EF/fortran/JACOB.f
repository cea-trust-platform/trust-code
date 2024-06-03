      SUBROUTINE JACOB(IDIG  ,NBNN ,NBPGAU ,XL ,XG ,DETJ ,AJM1 ,AJ,     
     1                 JSHORT)                                          
      IMPLICIT REAL*8 (A-H,O-Z)                                         
CC                                                                      
CC    CALCUL DU JACOBIEN INVERSE ET DE SON DETERMINANT AUX              
CC           POINTS D'INTEGRATION : GAUSS POUR CUB8, QUA4               
CC                                  HAMMER POUR PRI6, TET4, TRI3        
CC           POUR TET4 ET TRI3 LE JACOBIEN EST CONSTANT                 
CC           ON EST DANS UN ELEMENT                                     
CE    IDIG        : DIMENSION DE L'ESPACE                               
CE    NBNN        : NOMBRE DE NOEUDS PAR ELEMENT                        
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CE    XL(I,J)     : JÉME COORDONNEE DU IÉME NOEUD                       
CE    XG(I,J)     : VALEUR DE LA JÉME COORDONNEE AU IÉME POINT D'INTEG. 
CE    JSHORT      : = FAUX : CALCUL COMPLET                             
CE                  = VRAI : ON NE FAIT PAS AJM1 (PAS DE DERIVEE)       
CS    DETJ(K)     : DETERMINANT AU KÉME POINT D'INTEGRATION             
CS    AJM1(K,I,J) : TERME (I,J) DU JACOBIEN INVERSE AU KÉME POINT D'INT.
CS    AJ(K,I,J)   : TERME (I,J) DU JACOBIEN AU KÉME POINT D'INT.        
C                                                                       
      LOGICAL JSHORT                                                    
C                                                                       
      DIMENSION XL(8,3)                ,XG(NBPGAU,IDIG) ,DETJ(NBPGAU),  
     1          AJM1(NBPGAU,IDIG,IDIG) ,AJ(NBPGAU,IDIG,IDIG)            
C                                                                       
      INTEGER IOIMP
      IOIMP=6 
C                                                                       
C     TERMES DU JACOBIEN                                                
C                                                                       
      IF (NBNN.EQ.8) THEN                                               
C       ELEMENT CUB8                                                    
        DO 20 I=1,IDIG                                                  
          DO 10 K=1,NBPGAU                                              
            AJ(K,1,I) = 0.125 * ( (XG(K,3) - 1.) *                      
     1                  ( (XG(K,2) - 1.) * (XL(2,I) - XL(1,I)) +        
     2                    (XG(K,2) + 1.) * (XL(4,I) - XL(3,I)) ) +      
     3                            (XG(K,3) + 1.) *                      
     4                  ( (XG(K,2) - 1.) * (XL(5,I) - XL(6,I)) +        
     5                    (XG(K,2) + 1.) * (XL(7,I) - XL(8,I)) ) )      
            AJ(K,2,I) = 0.125 * ( (XG(K,3) - 1.) *                      
     1                  ( (XG(K,1) - 1.) * (XL(4,I) - XL(1,I)) +        
     2                    (XG(K,1) + 1.) * (XL(2,I) - XL(3,I)) ) +      
     3                            (XG(K,3) + 1.) *                      
     4                  ( (XG(K,1) - 1.) * (XL(5,I) - XL(8,I)) +        
     5                    (XG(K,1) + 1.) * (XL(7,I) - XL(6,I)) ) )      
            AJ(K,3,I) = 0.125 * ( (XG(K,2) - 1.) *                      
     1                  ( (XG(K,1) - 1.) * (XL(5,I) - XL(1,I)) +        
     2                    (XG(K,1) + 1.) * (XL(2,I) - XL(6,I)) ) +      
     3                            (XG(K,2) + 1.) *                      
     4                  ( (XG(K,1) - 1.) * (XL(4,I) - XL(8,I)) +        
     5                    (XG(K,1) + 1.) * (XL(7,I) - XL(3,I)) ) )      
10          CONTINUE                                                    
20        CONTINUE                                                      
        ELSEIF (NBNN.EQ.6) THEN                                         
C       ELEMENT PRI6                                                    
        DO 40 I=1,IDIG                                                  
          DO 30 K=1,NBPGAU                                              
            AJ(K,1,I) = 0.5 * ( (XG(K,3) - 1.) * (XL(1,I) - XL(2,I)) +  
     1                          (XG(K,3) + 1.) * (XL(5,I) - XL(4,I)) )  
            AJ(K,2,I) = 0.5 * ( (XG(K,3) - 1.) * (XL(1,I) - XL(3,I)) +  
     1                          (XG(K,3) + 1.) * (XL(6,I) - XL(4,I)) )  
            AJ(K,3,I) = 0.5 * ( XG(K,1) * (XL(5,I) - XL(2,I)) +         
     1                          XG(K,2) * (XL(6,I) - XL(3,I)) +         
     2                  (1. - XG(K,1) - XG(K,2)) * (XL(4,I) - XL(1,I)) )
30          CONTINUE                                                    
40        CONTINUE                                                      
        ELSEIF (IDIG.EQ.3.AND.NBNN.EQ.4) THEN                           
C       ELEMENT TET4                                                    
C       BOUCLE SUR LES PTS D'INTEG. BIEN QUE LE JACOBIEN SOIT CONSTANT  
        DO 60 I=1,IDIG                                                  
          DO 50 K=1,NBPGAU                                              
            AJ(K,1,I) = XL(2,I) - XL(1,I)                               
            AJ(K,2,I) = XL(3,I) - XL(1,I)                               
            AJ(K,3,I) = XL(4,I) - XL(1,I)                               
50          CONTINUE                                                    
60        CONTINUE                                                      
        ELSEIF (IDIG.EQ.2.AND.NBNN.EQ.4) THEN                           
C       ELEMENT QUA4                                                    
        DO 80 I=1,IDIG                                                  
          DO 70 K=1,NBPGAU                                              
            AJ(K,1,I) = 0.25 * ( (XG(K,2) - 1.) * (XL(1,I) - XL(2,I)) + 
     1                           (XG(K,2) + 1.) * (XL(3,I) - XL(4,I)) ) 
            AJ(K,2,I) = 0.25 * ( (XG(K,1) - 1.) * (XL(1,I) - XL(4,I)) + 
     1                           (XG(K,1) + 1.) * (XL(3,I) - XL(2,I)) ) 
70          CONTINUE                                                    
80        CONTINUE                                                      
        ELSEIF (NBNN.EQ.3) THEN                                         
C       ELEMENT TRI3                                                    
C       BOUCLE SUR LES PTS D'INTEG. BIEN QUE LE JACOBIEN SOIT CONSTANT  
        DO 100 I=1,IDIG                                                 
          DO 90 K=1,NBPGAU                                              
            AJ(K,1,I) = XL(2,I) - XL(1,I)                               
            AJ(K,2,I) = XL(3,I) - XL(1,I)                               
90          CONTINUE                                                    
100       CONTINUE                                                      
        ELSE                                                            
        WRITE (IOIMP,1000) NBNN                                         
        WRITE (IOIMP,*) '*** ERROR, END OF THE RUN'
        STOP                                                     
        ENDIF                                                           
C                                                                       
C     DETERMINANT ET INVERSE                                            
C                                                                       
      IF (IDIG.EQ.3) THEN                                               
        DO 110 K=1,NBPGAU                                               
          DETJ(K) =                                                     
     1      AJ(K,1,1) *(AJ(K,2,2)*AJ(K,3,3) - AJ(K,2,3)*AJ(K,3,2))      
     2    + AJ(K,2,1) *(AJ(K,3,2)*AJ(K,1,3) - AJ(K,3,3)*AJ(K,1,2))      
     3    + AJ(K,3,1) *(AJ(K,1,2)*AJ(K,2,3) - AJ(K,1,3)*AJ(K,2,2))      
110       CONTINUE                                                      
C                                                                       
        IF (.NOT.JSHORT) THEN                                           
        DO 120 K=1,NBPGAU                                               
          AJM1(K,1,1) = (AJ(K,2,2)*AJ(K,3,3) - AJ(K,3,2)*AJ(K,2,3))     
     1                  /DETJ(K)                                        
          AJM1(K,1,2) = (AJ(K,1,3)*AJ(K,3,2) - AJ(K,1,2)*AJ(K,3,3))     
     1                  /DETJ(K)                                        
          AJM1(K,1,3) = (AJ(K,1,2)*AJ(K,2,3) - AJ(K,1,3)*AJ(K,2,2))     
     1                  /DETJ(K)                                        
          AJM1(K,2,1) = (AJ(K,2,3)*AJ(K,3,1) - AJ(K,2,1)*AJ(K,3,3))     
     1                  /DETJ(K)                                        
          AJM1(K,2,2) = (AJ(K,1,1)*AJ(K,3,3) - AJ(K,1,3)*AJ(K,3,1))     
     1                  /DETJ(K)                                        
          AJM1(K,2,3) = (AJ(K,1,3)*AJ(K,2,1) - AJ(K,1,1)*AJ(K,2,3))     
     1                  /DETJ(K)                                        
          AJM1(K,3,1) = (AJ(K,2,1)*AJ(K,3,2) - AJ(K,2,2)*AJ(K,3,1))     
     1                  /DETJ(K)                                        
          AJM1(K,3,2) = (AJ(K,1,2)*AJ(K,3,1) - AJ(K,1,1)*AJ(K,3,2))     
     1                  /DETJ(K)                                        
          AJM1(K,3,3) = (AJ(K,1,1)*AJ(K,2,2) - AJ(K,1,2)*AJ(K,2,1))     
     1                  /DETJ(K)                                        
120       CONTINUE                                                      
        ENDIF                                                           
        ELSEIF (IDIG.EQ.2) THEN                                         
        DO 130 K=1,NBPGAU                                               
          DETJ(K) =                                                     
     1      AJ(K,1,1) *AJ(K,2,2) - AJ(K,1,2)*AJ(K,2,1)                  
130       CONTINUE                                                      
C                                                                       
        IF (.NOT.JSHORT) THEN                                           
        DO 140 K=1,NBPGAU                                               
          AJM1(K,1,1) =   AJ(K,2,2)/DETJ(K)                             
          AJM1(K,1,2) = - AJ(K,1,2)/DETJ(K)                             
          AJM1(K,2,1) = - AJ(K,2,1)/DETJ(K)                             
          AJM1(K,2,2) =   AJ(K,1,1)/DETJ(K)                             
140       CONTINUE                                                      
        ENDIF                                                           
        ELSE                                                            
        WRITE (IOIMP,1010) IDIG                                         
        WRITE (IOIMP,*) '*** ERROR, END OF THE RUN'
        STOP                                                     
                                                       
        ENDIF                                                           
      DO 150 K=1,NBPGAU                                                 
        DETJ(K) = DABS(DETJ(K))                                         
150     CONTINUE                                                        
C                                                                       
      RETURN                                                            
1000  FORMAT(' *** NOMBRE DE NOEUDS NON PREVU DANS JACOB : ',I6)        
1010  FORMAT(' *** DIMENSION ESPACE NON PREVUE DANS JACOB : ',I6)       
      END                                                               
