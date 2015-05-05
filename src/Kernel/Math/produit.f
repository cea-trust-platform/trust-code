C////////////////////////////////////////////////////////////
C//
C// File:	produit.f
C// Directory:	$TRUST_ROOT/src/Kernel/Math
C//
C////////////////////////////////////////////////////////////

      SUBROUTINE PRODUIT(x,resu,n,coeff,tab1,tab2)
C  
      INTEGER i,k,j,n
      REAL*8 t,aij,xx
      INTEGER tab1(0:1),tab2(0:1)
      REAL*8 x(0:1),resu(0:1),coeff(0:1)
C
      DO 10 i=0,n-1
         xx = x(i)
         t = coeff(tab1(i)-1) * xx
         DO 20 k=tab1(i),tab1(i+1)-2
	    j = tab2(k)-1
	    aij = coeff(k)
	    t = t + aij * x(j)
	    resu(j) = resu(j) + aij * xx
20       CONTINUE
         resu(i) = resu(i) + t
10    CONTINUE
      RETURN
      END

      SUBROUTINE PRODUIT1(x,resu,n,coeff,tab1,tab2)
C 
      INTEGER i,k,j,n
      REAL*8 t,aij
      INTEGER tab1(0:1),tab2(0:1)
      REAL*8 x(0:1),resu(0:1),coeff(0:1)
C
      DO 10 i=0,n-1
         t = coeff(tab1(i)-1) * x(i)
         DO 20 k=tab1(i),tab1(i+1)-2
	    j = tab2(k)-1
	    aij = coeff(k)
	    t = t + aij * x(j)
	    resu(j) = resu(j) + aij * x(i)
20       CONTINUE
         resu(i) = resu(i) + t
10    CONTINUE
      RETURN
      END
      
      SUBROUTINE PRODUIT2(x,resu,n,coeff,tab1,tab2)
C
      INTEGER i,k,j,n,m,l
      REAL*8 t,aij
      INTEGER tab1(*),tab2(*)
      REAL*8 x(*),resu(*),coeff(*)
C
      m = tab1(1)
      DO 10 i=1,n
         l = m
         t = coeff(l) * x(i)
	 m = tab1(i+1)
         DO 20 k=l+1,m-1
	    j = tab2(k)
	    aij = coeff(k)
	    t = t + aij * x(j)
	    resu(j) = resu(j) + aij * x(i)
20       CONTINUE
         resu(i) = resu(i) + t
10    CONTINUE
      RETURN
      END
