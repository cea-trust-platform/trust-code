
        subroutine initgauss(idimg,nbnn,npgau,
     1   xgau,frgau,dfrgau,poigau) 
        implicit none
        integer nbnn,npgau,idimg
        real*8 xgau(npgau,3)
        real*8 FRGAU(NPGAU,NBNN),DFRGAU(NPGAU,NBNN,3)
        real*8 POIGAU(NPGAU)
        if (idimg.eq.3) then
        if (nbnn.eq.8) then
        CALL CHPGAU('CUB8',NPGAU,POIGAU,XGAU)
        else
        stop 'erreur initgauss dimension 3 et pas 8 noeuds'
        endif
        endif
        if (idimg.eq.2) then
        if (nbnn.eq.4) then
        CALL CHPGAU('QUA4',NPGAU,POIGAU,XGAU)
        else
        stop 'erreur initgauss dimension 2 et pas 4 noeuds'
        endif
        endif
        CALL FBPGAU(IDIMG,NBNN,NPGAU,XGAU,FRGAU,DFRGAU)
        return
        end
