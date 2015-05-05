cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     c
c     c File:	lireprsp_pr_trio_u.f
c     c Directory:	$TRUST_ROOT/VDF/Turbulence
c     c
c     c .NOM  lireprsp_pr_trio_u : 
c     c .ENTETE TRUST VDF/Turbulence
c     c .LIBRAIRIE libhydVDFturb
c     c .FILE  lireprsp_pr_trio_u.f
c     c .DESCRIPTION  
c     c 
c     Nous rassemblons dans ce fichier toutes les routines 
c     fortran necessaires aux calculs des fft!!
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine CALCUL0SPECTRE0AP0CHP0VERIF(vitesse_sommet,E,EX,EY,EZ,n
     *     ,uuu,vvv,www,u,v,w,uu,vv,ww,nE,fx,fy,fz,wl,wm,wn,iwork)
c     calcul_spectre_ap_chp
c     
      implicit none
      integer n
      real *8 u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1)
      real *8 uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1)
      real *8 uuu(n+2,n,n),vvv(n+2,n,n),www(n+2,n,n)
      real *8 E(n),nE(n),EX(n),EY(n),EZ(n)
      real *8 fx(n),fy(n),fz(n) 
      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n)
c     
c     print *,"calcul a tps= ",tps
c     
      call renum(vitesse_sommet,u,v,w,n)
c     
      call rendperiod4u(u,uu,n)   
      call rendperiod4v(v,vv,n)
      call rendperiod4w(w,ww,n)   
c     
      call freq (fx,fy,fz,n)     
c     

c     
      call fftrm3new(uu,uuu,n,wl,wm,wn,iwork)      
      call fftrm3new(vv,vvv,n,wl,wm,wn,iwork)          
      call fftrm3new(ww,www,n,wl,wm,wn,iwork) 
c     
      call spectrermcs(uuu,vvv,www,E,EX,EY,EZ,n,fx,fy,fz,nE) 
c     
      end   
c     
      subroutine CALCUL0SPECTRE0AP0CHP0PER
     *     (vitesse_sommet,E,n,uuu,vvv,www,
     *      u,v,w,uu,vv,ww,nE,EX,EY,EZ,fx,fy,fz
     *      ,wl,wm,wn,iwork)
c     calcul_spectre_ap_chp
c     
      implicit none
      integer n
      real *8 u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1)
      real *8 uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1)
      real *8 uuu(n+2,n,n),vvv(n+2,n,n),www(n+2,n,n)
      real *8 E(n),nE(n),EX(n),EY(n),EZ(n)
      real *8 fx(n),fy(n),fz(n) 
      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n)
c     
c     print *,"calcul a tps= ",tps
c     
      call renum(vitesse_sommet,u,v,w,n)
c     
c     call rendperiod4u(u,uu,n)   
c     call rendperiod4v(v,vv,n)
c     call rendperiod4w(w,ww,n)   
c     
      call freq (fx,fy,fz,n)     
c     

c     
      call fftrm3new(u,uuu,n,wl,wm,wn,iwork)     
      call fftrm3new(v,vvv,n,wl,wm,wn,iwork)       
      call fftrm3new(w,www,n,wl,wm,wn,iwork)
c     
      call spectrermcs(uuu,vvv,www,E,EX,EY,EZ,n,fx,fy,fz,nE) 
c     
      end   
c     
c     
      subroutine CALCUL0SPECTRE0AVEC030VIT
     *     (vit_u,vit_v,vit_w,E,n,uuu,vvv,www,
     *      u,v,w,uu,vv,ww,nE,EX,EY,EZ,fx,fy,fz
     *      ,wl,wm,wn,iwork)
c     calcul_spectre_ap_chp
c     
      implicit none
      integer n
      real *8 u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1)
      real *8 uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1)
      real *8 uuu(n+2,n,n),vvv(n+2,n,n),www(n+2,n,n)
      real *8 E(n),nE(n),EX(n),EY(n),EZ(n)
      real *8 fx(n),fy(n),fz(n) 
