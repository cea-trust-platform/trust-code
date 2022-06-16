#include <Eq_Conv_Diff.h>
#include <Constituant_Avec_Vitesse.h>
#include <Discret_Thyd.h>
#include <Probleme_base.h>

Implemente_instanciable(Eq_Conv_Diff,"Convection_Diffusion",Convection_Diffusion_Concentration);
// XD  convection_diffusion  eqn_base convection_diffusion -1 not_set

Sortie& Eq_Conv_Diff::printOn(Sortie& is) const
{
  return Convection_Diffusion_Concentration::printOn(is);
}

Entree& Eq_Conv_Diff::readOn(Entree& is)
{
  Convection_Diffusion_Concentration::readOn(is);
  return is;
}

int Eq_Conv_Diff::lire_motcle_non_standard(const Motcle& mot,Entree& is)
{
  if (mot=="convection")
    {
      Cerr <<"Reading and typing of the convection operator : "<< finl;
      terme_convectif.associer_vitesse(le_constituant->vitesse_pour_transport());
      is >> terme_convectif;
      return 1;
    }
  else
    {
      Cout << mot << finl;
      return Convection_Diffusion_Concentration::lire_motcle_non_standard(mot,is);
    }
  return 1;
}

void Eq_Conv_Diff::associer_milieu_base(const Milieu_base& un_milieu)
{
  Convection_Diffusion_Concentration::associer_milieu_base(un_milieu);
  const Constituant_Avec_Vitesse& un_constituant=ref_cast(Constituant_Avec_Vitesse,un_milieu);
  if (un_constituant.vit_convection_constituant().non_nul())
    associer_constituant(un_constituant);
  else
    {
      Cerr <<"The dye (constituant) convection has not been defined."<< finl;
      exit();
    }
}

void Eq_Conv_Diff::associer_constituant(const Constituant_Avec_Vitesse& un_constituant)
{
  le_constituant = un_constituant;
}
