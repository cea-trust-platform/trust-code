/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        MIt_Sou_EF.h
// Directory:   $TRUST_ROOT/src/EF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MIt_Sou_EF_H
#define MIt_Sou_EF_H

#include <Iterateur_Source_EF_base.h>
#include <Les_Cl.h>
#include <Zone_EF.h>
#include <Champ_Uniforme.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS Iterateur_Source_EF_Som
//
//////////////////////////////////////////////////////////////////////////////

#define Declare_It_Sou_EF(_TYPE_) \
class It_Sou_EF(_TYPE_) : public Iterateur_Source_EF_base\
{\
   \
  Declare_instanciable(It_Sou_EF(_TYPE_));\
   \
public:\
   \
   inline It_Sou_EF(_TYPE_)(const It_Sou_EF(_TYPE_)&);\
   inline Evaluateur_Source_EF& evaluateur();\
   \
   DoubleTab& calculer(DoubleTab& ) const;\
   DoubleTab& ajouter(DoubleTab& ) const;\
   inline void completer_();\
   inline int impr(Sortie&) const;\
   \
protected:\
   \
   _TYPE_ evaluateur_source_elem;\
   \
   DoubleTab& ajouter_elems_standard(DoubleTab& ) const;\
   DoubleTab& ajouter_elems_standard(DoubleTab& ,int ) const;\
   \
   int nb_elems;\
   mutable DoubleVect bilan;\
   mutable double coef;\
   \
};\
\
inline It_Sou_EF(_TYPE_)::\
   It_Sou_EF(_TYPE_)(const It_Sou_EF(_TYPE_)& iter) \
   \
   : Iterateur_Source_EF_base(iter),\
     evaluateur_source_elem(iter.evaluateur_source_elem),\
     nb_elems(iter.nb_elems)				 \
   \
{} \
\
inline void It_Sou_EF(_TYPE_)::completer_(){\
  nb_elems=la_zone->zone().nb_elem_tot();		 \
}\
\
inline Evaluateur_Source_EF& It_Sou_EF(_TYPE_)::evaluateur() \
{\
   Evaluateur_Source_EF& eval = (Evaluateur_Source_EF&) evaluateur_source_elem;\
   return eval;\
} \
class __dummy

#define Implemente_It_Sou_EF(_TYPE_) \
Implemente_instanciable(It_Sou_EF(_TYPE_),"Iterateur_Source_EF_Som",Iterateur_Source_EF_base);\
   \
Sortie& It_Sou_EF(_TYPE_)::printOn(Sortie& s ) const {\
   return s << que_suis_je() ;\
}\
Entree& It_Sou_EF(_TYPE_)::readOn(Entree& s ) {\
   return s ;\
}\
DoubleTab& It_Sou_EF(_TYPE_)::ajouter(DoubleTab& resu) const\
{\
   ((_TYPE_&) (evaluateur_source_elem)).mettre_a_jour( );\
   \
   assert(resu.nb_dim() < 3);\
   int ncomp=1;\
   if (resu.nb_dim() == 2)\
      ncomp=resu.dimension(1);   \
   \
   bilan.resize(ncomp);\
   bilan=0;\
   coef=1;\
   if (equation_divisee_par_rho())\
   {\
      const Milieu_base& milieu = la_zcl->equation().milieu();\
      const Champ_Don& rho = milieu.masse_volumique();\
      if ( sub_type(Champ_Uniforme,rho.valeur()))\
         coef = rho(0,0);\
      else\
      {\
         Cerr << "Cas non prevu dans It_Sou_EF(_TYPE_)::ajouter(DoubleTab& resu) const" << finl;\
	 exit();\
      }\
   }\
   if (equation_divisee_par_rho_cp())\
   {\
      const Milieu_base& milieu = la_zcl->equation().milieu();\
      const Champ_Don& rho = milieu.masse_volumique();\
      const Champ_Don& Cp = milieu.capacite_calorifique();\
      if ( (sub_type(Champ_Uniforme,rho.valeur())) && (sub_type(Champ_Uniforme,Cp.valeur())))\
         coef = rho(0,0)*Cp(0,0);\
      else\
      {\
         Cerr << "Cas non prevu dans It_Sou_EF(_TYPE_)::ajouter(DoubleTab& resu) const" << finl;\
	 exit();\
      }\
   }\
   if( ncomp == 1) \
   {\
      ajouter_elems_standard(resu) ;\
   }\
   else \
   {\
      ajouter_elems_standard(resu, ncomp) ;\
   }\
   return resu;\
}\
DoubleTab& It_Sou_EF(_TYPE_)::ajouter_elems_standard(DoubleTab& resu) const\
{\
  const IntTab& elems= la_zone->zone().les_elems() ;		\
  const DoubleTab& IPhi_thilde= la_zone->IPhi_thilde();		\
  int nb_som_elem=la_zone->zone().nb_som_elem();		\
  for (int num_elem=0; num_elem<nb_elems; num_elem++)	\
   {\
      double source_0 = evaluateur_source_elem.calculer_terme_source_standard(num_elem);\
      for (int i=0;i<nb_som_elem;i++)					\
	{					\
	  double source=source_0;					\
	  source*=IPhi_thilde(num_elem,i);	\
	  bilan(0) +=  coef * source;		\
	  resu(elems(num_elem,i))+=source ;	\
	}\
   }\
   return resu;\
}\
DoubleTab& It_Sou_EF(_TYPE_)::ajouter_elems_standard(DoubleTab& resu,int ncomp) const \
{\
   DoubleVect source(ncomp);\
   for (int num_elem=0; num_elem<nb_elems; num_elem++) { \
     Cerr<<"It_Sou_EF(_TYPE_)::ajouter_elems_standard(DoubleTab& resu,int ncomp) const  non code"<<finl; \
     abort() ; \
      evaluateur_source_elem.calculer_terme_source_standard(num_elem,source);\
      for (int k=0; k<ncomp; k++)\
      {\
	 resu(num_elem,k) += source(k);\
	 bilan(k) +=  coef * source(k);\
      }\
   }\
   return resu;\
}\
DoubleTab& It_Sou_EF(_TYPE_)::calculer(DoubleTab& resu) const\
{\
   resu=0;\
   return ajouter(resu);\
}\
int It_Sou_EF(_TYPE_)::impr(Sortie& os) const\
{\
   for(int k=0; k<bilan.size(); k++)\
   {\
      bilan(k)=Process::mp_sum(bilan(k));\
      if(je_suis_maitre()) os << " : " << bilan(k) << finl;\
   }\
   return 1;\
} \
class __dummy2
#endif
