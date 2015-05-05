 
      subroutine dlsodechimieold2(neq,y,t,tout,itol,rtol,atol,
     1     itask,istate,iopt,rwork,lrw,iwork,liw,mf)
      implicit none
      external f_chim,jac_chim
      INTEGER NEQ, ITOL, ITASK, ISTATE, IOPT, LRW, IWORK, LIW, MF
      DOUBLE PRECISION Y, T, TOUT, RTOL, ATOL, RWORK
      DIMENSION NEQ(*), Y(*), RTOL(*), ATOL(*), RWORK(LRW), IWORK(LIW)
      integer ii
      call dlsode(f_chim,neq,y,t,tout,itol,rtol,atol,itask,istate,
     1     iopt,rwork,lrw,iwork,liw,jac_chim,mf)
       do ii=1,neq(1)
          if (y(ii).lt.0) y(ii)=0.d0
       enddo
      end

      subroutine inittaillecommon(nreact,ncons)
      implicit none
      integer nreact,ncons,ii,jj
#Pinclude(common_chimie.h)
      nr=nreact
      ncs=ncons
      marqueur_espece_en_competition=-1
      if (nr.gt.nrmax) stop 'nr > nrmax'
      if (ncs.gt.ncmax) stop 'nc > ncmax'
      do ii=1,nrmax
         cw(ii)=0
         do jj=1,ncmax
            stochio(ii,jj)=0.d0
            activite(ii,jj)=0
         enddo
      enddo
      end
      subroutine initreactioncommon(ir,pstochio,pactivite,contrereac)
      implicit none
      integer ir,pactivite(*),contrereac
      double precision pstochio(*)
      integer ic
#Pinclude(common_chimie.h)
      aveccontrereaction(ir)=contrereac
      do ic=1,ncs
         stochio(ir,ic)=pstochio(ic)
         activite(ir,ic)=pactivite(ic)
      enddo
      end
      subroutine setcwreaction(ir,rcw)
      double precision rcw
      integer ir
#Pinclude(common_chimie.h)
      cw(ir)=rcw
      end
      subroutine setmarqueur(marqueur)
#Pinclude(common_chimie.h)
c     il faut decaler de 1 a cause du fortran....      
      marqueur_espece_en_competition=marqueur+1
      end
      subroutine printcommon()
      integer ir,ic
#Pinclude(common_chimie.h)
      print *,'nr',nr, 'ncs',ncs
      print *,'marqueur',marqueur_espece_en_competition
      do ir=1,nr
         print *,'reaction ', ir, ' cw ',cw(ir)
         print *,'ic stochio activite'
         do ic=1,ncs
            print *,ic,stochio(ir,ic),activite(ir,ic)
         enddo
      enddo
      cw(ir)=rcw
      end

      
      
      subroutine dlsode_chimie_s_old(neq,y,t,tout,t_mel,itol,rtol,atol,
     1     rwork,lrw,iwork,liw)
      implicit none
      external f_chim,jac_chim
      INTEGER NEQ, ITOL, ITASK, ISTATE, IOPT, LRW, IWORK, LIW, MF
      DOUBLE PRECISION Y, T, TOUT, RTOL, ATOL, RWORK,t_mel
      DIMENSION NEQ(*), Y(*), RTOL(*), ATOL(*), RWORK(LRW), IWORK(LIW)
      integer ii
      itask=1
      istate=1
      iopt=0
      mf=21
c      call print_common()
      call dlsode(f_chim,neq,y,t,tout,itol,rtol,atol,itask,istate,
     1     iopt,rwork,lrw,iwork,liw,jac_chim,mf)
      do ii=1,neq(1)
         if (y(ii).lt.0) y(ii)=0.d0
      enddo
      end
      subroutine dlsodechimies(neq,y,t,tout,t_mel,itol,rtol,atol,
     1     rwork,lrw,iwork,liw)
      implicit none
#Pinclude(common_chimie.h)
      external f_chim_2,jac_chim_2
      INTEGER NEQ, ITOL, ITASK, ISTATE, IOPT, LRW, IWORK, LIW, MF
      DOUBLE PRECISION Y, T, TOUT, RTOL, ATOL, RWORK,t_mel
      DIMENSION NEQ(*), Y(*), RTOL(*), ATOL(*), RWORK(LRW), IWORK(LIW)
      integer ii
c     on remplit tau_mel dans le common 
      tau_mel=t_mel
      itask=1
      istate=1
      iopt=0
c     pour ne pas avoir a donner la jacobienne
      mf=21
c      call print_common()
      call dlsode(f_chim_2,neq,y,t,tout,itol,rtol,atol,itask,istate,
     1     iopt,rwork,lrw,iwork,liw,jac_chim_2,mf)
      do ii=1,neq(1)
         if (y(ii).lt.1e-38) y(ii)=0.d0
      enddo
      end

      subroutine f_chim_2 (neq, t, y, ydot)
      implicit none
      integer neq, ir,ii,ic,ia,mcr
      double precision t, y, ydot, isto,cmin
      double precision  w1,cmin1
      dimension y(neq), ydot(neq)
#Pinclude(common_chimie.h)
      double precision w
      
      mcr=0
      w1=0
      if (tau_mel.gt.0.) then
