      SUBROUTINE CALFGR(IDIMG  , NBNN   , NBNNMX , NBPGAU , NBELEG ,    
     1                  L      , POIGAU , DETJ   , F      , DF     ,    
     2                  NUMG   , CHP    , VOLUME , AM     , BT     ,    
     3                  AINTF  )                                        
      IMPLICIT REAL*8 (A-H,O-Z)                                         
CC                                                                      
CC    CALCUL VOLUME, MASSE LUMPEE, GRADIENT, INTFBASE POREUX            
CC                                                                      
CE    IDIMG       : DIMENSION DE L'ESPACE                               
CE    NBNN        : NOMBRE DE NOEUDS PAR ELEMENT                        
CE    NBNNMX      : NOMBRE MAXIMAL DE NOEUDS PAR ELEMENT                
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CE    NBELEG      : NOMBRE D'ELEMENT                                    
CE    L           : NUMERO DE L'ELEMENT                                 
CE    POIGAU(K)   : Keme POIDS D'INTEGRATION                            
CE    DETJ(K)     : DETERMINANT AU Keme POINT D'INTEGRATION             
CE    F(I,J)      : VALEUR DE LA Jeme F. DE BASE AU Ieme POINT D'INTEG. 
CE    DF(I,J,K)   : VALEUR DE LA DERIVEE DE LA Jeme F. DE BASE PAR      
CE                  RAPPORT A LA Keme DIRECTION AU Ieme POINT D'INTEG.  
CE                  DANS L'ELEMENT REEL                                 
CE    NUMG        : CONNECTIVITE (NOEUD-ELEMENT) POUR CET ELEMENT       
CE    CHP         : CHPOINGE                                            
CS    VOLUME      : VOLUME AVEC CHP                                     
CS    AM          : MASSE AVEC CHP (LUMPEE) ; = 4 : 1 + 2               
CS    BT(I,L,J)   : FI * GRADIENT CHPOINGE + CHPOINGE * GRADIENT FI     
CS    AINTF(I,L,1): FI * CHPOINGE                                       
C                                                                       
      DIMENSION    POIGAU(NBPGAU)         ,DETJ(NBPGAU)  ,F(NBPGAU,NBNN)
     1            ,DF(NBPGAU,NBNN,IDIMG)  ,NUMG(*)       ,CHP(*)        
     2            ,BT(NBNNMX,NBELEG,IDIMG),AM(*)                        
     3            ,AINTF(NBNNMX,NBELEG,1) ,CHPL(8)                      
C                                                                       
      VOLUME = 0.D0                                                     
      DO 30 I=1,NBNN                                                    
        CHPL(I) = CHP(NUMG(I))                                          
        AINTF(I,L,1) = 0.D0                                             
        DO 10 J=1,IDIMG                                                 
          BT(I,L,J) = 0.D0                                              
10      CONTINUE                                                        
30    CONTINUE                                                          
C                                                                       
      DO 200 K=1,NBPGAU                                                 
        DTPG = DETJ(K) * POIGAU(K)                                      
        AX = 0.D0                                                       
        DO 50 J=1,NBNN                                                  
          AX = AX + F(K,J) * CHPL(J)                                    
50      CONTINUE                                                        
        DTPGAX = DTPG*AX                                                
        VOLUME = VOLUME + DTPGAX                                        
        DO 100 I=1,NBNN                                                 
          AINTF(I,L,1) = AINTF(I,L,1) + F(K,I) * DTPGAX                 
100     CONTINUE                                                        
        DO 180 IA=1,IDIMG                                               
          AXAIA = 0.D0                                                  
          DO 130 J=1,NBNN                                               
            AXAIA = AXAIA + CHPL(J) * DF(K,J,IA)                        
130       CONTINUE                                                      
          AXAIA = AXAIA * DTPG                                          
          DO 160 I=1,NBNN                                               
            BT(I,L,IA) = BT(I,L,IA) + F(K,I) * AXAIA +                  
     1                                DF(K,I,IA) * DTPGAX               
160       CONTINUE                                                      
180     CONTINUE                                                        
200   CONTINUE                                                          
C     LUMPED MASS MATRIX = ASSEMBLING INTFBASE                          
      DO 250 I=1,NBNN                                                   
        II = NUMG(I)                                                    
        AM(II) = AM(II) + AINTF(I,L,1)                                  
250   CONTINUE                                                          
C                                                                       
      RETURN                                                            
      END                                                               
