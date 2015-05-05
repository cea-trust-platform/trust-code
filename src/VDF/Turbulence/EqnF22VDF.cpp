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
// File:        EqnF22VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <EqnF22VDF.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Modele_turbulence_hyd_K_Eps_V2_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>

Implemente_instanciable(EqnF22VDF,"EqnF22VDF",EqnF22base);


// Description:
//    Imprime le type de l'equation sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& EqnF22VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


// Description:
//    Lit les specifications d'une equation de transport K-epsilon
//    a partir d'un flot d'entree.
//    Controle dynamique du type du terme source.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& EqnF22VDF::readOn(Entree& s )
{
  KEps=mon_modele->K_Eps();

  //Extraction de la viscosite moleculaire
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,le_fluide.valeur());
  ch_visco_cin = fluide.viscosite_cinematique();

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur().valeur()))
    visco_unif = 1;
  else
    visco_unif = 0;

  return EqnF22base::readOn(s);
}


// Description:
//    Calcule le champ F22 en VDF.
// Precondition:
// Parametre:
// Retour:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void EqnF22VDF::resoudre()
{
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF, zone_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis().valeur());
  const DoubleVect& volumes = zone_VDF.volumes();
  int nb_elem = zone_VDF.nb_elem();
  DoubleTab scdmembref22(nb_elem);

  const DoubleTab& v2 = mon_modele->eqn_V2().inconnue().valeurs();
  DoubleTab& f22 = le_champ_F22.valeurs();
  const DoubleTab& K_eps = KEps->valeurs();
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence());
  const Equation_base& eq_hydraulique = mod_turb.equation();
  const DoubleTab& vit = eq_hydraulique.inconnue().valeurs();
  const  Champ_Face& vit_bord = ref_cast(Champ_Face,eq_hydraulique.inconnue().valeur());
  const DoubleTab& visco_turb = mod_turb.viscosite_turbulente().valeurs();

  int elem;
  //Calcul du terme de production P
  DoubleTab P(nb_elem);
  DoubleTab S(nb_elem);
  DoubleTab T(nb_elem);
  DoubleTab L(nb_elem);

  T = 1.;
  L = 1.;

  //   if (axi)
  //     {
  //       //calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
  //       Cerr << " axi n'est pas code " << finl ;
  //       exit() ;
  //     }
  //   else {
  //     calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps,vit,vit_bord,visco_turb);
  //   }


  //Extraction de la viscosite moleculaire
  const Fluide_Incompressible& le_fluide_inc = ref_cast(Fluide_Incompressible, eq_hydraulique.milieu());
  const Champ_Don& ch_viscocin = le_fluide_inc.viscosite_cinematique();
  DoubleTab& tab_visco = ref_cast_non_const(DoubleTab,ch_viscocin->valeurs());
  double visco=-1;
  int l_unif;

  if (sub_type(Champ_Uniforme,ch_viscocin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    {
      // visco = tab_visco[j];
      l_unif = 0;
    }
  // HOU cast en dur faux !!!!
  // Modele_turbulence_hyd_K_Eps_V2_VDF& le_modele = (Modele_turbulence_hyd_K_Eps_V2_VDF&) mon_modele;
  Modele_turbulence_hyd_K_Eps_V2_VDF& le_modele = ref_cast(Modele_turbulence_hyd_K_Eps_V2_VDF, mon_modele.valeur());

  le_modele.calculer_Sij(zone_VDF,zcl_VDF,S,vit,ref_cast_non_const(Champ_Face,vit_bord));

  // Calcul production
  P = 0. ;
  for (int i=0; i<nb_elem; i++)
    {
      P(i) += 2.*visco_turb(i) ;
      P(i) *= S(i) ;
    }

  le_modele.Calculer_Echelle_T(K_eps,v2,S,tab_visco,visco,l_unif,T);
  //Cerr << " dans eqnf22 T= " << T << finl;
  le_modele.Calculer_Echelle_L(K_eps,v2,S,tab_visco,visco,l_unif,L);
  //Cerr << " dans eqnf22 L= " << L << finl;

  // on assemble la matrice du Laplacien
  matF22.clean();
  scdmembref22 = 0. ;
  terme_diffusif.valeur().contribuer_a_avec(f22,matF22);

  // Cerr << "la matrice du laplacien avant: " << finl;
  //matF22.imprimer(Cerr);

  // on ajoute f/L2
  for (int j=0; j<matF22.ordre(); j++)
    {
      matF22(j,j) +=  volumes(j)/L(j) ;
    }

  double C_delta = 5. ;

  //calcul du second membre
  for (elem=0; elem<nb_elem; elem++)
    {
      if (K_eps(elem,0) > 1.e-10)
        {
          double gamma  =  C1/T(elem)*(0.6666-v2(elem)/K_eps(elem,0)) + C2*P(elem)/K_eps(elem,0)         ;
          double delta     =   C_delta*v2(elem)/ (K_eps(elem,0) * T(elem) ) ;
          scdmembref22(elem) = gamma+delta ;
        }
      else
        {
          scdmembref22(elem)  = (C1/T(elem))*0.66;
        }

    }

  terme_diffusif.valeur().contribuer_au_second_membre(scdmembref22);

  for ( elem=0; elem<nb_elem; elem++)
    scdmembref22(elem) *= volumes(elem)/L(elem) ;

  // Cerr << "le secmem du laplacien apres: " << scdmembref22 << finl;

  solveur.resoudre_systeme(matF22,scdmembref22,f22);

}



