# -*- coding: utf-8 -*-
"""
Author : Elie Saikali

A script that helps finding out the source term knowing the analytical solution
Equation of interest : -div(K grad T ) = Q

Attention : use python3 to compile, we use SYMPY here !!!
"""

from sympy import *
init_printing(use_unicode=True)

# Test case 2 
# https://hal.archives-ouvertes.fr/hal-00605548/document

# All starts here : define the symbols
x,y,e = symbols('x y e')

# Here, the analytical solution is
T=sin(pi*x)**2*sin(pi*y)**2

# Compute the temperature gradients
gradTx=diff(T,x)
gradTy=diff(T,y)

# Components of the tensor
kxx= y**2+e*x**2
kyy=x**2+e*y**2
kxy=kyx=-(1-e)*x*y

# terme K grad T
kGTx=kxx*gradTx+kxy*gradTy
kGTy=kyx*gradTx+kyy*gradTy

# pretty printing, thanks SYMPY !!!
print("The term K grad T is composed of ")
pprint(kGTx)
pprint(kGTy)

# terme - div(K grad T )
DkGT= -diff(kGTx,x)-diff(kGTy,y)
print("Source term is : ")
# e is eta, here is 1.e-2
# So, substitute its value
DkGT=DkGT.subs(e,1.e-2)
pprint (DkGT)

# Generates a LaTeX code to put in repport 
print (latex(DkGT))