c      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))
      real *8 vit_u(n+1,n+1,n+1),vit_v(n+1,n+1,n+1),vit_w(n+1,n+1,n+1)
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n)
c     
c     print *,"calcul a tps= ",tps
c     
c      call renum(vitesse_sommet,u,v,w,n)
c     
c     call rendperiod4u(u,uu,n)   
c     call rendperiod4v(v,vv,n)
c     call rendperiod4w(w,ww,n)   
c     
      call freq (fx,fy,fz,n)     
c     

c     
      call fftrm3new(vit_u,uuu,n,wl,wm,wn,iwork)      
      call fftrm3new(vit_v,vvv,n,wl,wm,wn,iwork)
      call fftrm3new(vit_w,www,n,wl,wm,wn,iwork) 
c     
      call spectrermcs(uuu,vvv,www,E,EX,EY,EZ,n,fx,fy,fz,nE) 
c     
      end   
c     
c----------------
c     
CC      subroutine CALCUL0SPECTRE0AP0TF(uuu,vvv,www,E,n,fx,fy,fz)
c     calcul_spectre_ap_tf
c     
CC      implicit none
CC      integer NP,nt,n,i,j,k,l,ni,n1,n2,nn,n3,nf
CC      real *8 uuu(n+2,n,n),vvv(n+2,n,n),www(n+2,n,n),nu
CC      real *8 E(n),nE(n),EX(n),EY(n),EZ(n)
CC      real *8 fx(n),fy(n),fz(n) 
c     
CC      call freq (fx,fy,fz,n)      
CC      call spectrermcs(uuu,vvv,www,E,EX,EY,EZ,n,fx,fy,fz,nE) 	 
c     
CC      end   
c     
c----------------
      subroutine renum(vitesse_sommet,u,v,w,n)
c     
      implicit none
      integer i,j,k,n,num_som
      real *8 u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1)
      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))      
c     
      num_som = 1
      do 10 k=1,n+1
         do 10 j=1,n+1
            do 10 i=1,n+1
               u(i,j,k)=vitesse_sommet(1,num_som)
               v(i,j,k)=vitesse_sommet(2,num_som)
               w(i,j,k)=vitesse_sommet(3,num_som)
               num_som = num_som + 1
 10         continue
c     write(*,*) u(i,j,k),v(i,j,k),w(i,j,k)
c     
            end
c     
      subroutine renum0inv(u,v,w,n,vitesse_sommet)
c     
      implicit none
      integer i,j,k,n,num_som
      integer ii,jj,kk
      real *8 u(n+2,n,n),v(n+2,n,n),w(n+2,n,n)
      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))      
c     
      num_som = 1
      do 10 k=1,n+1
         if (k.EQ.(n+1)) then
            kk = 1
         else
            kk=k
         endif
         do 10 j=1,n+1
            if (j.EQ.(n+1)) then
               jj = 1
            else
               jj=j
            endif
            do 10 i=1,n+1
               if (i.EQ.(n+1)) then
                  ii= 1
               else
                  ii=i
               endif
               vitesse_sommet(1,num_som)=u(ii,jj,kk)
               vitesse_sommet(2,num_som)=v(ii,jj,kk)
               vitesse_sommet(3,num_som)=w(ii,jj,kk)
               num_som = num_som + 1
 10         continue
c     
            end
c     
c----------------
      subroutine CALCUL0TR0INVERSE
     &     (fft_u,fft_v,fft_w,vitesse_sommet,n,nn,
     &      u,v,w,uuu,vvv,www,E,nE,EX,EY,EZ,fx,fy,fz,
     &      wl,wm,wn,iwork)
c     calcul_tr_inverse
      implicit none
      integer n
      real *8 nn
      real *8 u(n+2,n,n),v(n+2,n,n),w(n+2,n,n)
      real *8 uuu(n+2,n,n),vvv(n+2,n,n),www(n+2,n,n)
      real *8 fft_u(n+2,n,n),fft_v(n+2,n,n),fft_w(n+2,n,n)   
      real *8 vitesse_sommet(3,(n+1)*(n+1)*(n+1))
      real *8 E(n),nE(n),EX(n),EY(n),EZ(n)
      real *8 fx(n),fy(n),fz(n) 
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n)

      nn = n *1.0
      call freq (fx,fy,fz,n)    
      call spectrermcs(fft_u,fft_v,fft_w,E,EX,EY,EZ,n,fx,fy,fz,nE) 
