      SUBROUTINE CHPGAU(CELE  ,NBPGAU,POIGAU  ,XG)                      
C                                                                       
CC    POINTS D'INTEGRATION (3 D)                                        
CC                                                                      
CC          **** VERIFIER SI LES POIDS SONT OK (SUR VOLUME) *****       
CC                                                                      
CC                 ****** VERIFIER LE PRI6 ********                     
CC                                                                      
CE    CELE        : TYPE ELEMENT                                        
CE    NBPGAU      : NOMBRE DE POINTS D'INTEGRATION                      
CS    POIGAU      : POIDS                                               
CS    XG(I,J)     : VALEUR DE LA JÉME COORDONNEE AU IÉME POINT D'INTEG. 
C                                                                       
      IMPLICIT REAL*8(A-H,O-Z)                                          
      CHARACTER*4  CELE                                                 
      DIMENSION    POIGAU(NBPGAU)  ,XG(NBPGAU,3)                        
C                                                                       
      IF (CELE.EQ.'TET4') THEN                                          
        IF (NBPGAU.EQ.1) THEN                                           
C                                                                       
C         ELEMENT TETRAEDRE 4 NOEUDS * INTEGRATION A 1 POINT            
C         EXACT POUR LES POLYNOMES DE DEGRE 1                           
C         DAUTRAY-LIONS TOME 2 PAGE 779                                 
C         CASTEM2000 RAPPORT DEMT 85/400 & ZIENKIEWICZ PAGE 221         
C                                                                       
          POIGAU(1) = 1.D0/6.D0                                         
          XG(1,1) = .25D0                                               
          XG(1,2) = .25D0                                               
          XG(1,3) = .25D0                                               
          ELSEIF (NBPGAU.EQ.4) THEN                                     
C                                                                       
C         ELEMENT TETRAEDRE 4 NOEUDS * INTEGRATION A 4 POINTS           
C         EXACT POUR LES POLYNOMES DE DEGRE 2                           
C         DAUTRAY-LIONS TOME 2 PAGE 779                                 
C         CASTEM2000 RAPPORT DEMT 85/400 & ZIENKIEWICZ PAGE 221         
C                                                                       
          P = 1.D0/24.D0                                                
          DO 20 I=1,4                                                   
            POIGAU(I) = P                                               
20          CONTINUE                                                    
          X138 = 0.1381966011250105D0                                   
          X585 = 0.5854101966249684D0                                   
          DO 30 I=1,4                                                   
            DO 30 J=1,3                                                 
              XG(I,J) = X138                                            
30            CONTINUE                                                  
          XG(2,1) = X585                                                
          XG(3,2) = X585                                                
          XG(4,3) = X585                                                
          ELSEIF (NBPGAU.EQ.5) THEN                                     
C                                                                       
C         ELEMENT TETRAEDRE 4 NOEUDS * INTEGRATION A 5 POINTS           
C         EXACT POUR LES POLYNOMES DE DEGRE 3                           
C         CASTEM2000 RAPPORT DEMT 85/400 & ZIENKIEWICZ PAGE 221         
C                                                                       
          P = .45D0/6.D0                                                
          DO 40 I=2,5                                                   
            POIGAU(I) = P                                               
40          CONTINUE                                                    
          POIGAU(1) = -.8D0/6.D0                                        
          XG(1,1) = .25D0                                               
          XG(1,2) = .25D0                                               
          XG(1,3) = .25D0                                               
          A = 1.D0/6.D0                                                 
          DO 50 I=2,5                                                   
            DO 50 J=1,3                                                 
              XG(I,J) = A                                               
50            CONTINUE                                                  
          XG(3,3) = .5D0                                                
          XG(4,2) = .5D0                                                
          XG(5,1) = .5D0                                                
          ENDIF                                                         
        ELSEIF (CELE.EQ.'PRI6') THEN                                    
        IF (NBPGAU.EQ.1) THEN                                           
