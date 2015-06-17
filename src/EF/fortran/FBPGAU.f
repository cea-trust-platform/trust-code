      SUBROUTINE FBPGAU(IDIMG  ,NBNN  ,NBPGAU  ,XG  ,FR  ,DFR)          
C                                                                       
CC    CALCUL DES FONCTIONS DE BASE ET DE LEURS DERIVEES AUX POINTS      
CC    DE GAUSS (CUB8, QUA4) OU HAMMER (PRI6, TET4, TRI3)                
CC    ON EST DANS L'ELEMENT DE REFERENCE                                
CE    NBNN        : NOMBRE DE NOEUDS PAR ELEMENT (DONC DE F. DE BASE)   
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CE    XG(I,J)     : VALEUR DE LA JÉME COORDONNEE AU IÉME POINT D'INTEG. 
CS    FR(I,J)     : VALEUR DE LA JÉME F. DE BASE AU IÉME POINT D'INTEG. 
CS    DFR(I,J,K)  : VALEUR DE LA DERIVEE DE LA JÉME F. DE BASE PAR      
CS                  RAPPORT A LA KÉME DIRECTION AU IÉME POINT D'INTEG.  
C                                                                       
      IMPLICIT REAL*8(A-H,O-Z)                                          
      DIMENSION  XG(NBPGAU,IDIMG)       ,FR(NBPGAU,NBNN)                
     1          ,DFR(NBPGAU,NBNN,IDIMG)                                 
      INTEGER IOIMP
      IOIMP=6
C                                                                       
      IF (NBNN.EQ.8) THEN                                               
        DO 10 K=1,NBPGAU                                                
          XM = XG(K,1)-1.                                               
          YM = XG(K,2)-1.                                               
          ZM = XG(K,3)-1.                                               
          XP = XG(K,1)+1.                                               
          YP = XG(K,2)+1.                                               
          ZP = XG(K,3)+1.                                               
C                                                                       
          FR(K,1) = - 0.125 * XM * YM * ZM                              
          FR(K,2) =   0.125 * XP * YM * ZM                              
          FR(K,3) = - 0.125 * XP * YP * ZM                              
          FR(K,4) =   0.125 * XM * YP * ZM                              
          FR(K,5) =   0.125 * XM * YM * ZP                              
          FR(K,6) = - 0.125 * XP * YM * ZP                              
          FR(K,7) =   0.125 * XP * YP * ZP                              
          FR(K,8) = - 0.125 * XM * YP * ZP                              
C                                                                       
          DFR(K,1,1) = - 0.125 * YM * ZM                                
          DFR(K,3,1) = - 0.125 * YP * ZM                                
          DFR(K,5,1) =   0.125 * YM * ZP                                
          DFR(K,7,1) =   0.125 * YP * ZP                                
          DFR(K,2,1) = - DFR(K,1,1)                                     
          DFR(K,4,1) = - DFR(K,3,1)                                     
          DFR(K,6,1) = - DFR(K,5,1)                                     
          DFR(K,8,1) = - DFR(K,7,1)                                     
C                                                                       
          DFR(K,1,2) = - 0.125 * XM * ZM                                
          DFR(K,2,2) =   0.125 * XP * ZM                                
          DFR(K,5,2) =   0.125 * XM * ZP                                
          DFR(K,6,2) = - 0.125 * XP * ZP                                
          DFR(K,3,2) = - DFR(K,2,2)                                     
          DFR(K,4,2) = - DFR(K,1,2)                                     
          DFR(K,7,2) = - DFR(K,6,2)                                     
          DFR(K,8,2) = - DFR(K,5,2)                                     
C                                                                       
          DFR(K,1,3) = - 0.125 * XM * YM                                
          DFR(K,2,3) =   0.125 * XP * YM                                
          DFR(K,3,3) = - 0.125 * XP * YP                                
          DFR(K,4,3) =   0.125 * XM * YP                                
          DFR(K,5,3) = - DFR(K,1,3)                                     
          DFR(K,6,3) = - DFR(K,2,3)                                     
          DFR(K,7,3) = - DFR(K,3,3)                                     
          DFR(K,8,3) = - DFR(K,4,3)                                     
10      CONTINUE                                                        
      ELSEIF (NBNN.EQ.6) THEN                                           
        DO 20 K=1,NBPGAU                                                
          ZM = XG(K,3)-1.                                               
          ZP = XG(K,3)+1.                                               
          X  = XG(K,1)                                                  
          Y  = XG(K,2)                                                  
          XY1 = 1.-X-Y                                                  
          FR(K,1) = - 0.5 * XY1 * ZM                                    
          FR(K,2) = - 0.5 * X   * ZM                                    
          FR(K,3) = - 0.5 * Y   * ZM                                    
          FR(K,4) =   0.5 * XY1 * ZP                                    
          FR(K,5) =   0.5 * X   * ZP                                    
          FR(K,6) =   0.5 * Y   * ZP                                    