c      if (1.eq.1) then
c         tau_mel=0
c      determination de c* pour chaque equation si ydot!=0        
              
         do ic=1,neq
            ydot(ic)=0
         enddo
         do ir=1,nr
            w=cw(ir)
            cmin=1.d38
            do ic=1,neq
               ia=activite(ir,ic)
               isto=stochio(ir,ic)
               if (ia.gt.0) then
                  if ((y(ic)/isto.lt.cmin).and.(ic.ne.
     1                 marqueur_espece_en_competition))  cmin=y(ic)/isto
                  do ii=1,ia
                     w=w*y(ic)
                  enddo
               endif
            enddo
c     on modifie w pour prendre en compte tau_mel
            if (aveccontrereaction(ir).eq.1) then
               if (mcr.eq.0) then
                  w1=w
                  cmin1=cmin
                  mcr=1
c     print *,'on attend'
                  GOTO 12
               else
c     print *,'on traite',w1,cmin1
                  mcr=0
               
                  if (w.gt.w1) then
                     
                     correction(ir)=1.d0/(1.d0+tau_mel*w/cmin)
                  
                  else
                     if (w1.ne.0.) then
                        
                        correction(ir)=1.d0/(1.d0+tau_mel*w1/cmin1)
                     else
                        correction(ir)=(1.d0)
                     endif
                  endif
                  w=(w-w1)*correction(ir)
                  correction(ir-1)=correction(ir)
               endif
               
            else
               if (w.ne.0.) then
                  
                  correction(ir)=1.d0/(1.d0+tau_mel*w/cmin)
                  w=w*correction(ir)
               else
                  correction(ir)=1.d0
               endif
            endif
            do ic=1,neq
               isto=stochio(ir,ic)
c     if (isto.ne.0) 
               ydot(ic)=ydot(ic)-isto*w
            enddo
 12         CONTINUE
         enddo
      else
         call f_chim (neq, t, y, ydot)
      endif
      end
 
      subroutine f_chim (neq, t, y, ydot)
      implicit none
      integer neq, ir,ii,ic,ia
      double precision t, y, ydot, isto
      dimension y(neq), ydot(neq)
#Pinclude(common_chimie.h)
   
      double precision w
      do ic=1,neq
         ydot(ic)=0
         correction(ic)=1.d0
      enddo
      do ir=1,nr
         w=-cw(ir)
         do ic=1,neq
            ia=activite(ir,ic)
c            if (ia.ne.0) w=w*y(ic)**ia
            do ii=1,ia
               w=w*y(ic)
            enddo
         enddo
         
         do ic=1,neq
            isto=stochio(ir,ic)
c            if (isto.ne.0) 
            ydot(ic)=ydot(ic)+isto*w
         enddo
      enddo
      return
     
      end

      subroutine jac_chim  (neq, t, y, ml, mu, pd, nrowpd)
      implicit none
      integer neq, ml,mu, nrowpd,ic,jc,ir,ii
      double precision t, y, pd,isto
      dimension y(neq), pd(nrowpd,neq)
#Pinclude(common_chimie.h)
      integer ia
  
      double precision w
    
      do ic=1,neq
         do jc=1,neq
            pd(ic,jc)=0
         enddo
      enddo
      do ir=1,nr
         do jc=1,neq
            if (activite(ir,jc).ne.0) then
               w=-cw(ir)
               do ic=1,neq
                  ia=activite(ir,ic)
                  if (ic.eq.jc) then
                     w=w*ia
                     ia=ia-1
                  endif
                  do ii=1,ia
                     w=w*y(ic)
                  enddo
                  
               enddo
               
               do ic=1,neq
                  isto=stochio(ir,ic)
                  if (isto.ne.0) 
     c                 pd(ic,jc)=pd(ic,jc)+isto*w
               enddo
            endif
         enddo
      enddo
      return
      end
      subroutine jac_chim_2  (neq, t, y, ml, mu, pd, nrowpd)
      implicit none
      integer neq, ml,mu, nrowpd,ic,jc,ir,ii
      double precision t, y, pd,isto
      dimension y(neq), pd(nrowpd,neq)
#Pinclude(common_chimie.h)
      integer ia
  
      double precision w
c      stop 'non code'
      call  jac_chim(neq, t, y, ml, mu, pd, nrowpd)
      do ic=1,neq
         do jc=1,neq
            pd(ic,jc)=0
         enddo
      enddo
      do ir=1,nr
      
c        cherchez le correctif pour chaque equation ...
         do jc=1,neq
            if (activite(ir,jc).ne.0) then
               w=-cw(ir)*correction(ir)
c               if (correction(ir).ne.1.) stop 'pb'
               do ic=1,neq
                  ia=activite(ir,ic)
                  if (ic.eq.jc) then
                     w=w*ia
                     ia=ia-1
                  endif
                  do ii=1,ia
                     w=w*y(ic)
                  enddo
                  
               enddo
               
               do ic=1,neq
                  isto=stochio(ir,ic)
                  if (isto.ne.0) 
     c                 pd(ic,jc)=pd(ic,jc)+isto*w
               enddo
            endif
         enddo
      enddo
      return
      end
