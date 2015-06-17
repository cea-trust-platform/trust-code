# ////////////////////////////////////////////////////////////////////
#
# Profils de temperature pour un probleme de conduction 1D
# instationnaire.
#
# La temperature est imposee en x=0 (T0) et le flux est impose en x=L
# (phiL).
#
# ////////////////////////////////////////////////////////////////////


# ====================================================================
# Definition des fonctions
# ====================================================================

# --------------------------------------------------------------------
# Constantes
# --------------------------------------------------------------------

L     = 0.05
k     = 0.5
rho   = 0.5
Cp    = 1000.
alpha = k/(rho*Cp)
T0    = 0.
phiL  = 100.
a     = 20.


# --------------------------------------------------------------------
# Profils de temperature
# --------------------------------------------------------------------

T(x,t) = T0 - phiL/k * x + a * sin(pi*x/(2.*L)) * exp(-alpha*(pi/(2.*L))*(pi/(2.*L))*t)
phi0(t) = phiL - k * a * pi/(2.*L) * exp(-alpha*(pi/(2.*L))*(pi/(2.*L))*t)


# --------------------------------------------------------------------
# Flux et températures pariétaux
# --------------------------------------------------------------------

print ""
print "phi0(0)   = ", phi0(0.)
print "phi0(0.5) = ", phi0(0.5)
print "phi0(1)   = ", phi0(1.)
print "phi0(2)   = ", phi0(2.)
print "phi0(10)  = ", phi0(10.)
print ""
print "TL(0)   = ", T(L,0.)
print "TL(0.5) = ", T(L,0.5)
print "TL(1)   = ", T(L,1.)
print "TL(2)   = ", T(L,2.)
print "TL(10)  = ", T(L,10.)


# ====================================================================
# Graphes
# ====================================================================

set term epslatex color

set xrange[0:L]

set key left

set xlabel "$x$"

set ylabel "$T$"

set output "profilsTInstat.eps"

plot T(x,0.) title "$t=0$" lw 2, T(x,0.5) title "$t=0.5$" lw 2, T(x,1.) title "$t=1$" lw 2, T(x,2.) title "$t=2$" lw 2, T(x,10.) title "$t=10$" lw 2
set term table
set output "ref_gnuplot.dat"
plot T(x,0.) title "$t=0$" lw 2, T(x,0.5) title "$t=0.5$" lw 2, T(x,1.) title "$t=1$" lw 2, T(x,2.) title "$t=2$" lw 2, T(x,10.) title "$t=10$" lw 2
