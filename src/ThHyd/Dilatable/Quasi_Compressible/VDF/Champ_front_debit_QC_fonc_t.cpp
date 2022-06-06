/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Champ_front_debit_QC_fonc_t.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Motcle.h>
#include <Zone_VDF.h>
#include <Interprete.h>

Implemente_instanciable(Champ_front_debit_QC_fonc_t,"Champ_front_debit_QC_VDF_fonc_t",Ch_front_var_instationnaire_indep);
// XD Champ_front_debit_QC_VDF_fonc_t front_field_base Champ_front_debit_QC_VDF_fonc_t 0 This keyword is used to define a flow rate field for quasi-compressible fluids in VDF discretization. The flow rate could be constant or time-dependent.
// XD attr dimension int  dim 0 Problem dimension
// XD attr liste bloc_lecture liste 0 List of the mass flow rate values [kg/s/m2] with the following syntaxe: { val1 ... valdim } where val1 ... valdim are constant or function of time.
// XD attr moyen chaine moyen 1 Option to use rho mean value
// XD attr pb_name chaine pb_name 0 Problem name



// Description:
//    Impression sur un flot de sortie au format:
//    taille
//    valeur(0) ... valeur(i)  ... valeur(taille-1)
// Precondition:
// Parametre: Sortie& os
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
Sortie& Champ_front_debit_QC_fonc_t::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}

// Description:
//    Lecture a partir d'un flot d'entree au format:
//    nombre_de_composantes
//    moyenne moyenne(0) ... moyenne(nombre_de_composantes-1)
//    moyenne amplitude(0) ... amplitude(nombre_de_composantes-1)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef inconnu a cet endroit
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_front_debit_QC_fonc_t::readOn(Entree& is)
{
  Cerr<<"Champ_front_debit_QC_VDF_fonc_t usage : dim { val1 .. valdim } [ moyen ] nom_pb"<<finl;
  ismoyen=0;
  int dim;
  is >> dim;
  Motcle motlu;
  Motcles les_mots(2);
  les_mots[0]="{";
  les_mots[1]="}";
  is >> motlu;
  if (motlu != les_mots[0])
    {
      Cerr << "Erreur a la lecture d'un Champ_front_debit_QC_VDF_fonc_t" << finl;
      Cerr << "On attendait { a la place de " << motlu << finl;
      exit();
    }
  fixer_nb_comp(dim);
  f_debit_t.dimensionner(dim);

  for (int i = 0; i<dim; i++)
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      f_debit_t[i].setNbVar(1);
      f_debit_t[i].setString(tmp);
      f_debit_t[i].addVar("t");
      f_debit_t[i].parseString();
      Cerr << "Interpretation of function " << tmp << " Ok" << finl;
    }
  is >> motlu;
  if (motlu != les_mots[1])
    {
      Cerr << "Erreur a la lecture d'un Champ_front_debit_QC_VDF_fonc_t" << finl;
      Cerr << "On attendait } a la place de " << finl;
      exit();
    }
  Nom nom_pb;
  is>>nom_pb;
  Motcle nom;
  nom=nom_pb;
  if (nom=="moyen")
    {
      ismoyen=1;
      is >> nom_pb;
    }
  Objet_U& ob1=Interprete::objet(nom_pb);
  const Probleme_base& pb=ref_cast(Probleme_base,ob1);
  fluide=ref_cast(Fluide_Quasi_Compressible,pb.equation(0).milieu());
  return is;
}


// Description:
//    Pas code !!
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Champ_front_debit_QC_fonc_t::affecter_(const Champ_front_base& ch)
{
  return *this;
}

// Description:
//     Mise a jour du temps
//     et retirage aleatoire des valeurs du bruit.
// Precondition:
// Parametre: double tps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_debit_QC_fonc_t::mettre_a_jour(double tps)
{


  const Frontiere& front=la_frontiere_dis->frontiere();
  int nb_faces=front.nb_faces();
  DoubleTab& tab=valeurs();
  DoubleTab& Debit=valeurs_au_temps(tps);
  int dim=nb_comp();
  const Zone_VDF& la_zone_VDF = ref_cast(Zone_VDF,zone_dis());
  const IntTab& face_voisins=la_zone_VDF.face_voisins();
  const Front_VF& front_vf=ref_cast(Front_VF,la_frontiere_dis.valeur());
  int ndeb = front_vf.num_premiere_face();
  int nfin = ndeb + nb_faces;
  const DoubleTab& tab_rhonp1P0 =fluide.valeur().loi_etat()->rho_np1();
  if (ismoyen==0)

    for (int num_face=ndeb; num_face<nfin; num_face++)
      {
        int n0 = face_voisins(num_face, 0);

        if (n0 == -1)
          n0 = face_voisins(num_face, 1);
        for (int ori=0; ori<dim; ori++)
          {
            f_debit_t[ori].setVar("t",tps);
            Debit(num_face-ndeb,ori)=f_debit_t[ori].eval();
            tab(num_face-ndeb,ori)=Debit(num_face-ndeb,ori)/tab_rhonp1P0(n0);
          }
      }
  else
    {
      int num_face;
      double rho_moy=0,S=0,s;
      const DoubleVect& surface=la_zone_VDF.face_surfaces();
      for ( num_face=ndeb; num_face<nfin; num_face++)
        {
          int n0 = face_voisins(num_face, 0);

          if (n0 == -1)
            n0 = face_voisins(num_face, 1);
          s=surface(num_face);
          S+=s;
          rho_moy+=s*tab_rhonp1P0(n0);
        }
      S = mp_sum(S);
      rho_moy = mp_sum(rho_moy);
      rho_moy/=S;
      for ( num_face=ndeb; num_face<nfin; num_face++)
        for (int ori=0; ori<dim; ori++)
          {
            f_debit_t[ori].setVar("t",tps);
            Debit(num_face-ndeb,ori)=f_debit_t[ori].eval();
            tab(num_face-ndeb,ori)=Debit(num_face-ndeb,ori)/rho_moy;
          }
    }

}

double Champ_front_debit_QC_fonc_t::valeur_au_temps(double tps, int som, int k) const
{
  Parser_U& f_debit_tk=f_debit_t[k];
  f_debit_tk.setVar("t",tps);
  return f_debit_tk.eval();
}


