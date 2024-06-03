      SUBROUTINE LNFIXX(VALEM , F , DETJ , POIGAU , NBNN , NBPGAU ,     
     1                  ID2   , LCHAMP , CHPL , TL)                     
      IMPLICIT REAL*8 (A-H,O-Z)                                         
CC                                                                      
CC    APPELE PAR ELNOFI POUR CHAQUE ELEMENT                             
CC    APPELE PAR ELNOFIFA POUR CHAQUE FACE                              
CC                                                                      
CE    VALEM                                                             
CE    F                                                                 
CE    DETJ                                                              
CE    POIGAU                                                            
CE    NBNN                                                              
CE    NBPGAU                                                            
CE    ID2                                                               
CE    LCHAMP                                                            
CE    CHPL                                                              
CS    TL                                                                
C                                                                       
      LOGICAL LCHAMP                                                    
C                                                                       
      DIMENSION DTPG(27)     , TL(8,3)         , F(NBPGAU,NBNN) ,       
     1          DETJ(NBPGAU) , POIGAU(NBPGAU)  , VALEM(1)       ,       
     2          T(8)         , CHPL(8)                                  
C                                                                       
      DO 10 K=1,NBPGAU                                                  
        DTPG(K) = DETJ(K)*POIGAU(K)                                     
10      CONTINUE                                                        
      IF (LCHAMP) THEN                                                  
        DO 25 K=1,NBPGAU                                                
          AX = 0.D0                                                     
          DO 20 I=1,NBNN                                                
            AX = AX + CHPL(I)*F(K,I)                                    
20          CONTINUE                                                    
          DTPG(K) = DTPG(K)*AX                                          
25        CONTINUE                                                      
        ENDIF                                                           
C                                                                       
      DO 30 I=1,NBNN                                                    
        T(I) = 0.D0                                                     
30      CONTINUE                                                        
C                                                                       
      DO 70 K=1,NBPGAU                                                  
        DO 50 I=1,NBNN                                                  
          T(I) = T(I) + DTPG(K)*F(K,I)                                  
50        CONTINUE                                                      
70      CONTINUE                                                        
C                                                                       
      DO 110 J=1,ID2                                                    
        DO 90 I=1,NBNN                                                  
          TL(I,J) = T(I)*VALEM(J)                                       
90        CONTINUE                                                      
110     CONTINUE                                                        
C                                                                       
      RETURN                                                            
      END                                                               
