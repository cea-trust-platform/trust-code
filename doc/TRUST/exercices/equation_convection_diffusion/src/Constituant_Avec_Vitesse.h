#ifndef Constituant_Avec_Vitesse_included
#define Constituant_Avec_Vitesse_included

#include <Constituant.h>
#include <Champ_Inc.h>

class Constituant_Avec_Vitesse : public Constituant
{
  Declare_instanciable(Constituant_Avec_Vitesse);
public :
  int   initialiser(const double temps) override;
  void  set_param(Param& param) override;
  void  mettre_a_jour(double temps) override;
  const Champ_Don& vit_convection_constituant() const;
  const Champ_base& vitesse_pour_transport() ;
  void discretiser(const Probleme_base& pb, const Discretisation_base& dis) override;
protected :
  Champ_Don C_;
private :
  Champ_Inc vitesse_transport;
};
#endif