C                                                                       
C         ELEMENT PRISME 6 NOEUDS * INTEGRATION A 1 POINT               
C         A VERIFIER                                                    
C                                                                       
          POIGAU(1) = 1.D0                                              
          A = 1.D0/3.D0                                                 
          XG(1,1) = A                                                   
          XG(1,2) = A                                                   
          XG(1,3) = 0.D0                                                
          ELSEIF (NBPGAU.EQ.2) THEN                                     
C                                                                       
C         ELEMENT PRISME 6 NOEUDS * INTEGRATION A 2 POINTS              
C         CASTEM2000 RAPPORT DEMT 85/400                                
C                                                                       
          A = 1.D0/3.D0                                                 
          B = DSQRT(A)                                                  
          POIGAU(1) = .5D0                                              
          POIGAU(2) = .5D0                                              
          XG(1,1) = A                                                   
          XG(1,2) = A                                                   
          XG(2,1) = A                                                   
          XG(2,2) = A                                                   
          XG(1,3) = -B                                                  
          XG(2,3) =  B                                                  
          ELSEIF (NBPGAU.EQ.6) THEN                                     
C                                                                       
C         ELEMENT PRISME 6 NOEUDS * INTEGRATION A 6 POINTS              
C         CASTEM2000 RAPPORT DEMT 85/400                                
C                                                                       
          A = 1.D0/3.D0                                                 
          B = DSQRT(A)                                                  
          C = 1.D0/6.D0                                                 
          D = 2.D0/3.D0                                                 
          DO 60 I=1,3                                                   
            POIGAU(I) = C                                               
            POIGAU(I+3) = C                                             
            XG(I,3) = -B                                                
            XG(I+3,3) = B                                               
            XG(I,2) =  C                                                
            XG(I+3,2) = C                                               
            XG(I,1) =  C                                                
            XG(I+3,1) = C                                               
60          CONTINUE                                                    
          XG(2,1) = D                                                   
          XG(3,2) = D                                                   
          XG(5,1) = D                                                   
          XG(6,2) = D                                                   
          ELSEIF (NBPGAU.EQ.9) THEN                                     
C                                                                       
C         ELEMENT PRISME 6 NOEUDS * INTEGRATION A 9 POINTS              
C         A VERIFIER                                                    
C                                                                       
          UNDEMI = 1.D0/2.D0                                            
          UNSIX = 1.D0/6.D0                                             
          P555 = .555555555555555555D0                                  
          P888 = .888888888888888888D0                                  
          X774 = .774596669241483D0                                     
          DO 70 IA=1,7,3                                                
            XG(IA,1)=UNDEMI                                             
            XG(IA+1,2)=UNDEMI                                           
            XG(IA+2,1)=UNDEMI                                           
            XG(IA+2,2)=UNDEMI                                           
70          CONTINUE                                                    
          DO 80 IA=1,3                                                  
            XG(IA,3)=-X774                                              
            POIGAU(IA  )=P555*UNSIX                                     
            POIGAU(IA+3)=P888*UNSIX                                     
            POIGAU(IA+6)=P555*UNSIX                                     
            XG(IA+6,3)= X774                                            
80          CONTINUE                                                    
          ELSEIF (NBPGAU.EQ.12) THEN                                    
C                                                                       
C         ELEMENT PRISME 6 NOEUDS * INTEGRATION A 12 POINTS             
C         CASTEM2000 RAPPORT DEMT 85/400                                
C                                                                       
          TROIS = 3.D0                                                  
          UNTIER = 1.D0/3.D0                                            
          UNCINQ = 1.D0/5.D0                                            
          P555 = .555555555555555555D0                                  
          P888 = .888888888888888888D0                                  
          X774 = .774596669241483D0                                     
          P260 = .2604166666667D0                                       
          P281 = -.28125D0                                              
          DO 90 IA=2,10,4                                               
            XG(IA,1)=UNCINQ                                             
            XG(IA,2)=UNCINQ                                             
90          CONTINUE                                                    
          DO 100 IA=3,11,4                                              
            XG(IA,1)=UNCINQ*TROIS                                       
            XG(IA,2)=UNCINQ                                             
            XG(IA+1,1)=UNCINQ                                           
            XG(IA+1,2)=UNCINQ*TROIS                                     