c     call ecritspectrermcsav(E,n)
c     
      call fftretour_espphy(fft_u,u,n,nn,wl,wm,wn,iwork) 
      call fftretour_espphy(fft_v,v,n,nn,wl,wm,wn,iwork)
      call fftretour_espphy(fft_w,w,n,nn,wl,wm,wn,iwork)
c     
c     call fftrm4new(u,uuu,n)      
c     call fftrm4new(v,vvv,n)          
c     call fftrm4new(w,www,n) 
c     
c     call spectrermcs(uuu,vvv,www,E,EX,EY,EZ,n,fx,fy,fz,nE) 
c     call ecritspectrermcsap(E,n)
c     c      call ecritspectrermcsap1(EX,n)
c     c      call ecritspectrermcsap2(EY,n)
c     c      call ecritspectrermcsap3(EZ,n)
c     
      call renum0inv(u,v,w,n,vitesse_sommet)
c     
      end      
c     
c----------------
      subroutine EXTRAPOLE
     *     (Eexp,E,U,V,W,n,fx,fy,fz,coeff)
c     
      implicit none
      integer  i,n,j,k,ver,ind
      real *8 Eexp(19,2),E(n),PI,coeff(n),coef
      real *8 fx(n), fy(n), fz(n),ftg,f,f1,f2
      real *8  U(n+2,n,n),V(n+2,n,n),W(n+2,n,n) 
      real *8 a,ee,nn,c
c     
      PI=ACOS(-1.d0)
c     
      do 10 i=1,n
c     if (E(i).GT.1E-100) then
         ftg=i*1.d0
         ver=0
         do 20 j=1,19
            if (ftg.LE.Eexp(1,1)) then
               a=(dlog(Eexp(2,2))-dlog(Eexp(1,2)))
     *              /(dlog(Eexp(2,1))-dlog(Eexp(1,1)))
               a=a*(dlog(ftg)-dlog(Eexp(1,1)))
               a=a+dlog(Eexp(1,2))
               ee=dexp(a)
               ver=1
               goto 20
            endif	   
            if ((ftg.GT.Eexp(j,1)).AND.(ftg.LE.Eexp(j+1,1))) then
               a=(dlog(Eexp(j+1,2))-dlog(Eexp(j,2)))
     *              /(dlog(Eexp(j+1,1))-dlog(Eexp(j,1)))
               a=a*(dlog(ftg)-dlog(Eexp(j,1)))
               a=a+dlog(Eexp(j,2))
               ee=dexp(a)
               ver=1
               goto 20
            endif
 20      continue
         if (ver.EQ.0) then
c     print *,"ATTENTION PAS D INTERPOLATION POUR K=",ftg
	 endif
         if (E(i).GT.1E-30) then
            coeff(i) = ee/E(i)
	 endif 
c     endif
 10   continue
c     
      do 11 i=1,n,2
         f1 = fx(i)*fx(i)
	 if (i.eq.1) then
	    c=2*PI
	    nn=1
	 else
	    c=4*PI
	    nn=2
	 endif   
         do 11 j=1,n
            f2 = fy(j)*fy(j) + f1	   
            do 11 k=1,n
               f=f2+fz(k)*fz(k)
               ind=dsqrt(f)+0.5
               if (ind.NE.0) then
                  coef=dsqrt(coeff(ind))
                  U(i,j,k)=coef*U(i,j,k)
                  U(i+1,j,k)=coef*U(i+1,j,k)	    
                  V(i,j,k)=coef*V(i,j,k)
                  V(i+1,j,k)=coef*V(i+1,j,k)	  	    
                  W(i,j,k)=coef*W(i,j,k)
                  W(i+1,j,k)=coef*W(i+1,j,k)
               endif
 11         continue  
            return
            end
c----------------
      subroutine EXTRAPOLE0E
     *     (Emod,E,U,V,W,n,fx,fy,fz,coeff)
c     
      implicit none
      integer  i,n,j,k,ind
      real *8 Emod(n),E(n),PI,coeff(n),coef
      real *8 fx(n), fy(n), fz(n),f,f1,f2
      real *8  U(n+2,n,n),V(n+2,n,n),W(n+2,n,n) 
      real *8 nn,c
