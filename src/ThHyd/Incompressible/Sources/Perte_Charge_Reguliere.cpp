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

#include <Perte_Charge_Reguliere.h>
#include <Motcle.h>

//Add a constructor for initialization
Perte_Charge_Reguliere::Perte_Charge_Reguliere( void ) : Perte_Charge( )
{

  D_ = - 1e3;
  d_ = - 1e3;
  a_ = - 1e3;
  b_ = - 1e3;
  Cf_ =  -1e3;

  couronne_tube = -1e3; ;
  Cf_utilisateur = -1e3 ;
  xo = -1e3;
  yo = -1e3;
  zo = -1e3;
  dir[ 0 ] = -1e3 ;
  dir[ 1 ] = -1e3 ;
  dir[ 2 ] = -1e3 ;
  h0 = -1e3;
  h = -1e3 ;

}

// Description:
//    Lit les specifications d'une perte de charge reguliere
//    a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: mot cle inattendu, on attendait "X","Y" ou "Z"
// Exception: mot cle inattendu, on attendait "d"
// Exception: mot cle inattendu, on attendait "longitudinale" ou
//            "transversale"
// Effets de bord:
// Postcondition:
Entree& Perte_Charge_Reguliere::lire_donnees(Entree& is)
{
  int dimension = Objet_U::dimension ;

  for (int i =0; i<dimension; i++ ) dir[i] = 0 ;

  couronne_tube = 0 ;
  Cf_utilisateur = 0 ;

  Motcle motlu;
  Motcles les_motcles(6);
  les_motcles[0] = "X";
  les_motcles[1] = "Y";
  les_motcles[2] = "Z";
  les_motcles[3] = "A";
  les_motcles[4] = "B";
  les_motcles[5] = "CF";

  is >> motlu;
  if (motlu == "longitudinale")
    {
      is >> motlu;
      int rang = les_motcles.search(motlu);
      if (rang == -1)
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "On attendait l'un des mots cles" << les_motcles << finl;
          Cerr << "a la place de " << motlu << finl;
          Process::exit();
        }
      else
        {
          direction_perte_charge_ = rang;
          is >> D_;
          Cerr << "D_ " << D_ << finl;
        }
      d_ = D_;
    }
  else if (motlu == "transversale")
    {
      is >> motlu;
      int rang = les_motcles.search(motlu);
      if (rang == -1)
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "On attendait l'un des mots cles" << les_motcles << finl;
          Cerr << "a la place de " << motlu << finl;
          Process::exit();
        }
      else
        {
          direction_perte_charge_ = rang;
          is >> D_;
        }
      is >> motlu;
      if (motlu != "d")
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "On attendait le mot cle d a la place de " << motlu << finl;
          Process::exit();
        }
      else
        {
          is >> d_;
        }
    }
  else if (motlu == "couronne")
    {
      couronne_tube = 1 ;

      if(dimension!=2)
        {
          // Une Couronne en 2D seulement !
          Cerr << "Une couronne est en 2D seulement : en dim 3 Tube "<< finl;
          Process::exit();
        }
      is >> motlu;
      if(motlu!=Motcle("Origine"))
        {
          Cerr << "On attendait le mot cle ORIGINE " << finl;
          Process::exit();
        }

      is >> xo >> yo ;
      is >> motlu;
      if (motlu != "d")
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "On attendait le mot cle d a la place de " << motlu << finl;
          Process::exit();
        }
      else
        {
          is >> d_;
          D_ = d_;
        }
      direction_perte_charge_ = 4;
      dir[0] = 1 ;
      dir[1] = 1 ;
    }
  else if (motlu == "tube")
    {
      couronne_tube = 1 ;
      if(dimension!=3)
        {
          // Un tube en 3D seulement !
          Cerr << "Un tube est en 3D seulement : en dim 2 couronne "<< finl;
          Process::exit();
        }
      is >> motlu;
      if(motlu!=Motcle("Origine"))
        {
          Cerr << "On attendait le mot cle ORIGINE " << finl;
          Process::exit();
        }
      is >> xo >> yo >> zo;
      is >> motlu;
      if(motlu!=Motcle("DIR"))
        {
          Cerr << "On attendait la direction du tube, mot cle : DIR " << finl;
          Process::exit();
        }
      int idir;
      is >> idir;
      switch(idir)
        {
        case 0:
          dir[0]=0;
          dir[1]=dir[2]=1;
          h0=xo;
          break;
        case 1:
          dir[1]=0;
          dir[0]=dir[2]=1;
          h0=yo;
          break;
        case 2:
          dir[2]=0;
          dir[1]=dir[0]=1;
          h0=zo;
          break;
        default:
          h0=-1;
          Cerr << "DIR vaut 0 pour tube // OX ; 1 pour un tube // OY et 2 pour un tube // OZ " << finl;
          Process::exit();
        }
      is >> motlu;
      if (motlu != "d")
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "On attendait le mot cle d a la place de " << motlu << finl;
          Process::exit();
        }
      else
        {
          is >> d_;
          D_ = d_;
        }
      direction_perte_charge_ = 4;
    }
  else
    {
      Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
      Cerr << "On attendait le mot cle longitudinale ou transversale ou couronne(en 2D) ou tube(en 3D)" << finl;
      Cerr << "a la place de " << motlu << finl;
      Process::exit();
    }

  // Lecture obligatoire de a_ et b_ :
  int err=0;
  is >> motlu;
  if (motlu == "A")
    is >> a_;
  else
    err=1;
  if (err)
    {
      if (motlu == "CF")
        {
          is >> Cf_;
          Cf_utilisateur = 1;
          Cerr << " Cf_ " << Cf_ << finl;
        }
    }
  else
    {
      is >> motlu;
      if (motlu == "B")
        is >> b_;
      else
        err=1;

    }
  if (err)
    {
      if (!Cf_utilisateur)
        {
          Cerr << "Erreur a la lecture des donnees de Perte_Charge_Reguliere" << finl;
          Cerr << "Il faut desormais specifier les valeurs de la loi de la perte de charge :" << finl;
          Cerr << "Exemple : Perte_Charge_Reguliere longitudinale X val A val B val" << finl;
          Cerr << "Avec A et B, pour une loi de perte de charge en A.Re**(-B)" << finl;
          Cerr << "Pour la loi de Blasius : A=0.316, B=0.25" << finl;
          Process::exit();
        }
    }
  else
    Cerr << a_ <<  " " << b_ << finl;

  return is;
}
