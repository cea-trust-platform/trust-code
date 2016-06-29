/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Echange_global_impose.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_global_impose_included
#define Echange_global_impose_included




#include <Echange_impose_base.h>
#include <Ref_Zone_dis_base.h>
#include <Ref_Milieu_base.h>




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_global_impose
//    Cette classe represente le cas particulier de la classe
//    Echange_impose_base ou l'echange de chaleur total est calcule grace au
//    coefficient d'echange de chaleur global fourni par l'utilisateur.
//
//     h_t   : coefficient d'echange total
//     h_imp_: coefficient d'echange global (donnee utilisateur)
//    On a: h_t = h_imp
//    Les champs h_imp et T_ext sont uniformes
//
// .SECTION voir aussi
//    Echange_impose_base Echange_externe_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_global_impose : public Echange_impose_base
{

  Declare_instanciable(Echange_global_impose);
};




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_externe_impose:
//    Cette classe represente le cas particulier de la classe
//    Echange_impose_base ou l'echange de chaleur total est calcule grace
//    au coefficient d'echange de chaleur a la paroi fourni par l'utilisateur.
//
//     h_total  : coefficient d'echange total
//     h_imp    : coefficient d'echange a la paroi (donnee utilisateur)
//     e/lambda : coefficient d'echange interne
//                avec : lambda, conductivite dans le fluide
//                       e = d,  en laminaire
//                       e = d',  en turbulent
//     (ou d' est la distance equivalente calculee lors de l'application
//       des lois de paroi)
//          1/h_total = (1/h_imp) + (e/lambda)
//    Ce n'est pas la classe Echange_externe_impose qui gere le calcul
//    de h_total. La classe fournit seulement h_imp et c'est l'evaluateur
//    de flux diffusif qui calcule les termes e/lambda et h_total.
//    Les classes Echange_global_impose et Echange_externe_impose ont
//    exactement la meme interface; la classe Echange_externe_impose
//    sert donc seulement a signaler a l'evaluateur de flux diffusif
//    qu'il doit calculer un coefficient d'echange total.
// .SECTION voir aussi
//    Echange_impose_base Echange_global_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_externe_impose  : public Echange_impose_base
{

  Declare_instanciable(Echange_externe_impose);
  virtual int compatible_avec_discr(const Discretisation_base& ) const;
  void verifie_ch_init_nb_comp();
};


#endif
