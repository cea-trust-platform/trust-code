        subroutine calculsi(nbnn,co3d,poro,ip,npgau,
     1   xgau,frgau,dfrgau,poigau,detj,cnorme, si) 
        implicit none
        integer nbnn,ip,npgau,l,idimg,dimch
        real*8 co3d(8,3),poro(1),xgau(npgau,3),cnorme(3)
        real*8 FRGAU(NPGAU,NBNN),DFRGAU(NPGAU,NBNN,3)
        real*8 POIGAU(NPGAU)
        real*8 DETJ(NPGAU)
        real*8 valem(1),si(nbnn)
        real*8 AJM1(27,3,3),AJ(27,3,3),DF(27,4,3),xl(8,3)
       
        logical JSHORT,LCHAMP
        JSHORT = .TRUE.    
        if (nbnn.ne.4) stop 'erreur calculsi'
        l=1
        idimg=2
        valem(1)=1
        call TRCO32(CO3D,Cnorme,nbnn,xl)
        CALL JACOB(IDIMG ,NBNN ,NPGAU ,XL     ,XGAU                    
     1            ,DETJ  ,AJM1 ,AJ     ,JSHORT)                         
        LCHAMP= (ip.eq.1) 
        dimch=1
        CALL LNFIXX(VALEM , FRGAU  , DETJ   ,
     1     POIGAU , NBNN , NPGAU ,
     1                dimch, LCHAMP , poro , si)         
90    CONTINUE                                                          
C                                                                       
      RETURN                                                            
      END                                                               