100         CONTINUE                                                    
          DO 110 IA=1,9,4                                               
            XG(IA,1)=UNTIER                                             
            XG(IA,2)=UNTIER                                             
110         CONTINUE                                                    
          DO 120 IA=1,4                                                 
            XG(IA,3)=-X774                                              
            XG(IA+8,3)= X774                                            
120         CONTINUE                                                    
          DO 130 IA=2,4                                                 
            POIGAU(IA  )=P260*P555                                      
            POIGAU(IA+4)=P260*P888                                      
            POIGAU(IA+8)=P260*P555                                      
130         CONTINUE                                                    
          POIGAU(1)=P281*P555                                           
          POIGAU(5)=P281*P888                                           
          POIGAU(9)=P281*P555                                           
          ENDIF                                                         
        ELSEIF (CELE.EQ.'CUB8') THEN                                    
        IF (NBPGAU.EQ.1) THEN                                           
C                                                                       
C         ELEMENT CUBE 8 NOEUDS * INTEGRATION A 1 POINT                 
C                                                                       
          POIGAU(1) = 8.D0                                              
          XG(1,1) = 0.D0                                                
          XG(1,2) = 0.D0                                                
          XG(1,3) = 0.D0                                                
          ELSEIF (NBPGAU.EQ.8) THEN                                     
C                                                                       
C         ELEMENT CUBE 8 NOEUDS * INTEGRATION A 8 POINTS                
C         INTEGRE LE VOLUME, DIVERGENCE/GRADIENT ET FI*DFI              
C                                                                       
          DO 150 I=1,8                                                  
            POIGAU(I) = 1.D0                                            
150         CONTINUE                                                    
          A = .577350269189626D0                                        
          XG(1,1) = -A                                                  
          XG(1,2) = -A                                                  
          XG(1,3) = -A                                                  
          XG(2,1) =  A                                                  
          XG(2,2) = -A                                                  
          XG(2,3) = -A                                                  
          XG(3,1) =  A                                                  
          XG(3,2) =  A                                                  
          XG(3,3) = -A                                                  
          XG(4,1) = -A                                                  
          XG(4,2) =  A                                                  
          XG(4,3) = -A                                                  
          XG(5,1) = -A                                                  
          XG(5,2) = -A                                                  
          XG(5,3) =  A                                                  
          XG(6,1) =  A                                                  
          XG(6,2) = -A                                                  
          XG(6,3) =  A                                                  
          XG(7,1) =  A                                                  
          XG(7,2) =  A                                                  
          XG(7,3) =  A                                                  
          XG(8,1) = -A                                                  
          XG(8,2) =  A                                                  
          XG(8,3) =  A                                                  
          ELSEIF (NBPGAU.EQ.27) THEN                                    
C                                                                       
C         ELEMENT CUBE 8 NOEUDS * INTEGRATION A 27 POINTS               
C         INTEGRE LA MASSE ET FI*FI*DFI                                 
C                                                                       
          P5 = 5.D0/9.D0                                                
          P8 = 8.D0/9.D0                                                
          P5P8 = P5*P8                                                  
          P5P5 = P5*P5                                                  
          P8P8 = P8*P8                                                  
          POIGAU(1) = P5P5                                              
          POIGAU(2) = P5P8                                              
          POIGAU(3) = P5P5                                              
          POIGAU(4) = P5P8                                              
          POIGAU(5) = P8P8                                              
          POIGAU(6) = P5P8                                              
          POIGAU(7) = P5P5                                              
          POIGAU(8) = P5P8                                              
          POIGAU(9) = P5P5                                              
          DO 160 I=10,18                                                
            POIGAU(I) = POIGAU(I-9)*P8                                  
            POIGAU(I+9) = POIGAU(I-9)*P5                                
160         CONTINUE                                                    
          DO 170 I=1,9                                                  
            POIGAU(I) = POIGAU(I+18)                                    
