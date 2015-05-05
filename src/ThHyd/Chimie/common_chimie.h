C INCLUDE_FORTRAN
      integer nrmax,ncmax
      parameter(nrmax=50,ncmax=12)
	integer nr,ncs,marqueur_espece_en_competition
	integer activite,aveccontrereaction
	double precision cw,stochio,tau_mel,correction
	common/reaction/nr,ncs,cw(nrmax),aveccontrereaction(nrmax),
     1 stochio(nrmax,ncmax),activite(nrmax,ncmax),tau_mel,
     1 correction(nrmax),marqueur_espece_en_competition
