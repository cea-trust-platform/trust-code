#include <Constituant_Avec_Vitesse.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Param.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(Constituant_Avec_Vitesse,"Constituant_Avec_Vitesse",Constituant);
// XD constituant_avec_vitesse constituant constituant_avec_vitesse -1 not_set
Sortie& Constituant_Avec_Vitesse::printOn(Sortie& os) const
{
  return os;
}

Entree& Constituant_Avec_Vitesse::readOn(Entree& is) 
{
  Constituant::readOn(is);
  return is;
}

void Constituant_Avec_Vitesse::set_param(Param& param)
{
  Constituant::set_param(param);
  param.ajouter("vitesse_convection",&C,Param::REQUIRED); // XD_ADD_P field_base not_set
}

void Constituant_Avec_Vitesse::discretiser(const Probleme_base& pb, 
                                      const Discretisation_base& dis)
{
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  dis.nommer_completer_champ_physique(zone_dis,"coefficient_diffusion","m2/s",D.valeur(),pb);
  dis.nommer_completer_champ_physique(zone_dis,"vitesse_convection_lu","m/s",C.valeur(),pb);
  champs_compris_.ajoute_champ(D.valeur());
  champs_compris_.ajoute_champ(C.valeur());
  const Schema_Temps_base& sch=pb.schema_temps();
  dis.discretiser_champ("vitesse",zone_dis,"vitesse_convection","m/s",
                        dimension,sch.nb_valeurs_temporelles(),
                        sch.temps_courant(),vitesse_transport);
  champs_compris_.ajoute_champ(vitesse_transport.valeur());
  Milieu_base::discretiser(pb,dis);
  if (C.non_nul())
    vitesse_transport.valeur().affecter_(C.valeur());
  else
  {
    Cerr<<"To must read "<<que_suis_je()<<" before discretiser "<<finl;
    exit();
  }
}

const Champ_base& Constituant_Avec_Vitesse::vitesse_pour_transport()
{
  return vitesse_transport;
}

const Champ_Don& Constituant_Avec_Vitesse::vit_convection_constituant() const
{
  return C;
}

void Constituant_Avec_Vitesse::mettre_a_jour(double temps)
{
  Constituant::mettre_a_jour(temps);
  if (C.non_nul())
    C.mettre_a_jour(temps);
  vitesse_transport.valeur().affecter_(C.valeur());
  vitesse_transport.changer_temps(temps);
}

int Constituant_Avec_Vitesse::initialiser(const double& temps)
{
  Constituant::initialiser(temps);
  if (C.non_nul())
    C.initialiser(temps);
  return 1;
}