170         CONTINUE                                                    
          X774 = DSQRT(3.D0/5.D0)                                       
          XG(1,1) = -X774                                               
          XG(1,2) = -X774                                               
          XG(2,1) =  0.D0                                               
          XG(2,2) = -X774                                               
          XG(3,1) =  X774                                               
          XG(3,2) = -X774                                               
          XG(4,1) = -X774                                               
          XG(4,2) =  0.D0                                               
          XG(5,1) =  0.D0                                               
          XG(5,2) =  0.D0                                               
          XG(6,1) =  X774                                               
          XG(6,2) =  0.D0                                               
          XG(7,1) = -X774                                               
          XG(7,2) =  X774                                               
          XG(8,1) =  0.D0                                               
          XG(8,2) =  X774                                               
          XG(9,1) =  X774                                               
          XG(9,2) =  X774                                               
          DO 180 I=1,9                                                  
            XG(I+9,1) = XG(I,1)                                         
            XG(I+9,2) = XG(I,2)                                         
            XG(I+18,1) = XG(I,1)                                        
            XG(I+18,2) = XG(I,2)                                        
            XG(I,3) = -X774                                             
            XG(I+9,3) = 0.D0                                            
            XG(I+18,3) = X774                                           
180         CONTINUE                                                    
          ENDIF                                                         
        ELSEIF (CELE.EQ.'QUA4') THEN                                    
        IF (NBPGAU.EQ.1) THEN                                           
C                                                                       
C         ELEMENT QUADRANGLE 4 NOEUDS * INTEGRATION A 1 POINT           
C         VOIR CUB8                                                     
C                                                                       
          POIGAU(1) = 4.D0                                              
          XG(1,1) = 0.D0                                                
          XG(1,2) = 0.D0                                                
          ELSEIF (NBPGAU.EQ.4) THEN                                     
C                                                                       
C         ELEMENT QUADRANGLE 4 NOEUDS * INTEGRATION A 4 POINTS          
C         VOIR CUB8                                                     
C                                                                       
          DO 200 I=1,4                                                  
            POIGAU(I) = 1.D0                                            
200         CONTINUE                                                    
          A = .577350269189626D0                                        
          XG(1,1) = -A                                                  
          XG(1,2) = -A                                                  
          XG(2,1) =  A                                                  
          XG(2,2) = -A                                                  
          XG(3,1) =  A                                                  
          XG(3,2) =  A                                                  
          XG(4,1) = -A                                                  
          XG(4,2) =  A                                                  
          ELSEIF (NBPGAU.EQ.9) THEN                                     
C                                                                       
C         ELEMENT QUADRANGLE 4 NOEUDS * INTEGRATION A 9 POINTS          
C         VOIR CUB8                                                     
C                                                                       
          P5 = 5.D0/9.D0                                                
          P8 = 8.D0/9.D0                                                
          P5P8 = P5*P8                                                  
          P5P5 = P5*P5                                                  
          P8P8 = P8*P8                                                  
          POIGAU(1) = P5P5                                              
          POIGAU(2) = P5P8                                              
          POIGAU(3) = P5P5                                              
          POIGAU(4) = P5P8                                              
          POIGAU(5) = P8P8                                              
          POIGAU(6) = P5P8                                              
          POIGAU(7) = P5P5                                              
          POIGAU(8) = P5P8                                              
          POIGAU(9) = P5P5                                              
          X774 = DSQRT(3.D0/5.D0)                                       
          DO 210 I=1,3                                                  
            XG(I,2)   = - X774                                          
            XG(I+3,2) =   0.D0                                          
            XG(I+6,2) =   X774                                          
            XG(3*(I-1)+1,1) = - X774                                    
            XG(3*(I-1)+2,1) =   0.D0                                    
            XG(3*(I-1)+3,1) =   X774                                    
210         CONTINUE                                                    
          ENDIF                                                         
        ELSEIF (CELE.EQ.'TRI3') THEN                                    
        IF (NBPGAU.EQ.1) THEN                                           
C                                                                       
C         ELEMENT TRIANGLE 3 NOEUDS * INTEGRATION A 1 POINT             
C         EXACT POUR LES POLYNOMES DE DEGRE 1                           
C         DAUTRAY-LIONS TOME 2 PAGE 780                                 
C                                                                       
          UNDEMI = 1.D0/2.D0                                            
          UNTIER = 1.D0/3.D0                                            
          POIGAU(1) = UNDEMI                                            
          XG(1,1) = UNTIER                                              
          XG(1,2) = UNTIER                                              
          ELSEIF (NBPGAU.EQ.3) THEN                                     
