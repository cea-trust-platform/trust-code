cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     c
c     c File:	routine_fft.f
c     c Directory:	$TRUST_ROOT/VDF/Turbulence
c     c
c     c .NOM  routine_fft.f: 
c     c .ENTETE TRUST VDF/Turbulence
c     c .LIBRAIRIE libhydVDFturb
c     c .FILE routine_fft.f 
c     c .DESCRIPTION  
c     c 
c     Nous rassemblons dans ce fichier toutes les routines 
c     fortran necessaires aux calculs des fft!!
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     
c------------------------------------------
c     CALCUL DES FFT
c------------------------------------------
      subroutine normaliserm (C,n,nn)
      implicit none
      integer i,j,k,n
      real *8  C(n+2,n,n),nn
      do 10 i=1,n+2
         do 10 j=1,n
            do 10 k=1,n
               C(i,j,k) = C(i,j,k)/(nn*nn*nn)
 10         continue
            return
            end          
c------------------------------------------
      subroutine prepfftnewnew(u,uu,n)
      implicit none
      integer  i,j,k,n       
      real *8  uu(n+2,n,n),u(n+2,n,n)
c     
      do 10 j=1,n
         do 10 k=1,n
            do 20 i=1,n
               uu(i,j,k)=u(i,j,k)
 20         continue	  	     
            uu(n+1,j,k)=0.d0
            uu(n+2,j,k)=0.d0
 10      continue	   
         return
         end    
c     
c------------------------------------------
      subroutine prepfftnew(u,uu,n)
      implicit none
      integer  i,j,k,n       
      real *8  uu(n+2,n,n),u(n+1,n+1,n+1)
c     
      do 10 j=1,n
         do 10 k=1,n
            do 20 i=1,n
               uu(i,j,k)=u(i,j,k)
 20         continue	  	     
            uu(n+1,j,k)=0.d0
            uu(n+2,j,k)=0.d0
 10      continue	   
         return
         end    
c     
c------------------------------------------
c     
      subroutine fftretour_espphy(u,uu,n,nn,wl,wm,wn,iwork)
c     
      implicit none
      integer n,id
      real *8 u(n+2,n,n), uu(n+2,n,n) ,nn      
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n),ierr
c     
      id=n+2   
      nn = n*1.0
      call prepfftnewnew(u,uu,n)               
c     
      call R3FFT(uu,id,n,n,n,wl,wm,wn,0,0,1,iwork,ierr)     
      call R3FFT(uu,id,n,n,n,wl,wm,wn,0,1,1,iwork,ierr)  
c     
      return
      end
c     
c------------------------------------------
c     
      subroutine fftrm3new(u,uu,n,wl,wm,wn,iwork)
c     
      implicit none
      integer n,id  
      real *8 uu(n+2,n,n),u(n+1,n+1,n+1) ,nn     
      integer  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n),ierr
c     
      id=n+2  
      nn = n*1.0
      call prepfftnew(u,uu,n)       
c     
      call R3FFT(uu,id,n,n,n,wl,wm,wn,0,0,1,iwork,ierr)        
      call R3FFT(uu,id,n,n,n,wl,wm,wn,0,-1,1,iwork,ierr)  
c     
      call normaliserm (uu,n,nn)       
c     
c     
      return
      end   
c------------------------------------------
c     CALCUL DES SPECTRES
c------------------------------------------
c     
      subroutine spectrermcs(U,V,W,E,EX,EY,EZ,n,fx,fy,fz,nE)
      implicit none
      integer  n,i,j,k,nn,ind,nE(n)
      real *8 fx(n),fy(n),fz(n),f1,f2,f ,delta      
      real *8  U(n+2,n,n),V(n+2,n,n),W(n+2,n,n)
      real *8  E(3*n*n),EX(3*n*n),EY(3*n*n),EZ(3*n*n),PI,c
      PI=ACOS(-1.d0)
      delta=2*PI/(n*1.d0)
c     
c     "initialisation de l'energie"  
      do 05 i=1,n
         E(i)=0.d0
         EX(i)=0.d0
         EY(i)=0.d0
         EZ(i)=0.d0	    
         nE(i)=0.d0
 05   continue
c     
      do 10 i=1,n,2
         f1 = fx(i)*fx(i)
	 if (i.eq.1) then
	    c=2*PI
	    nn=1
	 else
	    c=4*PI
	    nn=2
	 endif   
         do 10 j=1,n
            f2 = fy(j)*fy(j) + f1
            do 10 k=1,n
               f=f2+fz(k)*fz(k)
               ind=dsqrt(f)+0.5
               if (ind.GE.1) then	    
                  EX(ind)=c*f*(U(i,j,k)**2+U(i+1,j,k)**2)+EX(ind)
                  EY(ind)=c*f*(V(i,j,k)**2+V(i+1,j,k)**2)+EY(ind) 
                  EZ(ind)=c*f*(W(i,j,k)**2+W(i+1,j,k)**2)+EZ(ind)
                  nE(ind)=nE(ind)+1*nn
               endif	    
c     
 10         continue
c     
            do 15 i=1,n
               if (nE(i).GT.0) then
                  EX(i)=EX(i)/(1.d0*nE(i))
                  EY(i)=EY(i)/(1.d0*nE(i))
                  EZ(i)=EZ(i)/(1.d0*nE(i))
                  E(i)=EX(i)+EY(i)+EZ(i)
               endif
 15         continue 
c     
            return
            end  
c     -----------------------------------------------------------------   
      subroutine freq (fx,fy,fz,n)
      implicit none
      integer  n,i
      real *8   fx(n),fy(n),fz(n)
c     
      do 10 i=1,n/2
         fx(2*i-1)=(i-1)*1.d0
         fx(2*i)=(i-1)*1.d0
         fy(i)=(i-1)*1.d0
         fy(i+n/2)=(i-(n/2+2)+1)*1.d0
         fz(i)=(i-1)*1.d0
         fz(i+n/2)=(i-(n/2+2)+1)*1.d0
 10   continue	
      fy(n/2+1)=0.d0
      fz(n/2+1)=0.d0
c     
      return
      end
