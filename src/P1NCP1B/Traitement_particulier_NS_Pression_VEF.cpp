/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Traitement_particulier_NS_Pression_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Assembleur_base.h>

Implemente_instanciable_sans_constructeur(Traitement_particulier_NS_Pression_VEF,"Traitement_particulier_NS_Pression_VEF",Traitement_particulier_NS_Pression);

Traitement_particulier_NS_Pression_VEF::Traitement_particulier_NS_Pression_VEF()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="Pression_porosite";
  */
}
// Description:
//
// Precondition:
// Parametre: Sortie& is
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
Sortie& Traitement_particulier_NS_Pression_VEF::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
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
Entree& Traitement_particulier_NS_Pression_VEF::readOn(Entree& is)
{
  return is;
}

inline int Traitement_particulier_NS_Pression_VEF::comprend_champ(const Motcle& mot) const
{
  if (mot == "Pression_porosite") return 1 ;
  return 0 ;
}

int Traitement_particulier_NS_Pression_VEF::a_pour_Champ_Fonc(const Motcle& mot,
                                                              REF(Champ_base)& ch_ref) const
{
  if (mot == "Pression_porosite")
    {
      ch_ref = ch_p;
      return 1 ;
    }
  return 0 ;
}

Entree& Traitement_particulier_NS_Pression_VEF::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;

  c_pression = 0 ;

  is >> motbidon ;

  Motcles les_mots(1);
  {
    les_mots[0] = "Pression_porosite";
  }


  if (motbidon == accouverte)
    {
      // is >> motlu;

      while(motlu != accfermee)
        {
          is >> motlu;
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                Cerr << " Lire Pression_porosite " << finl;
                const Zone_dis_base& zdis=mon_equation->inconnue().zone_dis_base();
                const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdis);
                //                  const Probleme_base& pb = mon_equation->probleme();
                const int nb_elem = zone_VEF.nb_elem() ;

                ch_p.associer_zone_dis_base(zdis);
                ch_p.nommer("Pression_porosite");
                ch_p.fixer_nb_comp(1);
                ch_p.fixer_nb_valeurs_nodales(nb_elem);
                ch_p.fixer_unite("Pa.m3/kg");
                c_pression = 1 ;
                champs_compris_.ajoute_champ(ch_p);
                break ;

              }
            default :
              {
                if (motlu == accfermee)
                  {
                    break ;
                  }
                else
                  {
                    Cerr << "Erreur dans la lecture de Traitement_particulier_Brech_VEF" << finl;
                    Cerr << "Les mots cles possibles sont : calcul_flux ou Richardson " << finl;
                    Cerr << "Vous avez lu :" << motlu << finl;
                    exit();
                    break;
                  }
              }
            }
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_Brech_VEF 1 ";
          Cerr << "On attendait une } et pas " << motlu << finl;
          exit();
        }
    }

  return is;
}


void Traitement_particulier_NS_Pression_VEF::post_traitement_particulier()
{

  if (c_pression == 1 ) post_traitement_particulier_calcul_pression() ;

}

void Traitement_particulier_NS_Pression_VEF::post_traitement_particulier_calcul_pression()
{
  const Zone_VEF& zvef=ref_cast(Zone_VEF, mon_equation->zone_dis().valeur());
  const DoubleVect& porosite_face = zvef.porosite_face();
  int i,comp;
  int nb_face = zvef.nb_faces();
  Operateur_Div divergence = mon_equation->operateur_divergence();
  Operateur_Grad gradient = mon_equation->operateur_gradient();
  SolveurSys solveur_pression_ = mon_equation->solveur_pression();

  DoubleTab& pression=mon_equation->pression().valeurs();
  DoubleTab& vitesse=mon_equation->vitesse().valeurs();
  DoubleTab gradP(vitesse);
  DoubleTab inc_pre(pression);
  DoubleTab secmem(pression);
  DoubleTab grad_temp(vitesse);

  inc_pre  = 0. ;
  secmem = 0. ;
  gradP = 0. ;
  grad_temp = 0. ;

  gradient.calculer(mon_equation->pression().valeurs(),gradP);

  //on veut BM-1Bt(psi*Pression)
  mon_equation->solv_masse().appliquer(gradP);

  for(i=0; i<nb_face; i++)
    {
      for (comp=0; comp<dimension; comp++)
        grad_temp(i,comp) = gradP(i,comp)/porosite_face(i);

    }
  //mon_equation->solv_masse().appliquer(grad_temp);
  divergence.calculer(grad_temp, secmem);

  secmem *= -1; // car div =-B
  // Correction du second membre d'apres les conditions aux limites :
  mon_equation->assembleur_pression().valeur().modifier_secmem(secmem);

  //solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre,mon_equation->pression().valeur());
  solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre);
  DoubleVect& la_pression_porosite = ch_p.valeurs();
  la_pression_porosite = pression;
  //  Cerr << " la_pression_porosite = pression " << la_pression_porosite  << finl;
  //   Cerr << " inc_pre " << inc_pre << finl;
  la_pression_porosite += inc_pre;



  //  Cerr << "la_pression " << mon_equation->pression().valeurs() << finl;
  //   Cerr << "ch_p.valeurs() " << ch_p.valeurs() << finl;
  // const Zone_VEF& zvef=ref_cast(Zone_VEF, mon_equation->zone_dis().valeur());
  //   const DoubleVect& porosite_face = zvef.porosite_face();
  //   int i,comp;
  //   int nb_face = zvef.nb_faces();
  //   Champ_Inc la_pression = mon_equation->pression();
  //   Champ_Inc la_vitesse =  mon_equation->vitesse();
  //   Operateur_Div divergence = mon_equation->operateur_divergence();
  //   Operateur_Grad gradient = mon_equation->operateur_gradient();
  //   SolveurSys solveur_pression_ = mon_equation->solveur_pression();

  //   DoubleTab& pression=la_pression.valeurs();
  //   DoubleTrav gradP(la_vitesse.valeurs());
  //   DoubleTrav inc_pre(pression);

  //  //  DoubleVect& inc_pre = ch_p.valeurs();
  // //   inc_pre  = 0. ;
  //   DoubleTrav secmem(pression);

  //   gradient.calculer(la_pression,gradP);
  //   // Cerr << "gradP " << gradP << finl;
  //   //on veut BM-1Bt(psi*Pression)
  //   mon_equation->solv_masse().appliquer(gradP);

  //   DoubleTrav grad_temp(la_vitesse.valeurs());
  //   for(i=0; i<nb_face; i++)
  //     {
  //       for (comp=0; comp<dimension; comp++)
  //         grad_temp(i,comp) = gradP(i,comp)/porosite_face(i);
  //     }

  //   divergence.calculer(grad_temp, secmem);
  //   secmem *= -1; // car div =-B
  //   solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre, la_pression.valeur());
  //   // Cerr << "la pression " << pression << finl;
  //   //Cerr << "inc_pre " << inc_pre << finl;

  //   DoubleVect& la_pression_porosite = ch_p.valeurs();
  //   la_pression_porosite = inc_pre ;
  //   Cerr << "la_pression_porosite" << la_pression_porosite << finl;
  //   Cerr << "ch_p.valeurs() " << ch_p.valeurs() << finl;
}
