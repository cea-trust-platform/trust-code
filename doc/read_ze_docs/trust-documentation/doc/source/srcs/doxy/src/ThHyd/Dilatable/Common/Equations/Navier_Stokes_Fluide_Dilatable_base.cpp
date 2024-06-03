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

#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Schema_Temps_base.h>
#include <Discret_Thyd.h>

Implemente_base(Navier_Stokes_Fluide_Dilatable_base,"Navier_Stokes_Fluide_Dilatable_base",Navier_Stokes_std);

Sortie& Navier_Stokes_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Navier_Stokes_std::printOn(is);
}

Entree& Navier_Stokes_Fluide_Dilatable_base::readOn(Entree& is)
{
  Navier_Stokes_std::readOn(is);
  divergence.set_description((Nom)"Mass flow rate=Integral(rho*u*ndS) [kg.s-1]");
  return is;
}

int Navier_Stokes_Fluide_Dilatable_base::impr(Sortie& os) const
{
  Navier_Stokes_Fluide_Dilatable_Proto::impr_impl(*this,os);
  return Navier_Stokes_std::impr(os);
}

/*! @brief Appel Equation_base::preparer_calcul() Assemblage du solveur pression et
 *
 *      initialisation de la pression.
 *
 * @return (int) renvoie toujours 1
 */
int Navier_Stokes_Fluide_Dilatable_base::preparer_calcul()
{
  return Navier_Stokes_std::preparer_calcul();
}

/*! @brief Complete l'equation base, associe la pression a l'equation,
 *
 *     complete la divergence, le gradient et le solveur pression.
 *     Ajout de 2 termes sources: l'un representant la force centrifuge
 *     dans le cas axi-symetrique,l'autre intervenant dans la resolution
 *     en 2D axisymetrique
 *
 */
void Navier_Stokes_Fluide_Dilatable_base::completer()
{
  Cerr<<"Navier_Stokes_std::completer"<<finl;
  Navier_Stokes_std::completer();
  Cerr<<"Unknown field type : " << inconnue()->que_suis_je() << finl;
  Cerr<<"Unknown field name : " << inconnue()->le_nom() << finl;
  Cerr<<"Equation type : " << inconnue()->equation().que_suis_je() << finl;
}

const Champ_Don& Navier_Stokes_Fluide_Dilatable_base::diffusivite_pour_transport() const
{
  return le_fluide->viscosite_dynamique();
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::diffusivite_pour_pas_de_temps() const
{
  return le_fluide->viscosite_cinematique();
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::vitesse_pour_transport() const
{
  return la_vitesse;
}

const Champ_base& Navier_Stokes_Fluide_Dilatable_base::get_champ(const Motcle& nom) const
{
  if (nom=="rho_u")
    return rho_la_vitesse().valeur();
  try
    {
      return Navier_Stokes_std::get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }

  try
    {
      return milieu().get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  throw Champs_compris_erreur();
}

bool Navier_Stokes_Fluide_Dilatable_base::initTimeStep(double dt)
{
  DoubleTab& tab_vitesse=inconnue().valeurs();
  Fluide_Dilatable_base& fluide_dil=ref_cast(Fluide_Dilatable_base,le_fluide.valeur());
  const DoubleTab& tab_rho = fluide_dil.rho_discvit();
  DoubleTab& rhovitesse = rho_la_vitesse_.valeurs(); // will be filled
  rho_vitesse_impl(tab_rho,tab_vitesse,rhovitesse);

  return  Navier_Stokes_std::initTimeStep(dt);
}

void Navier_Stokes_Fluide_Dilatable_base::discretiser()
{
  Navier_Stokes_std::discretiser();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  dis.vitesse(schema_temps(), domaine_dis(), rho_la_vitesse_);
  rho_la_vitesse_.nommer("rho_u");
  rho_la_vitesse_.valeur().nommer("rho_u");
}

DoubleTab& Navier_Stokes_Fluide_Dilatable_base::derivee_en_temps_inco(DoubleTab& vpoint)
{
  return Navier_Stokes_Fluide_Dilatable_Proto::derivee_en_temps_inco_impl(*this,vpoint);
}

void Navier_Stokes_Fluide_Dilatable_base::assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  Navier_Stokes_Fluide_Dilatable_Proto::assembler_impl(mat_morse,present,secmem);
}

void Navier_Stokes_Fluide_Dilatable_base::assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  Navier_Stokes_Fluide_Dilatable_Proto::assembler_avec_inertie_impl(*this,mat_morse,present,secmem);
}

void Navier_Stokes_Fluide_Dilatable_base::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  Navier_Stokes_Fluide_Dilatable_Proto::assembler_blocs_avec_inertie(*this, matrices, secmem, semi_impl);
}