C                                                                       
          DFR(K,1,1) =   0.5 * ZM                                       
          DFR(K,4,1) = - 0.5 * ZP                                       
          DFR(K,2,1) = - DFR(K,1,1)                                     
          DFR(K,5,1) = - DFR(K,4,1)                                     
          DFR(K,3,1) =   0.                                             
          DFR(K,6,1) =   0.                                             
C                                                                       
          DFR(K,1,2) =   0.5 * ZM                                       
          DFR(K,4,2) = - 0.5 * ZP                                       
          DFR(K,3,2) = - DFR(K,1,2)                                     
          DFR(K,6,2) = - DFR(K,4,2)                                     
          DFR(K,2,2) =   0.                                             
          DFR(K,5,2) =   0.                                             
C                                                                       
          DFR(K,1,3) = - 0.5 * XY1                                      
          DFR(K,2,3) = - 0.5 * X                                        
          DFR(K,3,3) = - 0.5 * Y                                        
          DFR(K,4,3) = - DFR(K,1,2)                                     
          DFR(K,5,3) = - DFR(K,2,2)                                     
          DFR(K,6,3) = - DFR(K,3,2)                                     
20      CONTINUE                                                        
      ELSEIF (IDIMG.EQ.3.AND.NBNN.EQ.4) THEN                            
        DO 30 K=1,NBPGAU                                                
          X  = XG(K,1)                                                  
          Y  = XG(K,2)                                                  
          Z  = XG(K,3)                                                  
          XYZ1 = 1.-X-Y-Z                                               
          FR(K,1) = XYZ1                                                
          FR(K,2) = X                                                   
          FR(K,3) = Y                                                   
          FR(K,4) = Z                                                   
C                                                                       
          DFR(K,1,1) = - 1.                                             
          DFR(K,2,1) =   1.                                             
          DFR(K,3,1) =   0.                                             
          DFR(K,4,1) =   0.                                             
C                                                                       
          DFR(K,1,2) = - 1.                                             
          DFR(K,2,2) =   0.                                             
          DFR(K,3,2) =   1.                                             
          DFR(K,4,2) =   0.                                             
C                                                                       
          DFR(K,1,3) = - 1.                                             
          DFR(K,2,3) =   0.                                             
          DFR(K,3,3) =   0.                                             
          DFR(K,4,3) =   1.                                             
30      CONTINUE                                                        
      ELSEIF (IDIMG.EQ.2.AND.NBNN.EQ.4) THEN                            
        DO 40 K=1,NBPGAU                                                
          XM = XG(K,1)-1.                                               
          YM = XG(K,2)-1.                                               
          XP = XG(K,1)+1.                                               
          YP = XG(K,2)+1.                                               
C                                                                       
          FR(K,1) =   0.25 * XM * YM                                    
          FR(K,2) = - 0.25 * XP * YM                                    
          FR(K,3) =   0.25 * XP * YP                                    
          FR(K,4) = - 0.25 * XM * YP                                    
C                                                                       
          DFR(K,1,1) =   0.25 * YM                                      
          DFR(K,3,1) = - 0.25 * YP                                      
          DFR(K,2,1) = - DFR(K,1,1)                                     
          DFR(K,4,1) = - DFR(K,3,1)                                     
C                                                                       
          DFR(K,1,2) =   0.25 * XM                                      
          DFR(K,2,2) = - 0.25 * XP                                      
          DFR(K,3,2) = - DFR(K,2,2)                                     
          DFR(K,4,2) = - DFR(K,1,2)                                     
40      CONTINUE                                                        
      ELSEIF (NBNN.EQ.3) THEN                                           
        DO 50 K=1,NBPGAU                                                
          XY1 = 1.-XG(K,1)-XG(K,2)                                      
          X  = XG(K,1)                                                  
          Y  = XG(K,2)                                                  
          FR(K,1) =  XY1                                                
          FR(K,2) =  X                                                  
          FR(K,3) =  Y                                                  
C                                                                       
          DFR(K,1,1) = - 1.                                             
          DFR(K,2,1) =   1.                                             
          DFR(K,3,1) =   0.                                             
C                                                                       
          DFR(K,1,2) = - 1.                                             
          DFR(K,3,2) =   0.                                             
          DFR(K,2,2) =   1.                                             
50      CONTINUE                                                        
      ELSE                                                              
        WRITE (IOIMP,1000) NBNN                        
        WRITE (IOIMP,*) '*** ERROR, END OF THE RUN'
        STOP                                                     
 
      ENDIF                                                             
C                                                                       
      RETURN                                                            
1000  FORMAT(' *** NOMBRE DE NOEUDS NON PREVU DANS FBPGAU : ',I6)       
      END                                                               
