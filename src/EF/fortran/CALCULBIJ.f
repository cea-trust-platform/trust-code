        subroutine calculbij(nbnn,nbsomtot,xl,num,bij,poro,ip,npgau,
     1   xgau,frgau,dfrgau,poigau,detj,ajm1,aj,df,volume,
     2   volume_sommet,iphi) 
        implicit none
        integer nbnn,nbsomtot,num(nbnn),ip,npgau,l,idimg
        real*8 xl(nbnn),bij(nbnn,1,3),poro(1),xgau(npgau,3)
        real*8 FRGAU(NPGAU,NBNN),DFRGAU(NPGAU,NBNN,3)
        real*8 POIGAU(NPGAU)
        real*8 DETJ(NPGAU)
        real*8 AJM1(NPGAU,3,3),AJ(NPGAU,3,3),DF(NPGAU,nbnn,3)
        real*8 volume,volume_sommet(nbsomtot),iphi(nbnn)
       
        logical JSHORT
        JSHORT = .FALSE.    
        l=1
        idimg=3
        CALL JACOB(IDIMG ,NBNN ,NPGAU ,XL     ,XGAU                    
     1            ,DETJ  ,AJM1 ,AJ     ,JSHORT)                         
        CALL DERIVE(IDIMG  ,NBNN  ,NPGAU  ,AJM1  ,DFRGAU  ,DF)         
        IF (ip.eq.0) THEN                                           
          CALL VOMABT(IDIMG     ,NBNN       ,NBNN,NPGAU  ,l  ,   
     1                L         ,POIGAU     ,DETJ  ,FRGAU   ,DF      ,  
     2               num, volume, volume_sommet,bij,iphi)
        ELSE                                                            
          CALL CALFGR(IDIMG     ,NBNN       ,NBNN,NPGAU  ,l  ,  
     1                L         ,POIGAU     ,DETJ  ,FRGAU   ,DF      ,  
     2                NUM ,poro, volume, volume_sommet,bij ,iphi)
        ENDIF                                                           
90    CONTINUE                                                          
C                                                                       
      RETURN                                                            
      END                                                               