C                                                                       
C         ELEMENT TRIANGLE 3 NOEUDS * INTEGRATION A 3 POINTS            
C         EXACT POUR LES POLYNOMES DE DEGRE 2                           
C         DAUTRAY-LIONS TOME 2 PAGE 780                                 
C                                                                       
          UNDEMI = 1.D0/2.D0                                            
          UNSIX  = 1.D0/6.D0                                            
          POIGAU(1) = UNSIX                                             
          POIGAU(2) = UNSIX                                             
          POIGAU(3) = UNSIX                                             
          XG(1,1) = UNDEMI                                              
          XG(1,2) = 0.D0                                                
          XG(2,1) = 0.D0                                                
          XG(2,2) = UNDEMI                                              
          XG(3,1) = UNDEMI                                              
          XG(3,2) = UNDEMI                                              
          ELSEIF (NBPGAU.EQ.4) THEN                                     
C                                                                       
C         ELEMENT TRIANGLE 3 NOEUDS * INTEGRATION A 4 POINTS            
C         EXACT POUR LES POLYNOMES DE DEGRE 3                           
C         DAUTRAY-LIONS TOME 2 PAGE 780                                 
C                                                                       
          A = - 9.D0/32.D0                                              
          B =   25.D0/96.D0                                             
          UNTIER = 1.D0/3.D0                                            
          UNCINQ = 1.D0/5.D0                                            
          TRCINQ = 3.D0/5.D0                                            
          POIGAU(1) = A                                                 
          POIGAU(2) = B                                                 
          POIGAU(3) = B                                                 
          POIGAU(4) = B                                                 
          XG(1,1) = UNTIER                                              
          XG(1,2) = UNTIER                                              
          XG(2,1) = UNCINQ                                              
          XG(2,2) = UNCINQ                                              
          XG(3,1) = TRCINQ                                              
          XG(3,2) = UNCINQ                                              
          XG(4,1) = UNCINQ                                              
          XG(4,2) = TRCINQ                                              
          ELSEIF (NBPGAU.EQ.7) THEN                                     
C         ELEMENT TRIANGLE 3 NOEUDS * INTEGRATION A 7 POINTS            
C         EXACT POUR LES POLYNOMES DE DEGRE 5                           
C         DAUTRAY-LIONS TOME 2 PAGE 781                                 
          P1 = (9.D0/40.D0)*.5D0                                        
          P2 = ((155.D0 - DSQRT(15.D0))/1200.D0)*.5D0                   
          P3 = ((155.D0 + DSQRT(15.D0))/1200.D0)*.5D0                   
          UNTIER = 1.D0/3.D0                                            
          A = (6.D0 - DSQRT(15.D0))/21.D0                               
          C = (6.D0 + DSQRT(15.D0))/21.D0                               
          B = (9.D0 + 2*DSQRT(15.D0))/21.D0                             
          D = (9.D0 - 2*DSQRT(15.D0))/21.D0                             
          POIGAU(1) = P1                                                
          POIGAU(2) = P2                                                
          POIGAU(3) = P2                                                
          POIGAU(4) = P2                                                
          POIGAU(5) = P3                                                
          POIGAU(6) = P3                                                
          POIGAU(7) = P3                                                
          XG(1,1) = UNTIER                                              
          XG(1,2) = UNTIER                                              
          XG(2,1) = A                                                   
          XG(2,2) = A                                                   
          XG(3,1) = A                                                   
          XG(3,2) = B                                                   
          XG(4,1) = B                                                   
          XG(4,2) = A                                                   
          XG(5,1) = C                                                   
          XG(5,2) = C                                                   
          XG(6,1) = C                                                   
          XG(6,2) = D                                                   
          XG(7,1) = D                                                   
          XG(7,2) = C                                                   
          ENDIF                                                         
        ENDIF                                                           
C                                                                       
      RETURN                                                            
      END                                                               
