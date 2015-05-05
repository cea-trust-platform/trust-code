        subroutine read_data(indmail,nx,ny,nz)
        integer indmail(nx,ny,nz)
        OPEN(UNIT=1,FILE='avs/model.val',FORM='UNFORMATTED')
        READ(1)INDMAIL
        CLOSE(1)
        END
        subroutine write_file_data(xi,yi,zi,indmail,nx,ny,nz)
        integer indmail(nx,ny,nz)
        real xi(nx),yi(ny),zi(nz)
        OPEN(UNIT=1,FILE='verif.val',FORM='UNFORMATTED')
        WRITE(1)XI
        WRITE(1)YI
        WRITE(1)ZI
        WRITE(1)INDMAIL
        CLOSE(1)
        END
        subroutine read_data_2(indmail,nx,ny,nz)
        integer indmail(nx,ny,nz)
        real xi(nx),yi(ny),zi(nz)
        OPEN(UNIT=1,FILE='verif.val',FORM='UNFORMATTED')
        READ(1)XI
        READ(1)YI
        READ(1)ZI
        READ(1)INDMAIL
        CLOSE(1)
        END
        subroutine write_file_data_2(xi,yi,zi,indmail,nx,ny,nz)
        integer indmail(nx,ny,nz)
        real xi(nx),yi(ny),zi(nz)
        OPEN(UNIT=1,FILE='verif2.val',FORM='UNFORMATTED')
        WRITE(1)XI
        WRITE(1)YI
        WRITE(1)ZI
        WRITE(1)INDMAIL
        CLOSE(1)
        END
