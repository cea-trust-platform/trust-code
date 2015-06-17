      SUBROUTINE VOMABT(IDIM    ,NBNN  ,NBNNMX  ,NBPGAU  ,NBELEM  ,     
     1                  L       ,POIGAU  ,DETJ  ,F       ,DF      ,     
     2                  NUM     ,VOLUME  ,AM    ,BT      ,AINTF   )     
C                                                                       
CC    CALCUL POUR L'ELEMENT L DE :                                      
CC    VOLUME                                                            
CC    MATRICE DE MASSE LUMPEE (ATTENTION IL Y A ASSEMBLAGE)             
CC    MATRICE GRADIENT (DIVERGENCE)                                     
CC    MATRICE INTEGRALE DE FONCTION DE BASE (INTFBASE)                  
CE    IDIM        : DIMENSION DE L'ESPACE                               
CE    NBNN        : NOMBRE DE NOEUDS PAR ELEMENT                        
CE    NBNNMX      : NOMBRE MAXIMAL DE NOEUDS PAR ELEMENT                
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CE    NBELEM      : NOMBRE D'ELEMENT                                    
CE    L           : NUMERO DE L'ELEMENT                                 
CE    POIGAU(K)   : Keme POIDS D'INTEGRATION                            
CE    DETJ(K)     : DETERMINANT AU Keme POINT D'INTEGRATION             
CE    F(I,J)      : VALEUR DE LA Jeme F. DE BASE AU Ieme POINT D'INTEG. 
CE    DF(I,J,K)   : VALEUR DE LA DERIVEE DE LA Jeme F. DE BASE PAR      
CE                  RAPPORT A LA Keme DIRECTION AU Ieme POINT D'INTEG.  
CE                  DANS L'ELEMENT REEL                                 
CE    NUM(I)      : CONNECTIVITE (NOEUD-ELEMENT) POUR CET ELEMENT       
CS    VOLUME      : DE L'ELEMENT                                        
CS    AM(I)       : MATRICE DE MASSE LUMPEE, NOEUD I (GLOBAL)           
CS    BT(I,L,J)   : MATRICE GRADIENT (DIVERGENCE) NOEUD I (LOCAL),      
CS                  ELEMENT L, COMPOSANTE J                             
CS    AINTF(I,L,1): INTFBASE NOEUD I                                    
C                                                                       
      IMPLICIT REAL*8 (A-H,O-Z)                                         
      DIMENSION     POIGAU(NBPGAU)        ,DETJ(NBPGAU) ,F(NBPGAU,NBNN) 
     1             ,DF(NBPGAU,NBNN,IDIM)  ,NUM(*)       ,AM(*)          
     2             ,BT(NBNNMX,NBELEM,IDIM)                              
     3             ,AINTF(NBNNMX,NBELEM,1)                              
C                                                                       
      VOLUME = 0.D0                                                     
      DO 30 I=1,NBNN                                                    
        AINTF(I,L,1) = 0.D0                                             
        DO 10 J=1,IDIM                                                  
          BT(I,L,J) = 0.D0                                              
10      CONTINUE                                                        
30    CONTINUE                                                          
C                                                                       
      DO 100 K=1,NBPGAU                                                 
        DTPG = DETJ(K) * POIGAU(K)                                      
        VOLUME = VOLUME + DTPG                                          
        DO 50 I=1,NBNN                                                  
          AINTF(I,L,1) = AINTF(I,L,1) + F(K,I) * DTPG                   
50      CONTINUE                                                        
        DO 80 J=1,IDIM                                                  
          DO 60 I=1,NBNN                                                
            BT(I,L,J) = BT(I,L,J) + DTPG * DF(K,I,J)                    
60        CONTINUE                                                      
80      CONTINUE                                                        
100   CONTINUE                                                          
C     LUMPED MASS MATRIX = ASSEMBLING INTFBASE                          
      DO 150 I=1,NBNN                                                   
        II = NUM(I)                                                     
        AM(II) = AM(II) + AINTF(I,L,1)                                  
150   CONTINUE                                                          
C                                                                       
      RETURN                                                            
      END                                                               