c     
      PI=ACOS(-1.d0)
c     
      do 10 i=1,n
         if (E(i).GT.1E-30) then
            coeff(i) = Emod(i)/E(i)
            print *, coeff(i)
	 endif 
 10   continue
c     
      do 11 i=1,n,2
         f1 = fx(i)*fx(i)
	 if (i.eq.1) then
	    c=2*PI
	    nn=1
	 else
	    c=4*PI
	    nn=2
	 endif   
         do 11 j=1,n
            f2 = fy(j)*fy(j) + f1	   
            do 11 k=1,n
               f=f2+fz(k)*fz(k)
               ind=dsqrt(f)+0.5
               if (ind.NE.0) then
                  coef=dsqrt(coeff(ind))
                  U(i,j,k)=coef*U(i,j,k)
                  U(i+1,j,k)=coef*U(i+1,j,k)	    
                  V(i,j,k)=coef*V(i,j,k)
                  V(i+1,j,k)=coef*V(i+1,j,k)	  	    
                  W(i,j,k)=coef*W(i,j,k)
                  W(i+1,j,k)=coef*W(i+1,j,k)
               endif
 11         continue  
            return
            end
c-------------------------------------------
      subroutine rendperiod4u(u,uu,n)
      implicit none
      integer i,j,k,n
      real *8 u(n+1,n+1,n+1),uu(n+1,n+1,n+1)
c     
      do 05 i=1,n+1
         do 05 j=1,n+1
   	    do 05 k=1,n+1
               uu(i,j,k)=u(i,j,k)
 05         continue   
c     Calculs pour les sommets "interieurs" a une face de periodicite
c     (i.e. ni les aretes ni les coins)
      do 10 i=2,n
        do 10 j=2,n
         uu(i,j,1)=(u(i,j,1)+u(i,j,n+1))/2.d0
         uu(i,j,n+1)=uu(i,j,1)
 10     continue 
      do 20 i=2,n
       do 20 k=2,n
         uu(i,1,k)=(u(i,1,k)+u(i,n+1,k))/2.d0
         uu(i,n+1,k)=uu(i,1,k)    
 20    continue   
      do 30 j=2,n
       do 30 k=2,n
         uu(1,j,k)=(u(1,j,k)+u(n+1,j,k))/2.d0
         uu(n+1,j,k)=uu(1,j,k)  
 30     continue 
      do 40 j=2,n
        uu(1,j,1)=(u(1,j,1)+u(1,j,n+1))/2.d0
        uu(1,j,n+1)=uu(1,j,1)
        uu(n+1,j,1)=(u(1,j,1)+u(1,j,n+1))/2.d0
        uu(n+1,j,n+1)=uu(1,j,1)	      
 40   continue 
      do 60 k=2,n
        uu(1,1,k)=(u(1,1,k)+u(1,n+1,k))/2.d0
        uu(1,n+1,k)=uu(1,1,k) 
        uu(n+1,1,k)=(u(n+1,1,k)+u(n+1,n+1,k))/2.d0
        uu(n+1,n+1,k)=uu(1,1,k) 	      
 60   continue    
      do 50 i=1,n+1 
        uu(i,1,1)=(u(i,1,1)+u(i,1,n+1)+u(i,n+1,1)+u(i,n+1,n+1))/4.d0
                        uu(i,1,n+1)=uu(i,1,1)
                        uu(i,n+1,1)=uu(i,1,1)
                        uu(i,n+1,n+1)=uu(i,1,1)	      	      
 50   continue    
c     
      return
      end	
c     
c----------------
      subroutine rendperiod4v(u,uu,n)
      implicit none
      integer i,j,k,n
      real *8 u(n+1,n+1,n+1),uu(n+1,n+1,n+1)
c     
      do 05 i=1,n+1
         do 05 j=1,n+1
   	    do 05 k=1,n+1
               uu(i,j,k)=u(i,j,k)
 05         continue   
