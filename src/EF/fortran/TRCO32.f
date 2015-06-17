      SUBROUTINE TRCO32(CO3,CN,NBNN,XL )                                
      IMPLICIT REAL*8 (A-H,O-Z)                                         
CC    CALCUL DES COORDONNEES DES NOEUDS D'UNE FACE DANS LE PLAN         
CC    DE CETTE FACE                                                     
CC    ON PREND LE NOEUD 1 COMME ORIGINE                                 
CC             LE VECTEUR 12 (NORMALISE) COMME AXE DES X                
CE    CO3 : COORDONNEES DANS LE REPERE 3D                               
CE    CN : NORMALE A LA FACE                                            
CE    NBNN : NOMBRE DE NOEUDS DE LA FACE                                
CS    XL  : COORDONNEES DANS LE REPERE 2D (LA FACE)                     
C                                                                       
C     OPERATIONS : 1 RACINE, 3 **, 3 /, 12 OU 18 *                      
C                                                                       
      DIMENSION CO3(8,3),CN(3),XL(8,3)                                  
C                                                                       
C     VECTEUR 12                                                        
      X12 = CO3(2,1) - CO3(1,1)                                         
      Y12 = CO3(2,2) - CO3(1,2)                                         
      Z12 = CO3(2,3) - CO3(1,3)                                         
C     LONGUEUR 12                                                       
      XM = DSQRT(X12**2 + Y12**2 + Z12**2)                              
C     AXE X                                                             
      XCX = X12/XM                                                      
      XCY = Y12/XM                                                      
      XCZ = Z12/XM                                                      
C     AXE Y (X VECT CN)                                                 
      YCX = XCY*CN(3) - XCZ*CN(2)                                       
      YCY = XCZ*CN(1) - XCX*CN(3)                                       
      YCZ = XCX*CN(2) - XCY*CN(1)                                       
C     PREMIER NOEUD (ORIGINE)                                           
      XL (1,1) = 0.                                                     
      XL (1,2) = 0.                                                     
C     DEUXIEME NOEUD (12 DANS (X,Y))                                    
      XL (2,1) = XM                                                     
      XL (2,2) = 0.                                                     
C     TROISIEME NOEUD (13 SCAL X , 13 SCAL Y)                           
      XL (3,1) = (CO3(3,1) - CO3(1,1))*XCX                              
     1         + (CO3(3,2) - CO3(1,2))*XCY                              
     2         + (CO3(3,3) - CO3(1,3))*XCZ                              
      XL (3,2) = (CO3(3,1) - CO3(1,1))*YCX                              
     1         + (CO3(3,2) - CO3(1,2))*YCY                              
     2         + (CO3(3,3) - CO3(1,3))*YCZ                              
      IF (NBNN.EQ.3)                           RETURN                   
C     QUATRIEME NOEUD (14 SCAL X , 14 SCAL Y)                           
      XL (4,1) = (CO3(4,1) - CO3(1,1))*XCX                              
     1         + (CO3(4,2) - CO3(1,2))*XCY                              
     2         + (CO3(4,3) - CO3(1,3))*XCZ                              
      XL (4,2) = (CO3(4,1) - CO3(1,1))*YCX                              
     1         + (CO3(4,2) - CO3(1,2))*YCY                              
     2         + (CO3(4,3) - CO3(1,3))*YCZ                              
                                               RETURN                   
      END                                                               
