/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Cond_lim_utilisateur_base_included
#define Cond_lim_utilisateur_base_included

#include <Cond_lim_base.h>
#include <Motcle.h>

class Cond_lim;

/*! @brief classe Cond_lim_utilisateur_base: Les classes heritant de cette classe sont des classes utilisaturs
 *
 *     elles vont transformer le jdd
 *     la cl_utilisateur est typee est passe dans la methode complement(ajout)
 *    Dans le jdd on aviat bord cl_util champ....
 *    tout se passe comme si on n avait pas cl_util mais ajout champ...
 *
 */
class Cond_lim_utilisateur_base  : public Cond_lim_base
{
  Declare_base_sans_constructeur(Cond_lim_utilisateur_base);
public:
  void lire(Entree&,Equation_base&,const Nom& nom_bord);
  virtual void complement(Nom&);

  inline int compatible_avec_eqn(const Equation_base&) const override
  {
    Cerr<<"we should not be here"<<(int)__LINE__<<__FILE__ <<finl;
    Process::exit();
    return 0;
  }

  Cond_lim& la_cl();
  Cond_lim_utilisateur_base();
  int is_pb_rayo();

  virtual void ecrire(const Nom&);
protected :
  Cond_lim* la_cl_;
  REF(Equation_base) mon_equation;
  Nom nom_bord_;
};

class cl_timp: public Cond_lim_utilisateur_base
{
  Declare_instanciable(cl_timp);
public:
  void complement(Nom& ) override;
};

class paroi_adiabatique: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_adiabatique);
public:
  void complement(Nom& nom ) override;
};

class paroi_flux_impose: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_flux_impose);
public:
  void complement(Nom& nom) override;
};

class paroi_contact: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_contact);
public:
  void complement(Nom& nom) override;
private:
  Nom nom_autre_pb,nom_autre_bord;
};

class paroi_contact_rayo: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_contact_rayo);
public:
  void complement(Nom& nom) override;
private:
  Nom nom_autre_pb, nom_autre_bord;
  Motcle type_rayo;
};

class paroi_contact_fictif: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_contact_fictif);
public:
  void complement(Nom& nom) override;
private:
  Nom nom_autre_pb, nom_autre_bord;
  double conduct_fictif = -100., ep_fictif = -100.;
};

class paroi_contact_fictif_rayo: public Cond_lim_utilisateur_base
{
  Declare_instanciable(paroi_contact_fictif_rayo);
public:
  void complement(Nom& nom) override;
private:
  Nom nom_autre_pb, nom_autre_bord;
  Motcle type_rayo;
  double conduct_fictif = -100., ep_fictif = -100.;
};

#endif /* Cond_lim_utilisateur_base_included */