c     Calculs pour les sommets "interieurs" a une face de periodicite
c     (i.e. ni les aretes ni les coins)
       do 10 i=2,n
        do 10 j=2,n
         uu(i,j,1)=(u(i,j,1)+u(i,j,n+1))/2.d0
         uu(i,j,n+1)=uu(i,j,1)
 10     continue 
       do 20 i=2,n
         do 20 k=2,n
          uu(i,1,k)=(u(i,1,k)+u(i,n+1,k))/2.d0
          uu(i,n+1,k)=uu(i,1,k)    
 20      continue   
         do 30 j=2,n
          do 30 k=2,n
            uu(1,j,k)=(u(1,j,k)+u(n+1,j,k))/2.d0
            uu(n+1,j,k)=uu(1,j,k)    
 30       continue 
         do 40 i=2,n
            uu(i,1,1)=(u(i,1,1)+u(i,1,n+1))/2.d0
            uu(i,1,n+1)=uu(i,1,1)
            uu(i,n+1,1)=(u(i,n+1,1)+u(i,n+1,n+1))/2.d0
            uu(i,n+1,n+1)=uu(i,n+1,1)	      
 40      continue 
         do 60 k=2,n
           uu(1,1,k)=(u(1,1,k)+u(n+1,1,k))/2.d0
           uu(n+1,1,k)=uu(1,1,k) 
           uu(1,n+1,k)=(u(1,n+1,k)+u(n+1,n+1,k))/2.d0
           uu(n+1,n+1,k)=uu(1,n+1,k) 	      
 60      continue    
         do 50 j=1,n+1
           uu(1,j,1)=(u(1,j,1)+u(1,j,n+1)+u(n+1,j,1)+u(n+1,j,n+1))/4.d0
           uu(1,j,n+1)=uu(1,j,1)
           uu(n+1,j,1)=uu(1,j,1)
           uu(n+1,j,n+1)=uu(1,j,1)	      	      
 50      continue    
c     
        return
        end	
c     
c----------------
      subroutine rendperiod4w(u,uu,n)
      implicit none
      integer i,j,k,n
      real *8 u(n+1,n+1,n+1),uu(n+1,n+1,n+1)
c     
      do 05 i=1,n+1
         do 05 j=1,n+1
   	    do 05 k=1,n+1
               uu(i,j,k)=u(i,j,k)
 05         continue   
c     Calculs pour les sommets "interieurs" a une face de periodicite
c     (i.e. ni les aretes ni les coins)
       do 10 i=2,n
         do 10 j=2,n
           uu(i,j,1)=(u(i,j,1)+u(i,j,n+1))/2.d0
           uu(i,j,n+1)=uu(i,j,1)
 10      continue 
         do 20 i=2,n
           do 20 k=2,n
            uu(i,1,k)=(u(i,1,k)+u(i,n+1,k))/2.d0
            uu(i,n+1,k)=uu(i,1,k)    
 20      continue   
         do 30 j=2,n
           do 30 k=2,n
              uu(1,j,k)=(u(1,j,k)+u(n+1,j,k))/2.d0
              uu(n+1,j,k)=uu(1,j,k)    
 30      continue 
         do 40 i=2,n
            uu(i,1,1)=(u(i,1,1)+u(i,n+1,1))/2.d0
            uu(i,n+1,1)=uu(i,1,1)
            uu(i,1,n+1)=(u(i,1,n+1)+u(i,n+1,n+1))/2.d0
            uu(i,n+1,n+1)=uu(i,1,n+1)	      
 40      continue 
         do 60 j=2,n
            uu(1,j,1)=(u(1,j,1)+u(n+1,j,1))/2.d0
            uu(n+1,j,1)=uu(1,j,1) 
            uu(1,j,n+1)=(u(1,j,n+1)+u(n+1,j,n+1))/2.d0
            uu(n+1,j,n+1)=uu(1,j,n+1) 	      
 60      continue    
         do 50 k=1,n+1
           uu(1,1,k)=(u(1,1,k)+u(n+1,1,k)+u(1,n+1,k)+u(n+1,n+1,k))/4.d0
           uu(n+1,1,k)=uu(1,1,k)
           uu(1,n+1,k)=uu(1,1,k)
           uu(n+1,n+1,k)=uu(1,1,k)	      	      
 50      continue    
c     
         return
         end	 
