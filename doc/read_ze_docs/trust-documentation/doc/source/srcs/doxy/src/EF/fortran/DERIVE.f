      SUBROUTINE DERIVE(IDIM  ,NBNN  ,NBPGAU  ,AJM1  ,DFR  ,DF)         
C                                                                       
CC    CALCUL DES DERIVEES DES FONCTIONS DE BASE DANS LA GEOMETRIE       
CC    REELLE                                                            
CE    IDIM        : DIMENSION DE L'ESPACE                               
CE    NBNN        : NOMBRE DE NOEUDS PAR ELEMENT                        
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CE    AJM1(K,I,J) : TERME (I,J) DU JACOBIEN INVERSE AU KÉME POINT D'INT.
CE    DFR(I,J,K)  : VALEUR DE LA DERIVEE DE LA JÉME F. DE BASE PAR      
CE                  RAPPORT A LA KÉME DIRECTION AU IÉME POINT D'INTEG.  
CE                  DANS L'ELEMENT DE REFERENCE                         
CS    DF(I,J,K)   : VALEUR DE LA DERIVEE DE LA JÉME F. DE BASE PAR      
CS                  RAPPORT A LA KÉME DIRECTION AU IÉME POINT D'INTEG.  
CS                  DANS L'ELEMENT REEL                                 
C                                                                       
      IMPLICIT REAL*8 (A-H,O-Z)                                         
      DIMENSION     AJM1(NBPGAU,IDIM,IDIM)  ,DFR(NBPGAU,NBNN,IDIM)      
     1             ,DF(NBPGAU,NBNN,IDIM)                                
C                                                                       
      DO 40 IA=1,IDIM                                                   
        DO 30 K=1,NBPGAU                                                
         DO 20 I=1,NBNN                                                 
           DFIA = 0.                                                    
           DO 10 IB=1,IDIM                                              
             DFIA = DFIA + DFR(K,I,IB) * AJM1(K,IA,IB)                  
10           CONTINUE                                                   
           DF(K,I,IA) = DFIA                                            
20         CONTINUE                                                     
30       CONTINUE                                                       
40     CONTINUE                                                         
C                                                                       
      RETURN                                                            
      END                                                               
