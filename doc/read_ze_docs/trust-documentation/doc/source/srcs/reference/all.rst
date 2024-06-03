
.. |nbsp| unicode:: 0xA0 
   :trim:
**Keywords derived from bloc_comment**
======================================

.. _bloc_comment:

**bloc_comment**
----------------

**Synonyms:** /*


bloc of Comment in a data file.

Usage:

**bloc_comment** **comm**  

Parameters are:

- **comm**  (*type:* string) Text to be commented.


----

**Keywords derived from champ_generique_base**
==============================================

.. _champ_generique_base:

**champ_generique_base**
------------------------


not_set

Usage:

| **champ_generique_base** *str*
| **Read** *str* {
| }


----

.. _champ_post_de_champs_post:

**champ_post_de_champs_post**
-----------------------------

**Inherits from:** :ref:`champ_generique_base` 


not_set

Usage:

| **champ_post_de_champs_post** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _champ_post_operateur_base:

**champ_post_operateur_base**
-----------------------------

**Inherits from:** :ref:`champ_generique_base` 


not_set

Usage:

| **champ_post_operateur_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _champ_post_operateur_eqn:

**champ_post_operateur_eqn**
----------------------------

**Synonyms:** operateur_eqn

**Inherits from:** :ref:`champ_generique_base` 


Post-process equation operators/sources

Usage:

| **champ_post_operateur_eqn** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[numero_source]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[numero_op]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[numero_masse]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sans_solveur_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[compo]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[numero_source]**  (*type:* int) the source to be post-processed (its number). If you have only one source term, numero_source will correspond to 0 if you want  to post-process that unique source

- **[numero_op]**  (*type:* int) numero_op will be 0 (diffusive operator) or 1 (convective operator) or 2  (gradient operator) or 3 (divergence operator).

- **[numero_masse]**  (*type:* int) numero_masse will be 0 for the mass equation operator in Pb_multiphase.

- **[sans_solveur_masse]**  (*type:* flag) not_set

- **[compo]**  (*type:* int) If you want to post-process only one component of a vector field, you can  specify the number of the component after compo keyword. By default, it is set to -1 which means that all the components will be post-processed. This feature is not available in VDF disretization.

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _champ_post_statistiques_base:

**champ_post_statistiques_base**
--------------------------------

**Inherits from:** :ref:`champ_generique_base` 


not_set

Usage:

| **champ_post_statistiques_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_deb**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_fin**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **t_deb**  (*type:* double) Start of integration time

- **t_fin**  (*type:* double) End of integration time

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _correlation:

**correlation**
---------------

**Synonyms:** champ_post_statistiques_correlation

**Inherits from:** :ref:`champ_generique_base` 


to calculate the correlation between the two fields.

Usage:

| **correlation** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_deb**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_fin**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **t_deb**  (*type:* double) Start of integration time

- **t_fin**  (*type:* double) End of integration time

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _divergence:

**divergence**
--------------

**Synonyms:** champ_post_operateur_divergence

**Inherits from:** :ref:`champ_generique_base` 


To calculate divergency of a given field.

Usage:

| **divergence** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _ecart_type:

**ecart_type**
--------------

**Synonyms:** champ_post_statistiques_ecart_type

**Inherits from:** :ref:`champ_generique_base` 


to calculate the standard deviation (statistic rms) of the field nom_champ.

Usage:

| **ecart_type** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_deb**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_fin**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **t_deb**  (*type:* double) Start of integration time

- **t_fin**  (*type:* double) End of integration time

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _extraction:

**extraction**
--------------

**Synonyms:** champ_post_extraction

**Inherits from:** :ref:`champ_generique_base` 


To create a surface field (values at the boundary) of a volume field

Usage:

| **extraction** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_frontiere**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode]**  string into ["trace", "champ_frontiere"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) name of the volume field

- **nom_frontiere**  (*type:* string) boundary name where the values of the volume field will be picked

- **[methode]**  (*type:* string into ["trace", "champ_frontiere"])  name of the extraction method (trace by_default  or champ_frontiere)

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _gradient:

**gradient**
------------

**Synonyms:** champ_post_operateur_gradient

**Inherits from:** :ref:`champ_generique_base` 


To calculate gradient of a given field.

Usage:

| **gradient** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _interpolation:

**interpolation**
-----------------

**Synonyms:** champ_post_interpolation

**Inherits from:** :ref:`champ_generique_base` 


To create a field which is an interpolation of the field given by the keyword source.

Usage:

| **interpolation** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **localisation**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[optimisation_sous_maillage]**  string into ["default", "yes", "no",]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **localisation**  (*type:* string) type_loc indicate where is done the interpolation (elem for element or som  for node).

- **[methode]**  (*type:* string) The optional keyword methode is limited to calculer_champ_post for the moment.

- **[domaine]**  (*type:* string) the domain name where the interpolation is done (by default, the calculation  domain)

- **[optimisation_sous_maillage]**  (*type:* string into ["default", "yes", "no",])  not_set

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _morceau_equation:

**morceau_equation**
--------------------

**Synonyms:** champ_post_morceau_equation

**Inherits from:** :ref:`champ_generique_base` 


To calculate a field related to a piece of equation.
For the moment, the field which can be calculated is the stability time step of an 
operator equation.
The problem name and the unknown of the equation should be given by Source refChamp 
{ Pb_Champ problem_name unknown_field_of_equation }

Usage:

| **morceau_equation** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **type**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[numero]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **option**  string into ["stabilite", "flux_bords", "flux_surfacique_bords"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[compo]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **type**  (*type:* string) can only be operateur for equation operators.

- **[numero]**  (*type:* int) numero will be 0 (diffusive operator) or 1 (convective operator) or 2 (gradient  operator) or 3 (divergence operator).

- **option**  (*type:* string into ["stabilite", "flux_bords", "flux_surfacique_bords"])  option is stability  for time steps or flux_bords for boundary fluxes or flux_surfacique_bords for boundary  surfacic fluxes

- **[compo]**  (*type:* int) compo will specify the number component of the boundary flux (for boundary  fluxes, in this case compo permits to specify the number component of the boundary  flux choosen).

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _moyenne:

**moyenne**
-----------

**Synonyms:** champ_post_statistiques_moyenne

**Inherits from:** :ref:`champ_generique_base` 


to calculate the average of the field over time

Usage:

| **moyenne** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[moyenne_convergee]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_deb**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t_fin**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[moyenne_convergee]**  (*type:* :ref:`field_base`) This option allows to read a converged time averaged field in a .xyz  file in order to calculate, when resuming the calculation, the statistics fields (rms,  correlation) which depend on this average. In that case, the time averaged field is not updated during the resume of calculation. In this case, the time averaged field must be fully converged to avoid errors when  calculating high order statistics.

- **t_deb**  (*type:* double) Start of integration time

- **t_fin**  (*type:* double) End of integration time

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _predefini:

**predefini**
-------------

**Inherits from:** :ref:`champ_generique_base` 


This keyword is used to post process predefined postprocessing fields.

Usage:

| **predefini** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **pb_champ**  :ref:`deuxmots`
| }

Parameters are:

- **pb_champ**  (*type:* :ref:`deuxmots`) { Pb_champ nom_pb nom_champ } : nom_pb is the problem name and nom_champ  is the selected field name. The available keywords for the field name are: energie_cinetique_totale, energie_cinetique_elem,  viscosite_turbulente, viscous_force_x, viscous_force_y, viscous_force_z, pressure_force_x,  pressure_force_y, pressure_force_z, total_force_x, total_force_y, total_force_z, viscous_force,  pressure_force, total_force


----

.. _reduction_0d:

**reduction_0d**
----------------

**Synonyms:** champ_post_reduction_0d

**Inherits from:** :ref:`champ_generique_base` 


To calculate the min, max, sum, average, weighted sum, weighted average, weighted 
sum by porosity, weighted average by porosity, euclidian norm, normalized euclidian 
norm, L1 norm, L2 norm of a field.

Usage:

| **reduction_0d** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **methode**  string into ["min", "max", "moyenne", "average", "moyenne_ponderee", "weighted_average", "somme", "sum", "somme_ponderee", "weighted_sum", "somme_ponderee_porosite", "weighted_sum_porosity", "euclidian_norm", "normalized_euclidian_norm", "l1_norm", "l2_norm", "valeur_a_gauche", "left_value"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **methode**  (*type:* string into ["min", "max", "moyenne", "average", "moyenne_ponderee", "weighted_average", "somme", "sum", "somme_ponderee", "weighted_sum", "somme_ponderee_porosite", "weighted_sum_porosity", "euclidian_norm", "normalized_euclidian_norm", "l1_norm", "l2_norm", "valeur_a_gauche", "left_value"])   name of the reduction method:  - min for the minimum value,  - max for the maximum value,  - average (or moyenne) for a mean,  - weighted_average (or moyenne_ponderee) for a mean ponderated by integration volumes,  e.g: cell volumes for temperature and pressure in VDF, volumes around faces for velocity  and temperature in VEF,  - sum (or somme) for the sum of all the values of the field,  - weighted_sum (or somme_ponderee) for a weighted sum (integral),  - weighted_average_porosity (or moyenne_ponderee_porosite) and weighted_sum_porosity  (or somme_ponderee_porosite) for the mean and sum weighted by the volumes of the elements,  only for ELEM localisation,  - euclidian_norm for the euclidian norm,  - normalized_euclidian_norm for the euclidian norm normalized,  - L1_norm for norm L1,  - L2_norm for norm L2

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _refchamp:

**refchamp**
------------

**Synonyms:** champ_post_refchamp

**Inherits from:** :ref:`champ_generique_base` 


Field of prolem

Usage:

| **refchamp** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **pb_champ**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| }

Parameters are:

- **pb_champ**  (*type:* :ref:`deuxmots`) { Pb_champ nom_pb nom_champ } : nom_pb is the problem name and nom_champ  is the selected field name.

- **[nom_source]**  (*type:* string) The alias name for the field


----

.. _tparoi_vef:

**tparoi_vef**
--------------

**Synonyms:** champ_post_tparoi_vef

**Inherits from:** :ref:`champ_generique_base` 


This keyword is used to post process (only for VEF discretization) the temperature 
field with a slight difference on boundaries with Neumann condition where law of the 
wall is applied on the temperature field.
nom_pb is the problem name and field_name is the selected field name.
A keyword (temperature_physique) is available to post process this field without 
using Definition_champs.

Usage:

| **tparoi_vef** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

.. _transformation:

**transformation**
------------------

**Synonyms:** champ_post_transformation

**Inherits from:** :ref:`champ_generique_base` 


To create a field with a transformation.

Usage:

| **transformation** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **methode**  string into ["produit_scalaire", "norme", "vecteur", "formule", "composante"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[expression]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[numero]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[localisation]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source]**  :ref:`champ_generique_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_source]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[source_reference]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources_reference]**  :ref:`list_nom_virgule`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`listchamp_generique`
| }

Parameters are:

- **methode**  (*type:* string into ["produit_scalaire", "norme", "vecteur", "formule", "composante"])  methode  norme : will calculate the norm of a vector given by a source field  methode produit_scalaire : will calculate the dot product of two vectors given by  two sources fields  methode composante numero integer : will create a field by extracting the integer  component of a field given by a source field  methode formule expression 1 : will create a scalar field located to elements using  expressions with x,y,z,t parameters and field names given by a source field or several  sources fields.  methode vecteur expression N f1(x,y,z,t) fN(x,y,z,t) : will create a vector field  located to elements by defining its N components with N expressions with x,y,z,t parameters  and field names given by a source field or several sources fields.

- **[expression]**  (*type:* string list) see methodes formule and vecteur

- **[numero]**  (*type:* int) see methode composante

- **[localisation]**  (*type:* string) type_loc indicate where is done the interpolation (elem for element or som  for node). The optional keyword methode is limited to calculer_champ_post for the moment

- **[source]**  (*type:* :ref:`champ_generique_base`) the source field.

- **[nom_source]**  (*type:* string) To name a source field with the nom_source keyword

- **[source_reference]**  (*type:* string) not_set

- **[sources_reference]**  (*type:* :ref:`list_nom_virgule`) not_set

- **[sources]**  (*type:* :ref:`listchamp_generique`) sources { Champ_Post.... { ... } Champ_Post.. { ... }}


----

**Keywords derived from champs_a_post**
=======================================

.. _champs_a_post:

**champs_a_post**
-----------------


Fields to be post-processed.


----

**Keywords derived from chimie**
================================

.. _chimie:

**chimie**
----------


Keyword to describe the chmical reactions

Usage:

| **chimie** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **reactions**  :ref:`reactions`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_micro_melange]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constante_modele_micro_melange]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[espece_en_competition_micro_melange]**  string
| }

Parameters are:

- **reactions**  (*type:* :ref:`reactions`) list of reactions

- **[modele_micro_melange]**  (*type:* int) modele_micro_melange (0 by default)

- **[constante_modele_micro_melange]**  (*type:* double) constante of modele (1 by default)

- **[espece_en_competition_micro_melange]**  (*type:* string) espece in competition in reactions


----

**Keywords derived from class_generic**
=======================================

.. _amgx:

**amgx**
--------

**Inherits from:** :ref:`class_generic` 


Solver via AmgX API

Usage:

**amgx** **solveur** **option_solveur** **[atol]** **[rtol]**  

Parameters are:

- **solveur**  (*type:* string) not_set

- **option_solveur**  (*type:* :ref:`bloc_lecture`) not_set

- **[atol]**  (*type:* double) Absolute threshold for convergence (same as seuil option)

- **[rtol]**  (*type:* double) Relative threshold for convergence


----

.. _cholesky:

**cholesky**
------------

**Inherits from:** :ref:`class_generic` 


Cholesky direct method.

Usage:

| **cholesky** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| }

Parameters are:

- **[impr]**  (*type:* flag) Keyword which may be used to print the resolution time.

- **[quiet]**  (*type:* flag) To disable printing of information


----

.. _class_generic:

**class_generic**
-----------------


not_set

Usage:

| **class_generic** *str*
| **Read** *str* {
| }


----

.. _dt_calc_dt_calc:

**dt_calc_dt_calc**
-------------------

**Synonyms:** dt_calc

**Inherits from:** :ref:`class_generic` 


The time step at first iteration is calculated in agreement with CFL condition.

Usage:

**dt_calc_dt_calc**  


----

.. _dt_calc_dt_fixe:

**dt_calc_dt_fixe**
-------------------

**Synonyms:** dt_fixe

**Inherits from:** :ref:`class_generic` 


The first time step is fixed by the user (recommended when resuming calculation with 
Crank Nicholson temporal scheme to ensure continuity).

Usage:

**dt_calc_dt_fixe** **value**  

Parameters are:

- **value**  (*type:* double) first time step.


----

.. _dt_calc_dt_min:

**dt_calc_dt_min**
------------------

**Synonyms:** dt_min

**Inherits from:** :ref:`class_generic` 


The first iteration is based on dt_min.

Usage:

**dt_calc_dt_min**  


----

.. _dt_start:

**dt_start**
------------

**Inherits from:** :ref:`class_generic` 


not_set

Usage:

**dt_start**  


----

.. _gcp_ns:

**gcp_ns**
----------

**Inherits from:** :ref:`class_generic` 


not_set

Usage:

| **gcp_ns** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur0**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur1**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond]**  :ref:`precond_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond_nul]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **seuil**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[save_matrice | save_matrix]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[optimized]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| }

Parameters are:

- **solveur0**  (*type:* :ref:`solveur_sys_base`) Solver type.

- **solveur1**  (*type:* :ref:`solveur_sys_base`) Solver type.

- **[precond]**  (*type:* :ref:`precond_base`) Keyword to define system preconditioning in order to accelerate resolution  by the conjugated gradient. Many parallel preconditioning methods are not equivalent to their sequential counterpart,  and you should therefore expect differences, especially when you select a high value  of the final residue (seuil). The result depends on the number of processors and on the mesh splitting. It is sometimes useful to run the solver with no preconditioning at all. In particular:  - when the solver does not converge during initial projection,  - when comparing sequential and parallel computations.  With no preconditioning, except in some particular cases (no open boundary), the  sequential and the parallel computations should provide exactly the same results within  fpu accuracy. If not, there might be a coding error or the system of equations is singular.

- **[precond_nul]**  (*type:* flag) Keyword to not use a preconditioning method.

- **seuil**  (*type:* double) Value of the final residue. The gradient ceases iteration when the Euclidean residue standard ||Ax-B|| is less  than this value.

- **[impr]**  (*type:* flag) Keyword which is used to request display of the Euclidean residue standard  each time this iterates through the conjugated gradient (display to the standard outlet).

- **[quiet]**  (*type:* flag) To not displaying any outputs of the solver.

- **[save_matrice | save_matrix]**  (*type:* flag) to save the matrix in a file.

- **[optimized]**  (*type:* flag) This keyword triggers a memory and network optimized algorithms useful for  strong scaling (when computing less than 100 000 elements per processor). The matrix and the vectors are duplicated, common items removed and only virtual  items really used in the matrix are exchanged.NL2 Warning: this is experimental and  known to fail in some VEF computations (L2 projection step will not converge). Works well in VDF.

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gcp.


----

.. _gen:

**gen**
-------

**Inherits from:** :ref:`class_generic` 


not_set

Usage:

| **gen** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solv_elem**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **precond**  :ref:`precond_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[save_matrice | save_matrix]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[force]**  flag
| }

Parameters are:

- **solv_elem**  (*type:* string) To specify a solver among gmres or bicgstab.

- **precond**  (*type:* :ref:`precond_base`) The only preconditionner that we can specify is ilu.

- **[seuil]**  (*type:* double) Value of the final residue. The solver ceases iterations when the Euclidean residue standard ||Ax-B|| is less  than this value. default value 1e-12.

- **[impr]**  (*type:* flag) Keyword which is used to request display of the Euclidean residue standard  each time this iterates through the conjugated gradient (display to the standard outlet).

- **[save_matrice | save_matrix]**  (*type:* flag) To save the matrix in a file.

- **[quiet]**  (*type:* flag) To not displaying any outputs of the solver.

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the GEN solver.

- **[force]**  (*type:* flag) Keyword to set ipar[5]=-1 in the GEN solver. This is helpful if you notice that the solver does not perform more than 100 iterations. If this keyword is specified in the datafile, you should provide nb_it_max.


----

.. _gmres:

**gmres**
---------

**Inherits from:** :ref:`class_generic` 


Gmres method (for non symetric matrix).

Usage:

| **gmres** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diag]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  int into ["0", "1"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[save_matrice | save_matrix]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dim_espace_krilov]**  int
| }

Parameters are:

- **[impr]**  (*type:* flag) Keyword which may be used to print the convergence.

- **[quiet]**  (*type:* flag) To disable printing of information

- **[seuil]**  (*type:* double) Convergence value.

- **[diag]**  (*type:* flag) Keyword to use diagonal preconditionner (in place of pilut that is not parallel).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* int into ["0", "1"])  Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.

- **[save_matrice | save_matrix]**  (*type:* flag) to save the matrix in a file.

- **[dim_espace_krilov]**  (*type:* int) not_set


----

.. _optimal:

**optimal**
-----------

**Inherits from:** :ref:`class_generic` 


Optimal is a solver which tests several solvers of the previous list to choose the 
fastest one for the considered linear system.

Usage:

| **optimal** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **seuil**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[save_matrice | save_matrix]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[frequence_recalc]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_fichier_solveur]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_solveur_non_recree]**  flag
| }

Parameters are:

- **seuil**  (*type:* double) Convergence threshold

- **[impr]**  (*type:* flag) To print the convergency of the fastest solver

- **[quiet]**  (*type:* flag) To disable printing of information

- **[save_matrice | save_matrix]**  (*type:* flag) To save the linear system (A, x, B) into a file

- **[frequence_recalc]**  (*type:* int) To set a time step period (by default, 100) for re-checking the fatest solver

- **[nom_fichier_solveur]**  (*type:* string) To specify the file containing the list of the tested solvers

- **[fichier_solveur_non_recree]**  (*type:* flag) To avoid the creation of the file containing the list


----

.. _petsc:

**petsc**
---------

**Inherits from:** :ref:`class_generic` 


Solver via Petsc API

\input{{solvpetsc}}

Usage:

**petsc** **solveur** **option_solveur** **[atol]** **[rtol]**  

Parameters are:

- **solveur**  (*type:* string) not_set

- **option_solveur**  (*type:* :ref:`bloc_lecture`) not_set

- **[atol]**  (*type:* double) Absolute threshold for convergence (same as seuil option)

- **[rtol]**  (*type:* double) Relative threshold for convergence


----

.. _rocalution:

**rocalution**
--------------

**Inherits from:** :ref:`class_generic` 


Solver via rocALUTION API

Usage:

**rocalution** **solveur** **option_solveur** **[atol]** **[rtol]**  

Parameters are:

- **solveur**  (*type:* string) not_set

- **option_solveur**  (*type:* :ref:`bloc_lecture`) not_set

- **[atol]**  (*type:* double) Absolute threshold for convergence (same as seuil option)

- **[rtol]**  (*type:* double) Relative threshold for convergence


----

.. _solv_gcp:

**solv_gcp**
------------

**Synonyms:** gcp

**Inherits from:** :ref:`class_generic` 


Preconditioned conjugated gradient.

Usage:

| **solv_gcp** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond]**  :ref:`precond_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond_nul]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **seuil**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[quiet]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[save_matrice | save_matrix]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[optimized]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| }

Parameters are:

- **[precond]**  (*type:* :ref:`precond_base`) Keyword to define system preconditioning in order to accelerate resolution  by the conjugated gradient. Many parallel preconditioning methods are not equivalent to their sequential counterpart,  and you should therefore expect differences, especially when you select a high value  of the final residue (seuil). The result depends on the number of processors and on the mesh splitting. It is sometimes useful to run the solver with no preconditioning at all. In particular:  - when the solver does not converge during initial projection,  - when comparing sequential and parallel computations.  With no preconditioning, except in some particular cases (no open boundary), the  sequential and the parallel computations should provide exactly the same results within  fpu accuracy. If not, there might be a coding error or the system of equations is singular.

- **[precond_nul]**  (*type:* flag) Keyword to not use a preconditioning method.

- **seuil**  (*type:* double) Value of the final residue. The gradient ceases iteration when the Euclidean residue standard ||Ax-B|| is less  than this value.

- **[impr]**  (*type:* flag) Keyword which is used to request display of the Euclidean residue standard  each time this iterates through the conjugated gradient (display to the standard outlet).

- **[quiet]**  (*type:* flag) To not displaying any outputs of the solver.

- **[save_matrice | save_matrix]**  (*type:* flag) to save the matrix in a file.

- **[optimized]**  (*type:* flag) This keyword triggers a memory and network optimized algorithms useful for  strong scaling (when computing less than 100 000 elements per processor). The matrix and the vectors are duplicated, common items removed and only virtual  items really used in the matrix are exchanged.NL2 Warning: this is experimental and  known to fail in some VEF computations (L2 projection step will not converge). Works well in VDF.

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gcp.


----

.. _solveur_sys_base:

**solveur_sys_base**
--------------------

**Inherits from:** :ref:`class_generic` 


Basic class to solve the linear system.

Usage:

| **solveur_sys_base** *str*
| **Read** *str* {
| }


----

**Keywords derived from coarsen_operators**
===========================================

.. _coarsen_operators:

**coarsen_operators**
---------------------


not_set


----

**Keywords derived from comment**
=================================

.. _comment:

**comment**
-----------

**Synonyms:** #


Comments in a data file.

Usage:

**comment** **comm**  

Parameters are:

- **comm**  (*type:* string) Text to be commented.


----

**Keywords derived from condinits**
===================================

.. _condinits:

**condinits**
-------------


Initial conditions.


----

**Keywords derived from condlim_base**
======================================

.. _condlim_base:

**condlim_base**
----------------


Basic class of boundary conditions.

Usage:

**condlim_base**  


----

.. _dirichlet:

**dirichlet**
-------------

**Inherits from:** :ref:`condlim_base` 


Dirichlet condition at the boundary called bord (edge) : 1).
For Navier-Stokes equations, velocity imposed at the boundary; 2).
For scalar transport equation, scalar imposed at the boundary.

Usage:

**dirichlet**  


----

.. _echange_couplage_thermique:

**echange_couplage_thermique**
------------------------------

**Inherits from:** :ref:`condlim_base` 


Thermal coupling boundary condition

Usage:

| **echange_couplage_thermique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[temperature_paroi]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[flux_paroi]**  :ref:`field_base`
| }

Parameters are:

- **[temperature_paroi]**  (*type:* :ref:`field_base`) Temperature

- **[flux_paroi]**  (*type:* :ref:`field_base`) Wall heat flux


----

.. _echange_interne_global_impose:

**echange_interne_global_impose**
---------------------------------

**Synonyms:** paroi_echange_interne_global_impose

**Inherits from:** :ref:`condlim_base` 


Internal heat exchange boundary condition with global exchange coefficient.

Usage:

**echange_interne_global_impose** **h_imp** **ch**  

Parameters are:

- **h_imp**  (*type:* string) Global exchange coefficient value. The global exchange coefficient value is expressed in W.m-2.K-1.

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _echange_interne_global_parfait:

**echange_interne_global_parfait**
----------------------------------

**Synonyms:** paroi_echange_interne_global_parfait

**Inherits from:** :ref:`condlim_base` 


Internal heat exchange boundary condition with perfect (infinite) exchange coefficient.

Usage:

**echange_interne_global_parfait**  


----

.. _echange_interne_impose:

**echange_interne_impose**
--------------------------

**Synonyms:** paroi_echange_interne_impose

**Inherits from:** :ref:`condlim_base` 


Internal heat exchange boundary condition with exchange coefficient.

Usage:

**echange_interne_impose** **h_imp** **ch**  

Parameters are:

- **h_imp**  (*type:* string) Exchange coefficient value expressed in W.m-2.K-1.

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _echange_interne_parfait:

**echange_interne_parfait**
---------------------------

**Synonyms:** paroi_echange_interne_parfait

**Inherits from:** :ref:`condlim_base` 


Internal heat exchange boundary condition with perfect (infinite) exchange coefficient.

Usage:

**echange_interne_parfait**  


----

.. _entree_temperature_imposee_h:

**entree_temperature_imposee_h**
--------------------------------

**Inherits from:** :ref:`condlim_base` 


Particular case of class frontiere_ouverte_temperature_imposee for enthalpy equation.

Usage:

**entree_temperature_imposee_h** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte:

**frontiere_ouverte**
---------------------

**Inherits from:** :ref:`condlim_base` 


Boundary outlet condition on the boundary called bord (edge) (diffusion flux zero).
This condition must be associated with a boundary outlet hydraulic condition.

Usage:

**frontiere_ouverte** **var_name** **ch**  

Parameters are:

- **var_name**  (*type:* string into ["t_ext", "c_ext", "y_ext", "k_eps_ext", "fluctu_temperature_ext", "flux_chaleur_turb_ext", "v2_ext", "a_ext", "tau_ext", "k_ext", "omega_ext"])   Field name.

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_concentration_imposee:

**frontiere_ouverte_concentration_imposee**
-------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed concentration condition at an open boundary called bord (edge) (situation 
corresponding to a fluid inlet).
This condition must be associated with an imposed inlet velocity condition.

Usage:

**frontiere_ouverte_concentration_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_fraction_massique_imposee:

**frontiere_ouverte_fraction_massique_imposee**
-----------------------------------------------

**Inherits from:** :ref:`condlim_base` 


not_set

Usage:

**frontiere_ouverte_fraction_massique_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_gradient_pression_impose:

**frontiere_ouverte_gradient_pression_impose**
----------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Normal imposed pressure gradient condition on the open boundary called bord (edge).
This boundary condition may be only used in VDF discretization.
The imposed $\partial P/\partial n$ value is expressed in Pa.m-1.

Usage:

**frontiere_ouverte_gradient_pression_impose** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_gradient_pression_impose_vefprep1b:

**frontiere_ouverte_gradient_pression_impose_vefprep1b**
--------------------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Keyword for an outlet boundary condition in VEF P1B/P1NC on the gradient of the pressure.

Usage:

**frontiere_ouverte_gradient_pression_impose_vefprep1b** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_gradient_pression_libre_vef:

**frontiere_ouverte_gradient_pression_libre_vef**
-------------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class for outlet boundary condition in VEF like Orlansky.
There is no reference for pressure for theses boundary conditions so it is better 
to add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two 
cells (for symmetry in a channel) of the boundary where Orlansky conditions are imposed.

Usage:

**frontiere_ouverte_gradient_pression_libre_vef**  


----

.. _frontiere_ouverte_gradient_pression_libre_vefprep1b:

**frontiere_ouverte_gradient_pression_libre_vefprep1b**
-------------------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class for outlet boundary condition in VEF P1B/P1NC like Orlansky.

Usage:

**frontiere_ouverte_gradient_pression_libre_vefprep1b**  


----

.. _frontiere_ouverte_pression_imposee:

**frontiere_ouverte_pression_imposee**
--------------------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed pressure condition at the open boundary called bord (edge).
The imposed pressure field is expressed in Pa.

Usage:

**frontiere_ouverte_pression_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_pression_imposee_orlansky:

**frontiere_ouverte_pression_imposee_orlansky**
-----------------------------------------------

**Inherits from:** :ref:`condlim_base` 


This boundary condition may only be used with VDF discretization.
There is no reference for pressure for this boundary condition so it is better to 
add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two cells 
(for symetry in a channel) of the boundary where Orlansky conditions are imposed.

Usage:

**frontiere_ouverte_pression_imposee_orlansky**  


----

.. _frontiere_ouverte_pression_moyenne_imposee:

**frontiere_ouverte_pression_moyenne_imposee**
----------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class for open boundary with pressure mean level imposed.

Usage:

**frontiere_ouverte_pression_moyenne_imposee** **pext**  

Parameters are:

- **pext**  (*type:* double) Mean pressure.


----

.. _frontiere_ouverte_rho_u_impose:

**frontiere_ouverte_rho_u_impose**
----------------------------------

**Inherits from:** :ref:`condlim_base` 


This keyword is used to designate a condition of imposed mass rate at an open boundary 
called bord (edge).
The imposed mass rate field at the inlet is vectorial and the imposed velocity values 
are expressed in kg.s-1.
This boundary condition can be used only with the Quasi compressible model.

Usage:

**frontiere_ouverte_rho_u_impose** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_temperature_imposee:

**frontiere_ouverte_temperature_imposee**
-----------------------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed temperature condition at the open boundary called bord (edge) (in the case 
of fluid inlet).
This condition must be associated with an imposed inlet velocity condition.
The imposed temperature value is expressed in oC or K.

Usage:

**frontiere_ouverte_temperature_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_vitesse_imposee:

**frontiere_ouverte_vitesse_imposee**
-------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class for velocity-inlet boundary condition.
The imposed velocity field at the inlet is vectorial and the imposed velocity values 
are expressed in m.s-1.

Usage:

**frontiere_ouverte_vitesse_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _frontiere_ouverte_vitesse_imposee_sortie:

**frontiere_ouverte_vitesse_imposee_sortie**
--------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Sub-class for velocity boundary condition.
The imposed velocity field at the open boundary is vectorial and the imposed velocity 
values are expressed in m.s-1.

Usage:

**frontiere_ouverte_vitesse_imposee_sortie** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _neumann:

**neumann**
-----------

**Inherits from:** :ref:`condlim_base` 


Neumann condition at the boundary called bord (edge) : 1).
For Navier-Stokes equations, constraint imposed at the boundary; 2).
For scalar transport equation, flux imposed at the boundary.

Usage:

**neumann**  


----

.. _neumann_homogene:

**neumann_homogene**
--------------------

**Inherits from:** :ref:`condlim_base` 


Homogeneous neumann boundary condition

Usage:

**neumann_homogene**  


----

.. _neumann_paroi:

**neumann_paroi**
-----------------

**Inherits from:** :ref:`condlim_base` 


Neumann boundary condition for mass equation (multiphase problem)

Usage:

**neumann_paroi**  


----

.. _neumann_paroi_adiabatique:

**neumann_paroi_adiabatique**
-----------------------------

**Inherits from:** :ref:`condlim_base` 


Adiabatic wall neumann boundary condition

Usage:

**neumann_paroi_adiabatique**  


----

.. _paroi:

**paroi**
---------

**Inherits from:** :ref:`condlim_base` 


Impermeability condition at a wall called bord (edge) (standard flux zero).
This condition must be associated with a wall type hydraulic condition.

Usage:

**paroi**  


----

.. _paroi_adiabatique:

**paroi_adiabatique**
---------------------

**Inherits from:** :ref:`condlim_base` 


Normal zero flux condition at the wall called bord (edge).

Usage:

**paroi_adiabatique**  


----

.. _paroi_contact:

**paroi_contact**
-----------------

**Inherits from:** :ref:`condlim_base` 


Thermal condition between two domains.
Important: the name of the boundaries in the two domains should be the same.
(Warning: there is also an old limitation not yet fixed on the sequential algorithm 
in VDF to detect the matching faces on the two boundaries: faces should be ordered 
in the same way).
The kind of condition depends on the discretization.
In VDF, it is a heat exchange condition, and in VEF, a temperature condition.

Such a coupling requires coincident meshes for the moment.
In case of non-coincident meshes, run is stopped and two external files are automatically 
generated in VEF (connectivity_failed_boundary_name and connectivity_failed_pb_name.med).
In 2D, the keyword Decouper_bord_coincident associated to the connectivity_failed_boundary_name 
file allows to generate a new coincident mesh.

In 3D, for a first preliminary cut domain with HOMARD (fluid for instance), the second 
problem associated to pb_name (solide in a fluid/solid coupling problem) has to be 
submitted to HOMARD cutting procedure with connectivity_failed_pb_name.med.

Such a procedure works as while the primary refined mesh (fluid in our example) impacts 
the fluid/solid interface with a compact shape as described below (values 2 or 4 indicates 
the number of division from primary faces obtained in fluid domain at the interface 
after HOMARD cutting):

2-2-2-2-2-2

2-4-4-4-4-4-2 \\; 2-2-2

2-4-4-4-4-2 \\; 2-4-2

2-2-2-2-2 \\; 2-2

OK

NL2 2-2 \\; \\; 2-2-2

2-4-2 \\; 2-2

2-2 \\; 2-2

NOT OK

Usage:

**paroi_contact** **autrepb** **nameb**  

Parameters are:

- **autrepb**  (*type:* :ref:`pb_base`) Name of other problem.

- **nameb**  (*type:* string) boundary name of the remote problem which should be the same than the local  name


----

.. _paroi_contact_fictif:

**paroi_contact_fictif**
------------------------

**Inherits from:** :ref:`condlim_base` 


This keyword is derivated from paroi_contact and is especially dedicated to compute 
coupled fluid/solid/fluid problem in case of thin material.
Thanks to this option, solid is considered as a fictitious media (no mesh, no domain 
associated), and coupling is performed by considering instantaneous thermal equilibrium 
in it (for the moment).

Usage:

**paroi_contact_fictif** **autrepb** **nameb** **conduct_fictif** **ep_fictive**  

Parameters are:

- **autrepb**  (*type:* :ref:`pb_base`) Name of other problem.

- **nameb**  (*type:* string) Name of bord.

- **conduct_fictif**  (*type:* double) thermal conductivity

- **ep_fictive**  (*type:* double) thickness of the fictitious media


----

.. _paroi_decalee_robin:

**paroi_decalee_robin**
-----------------------

**Inherits from:** :ref:`condlim_base` 


This keyword is used to designate a Robin boundary condition (a.u+b.du/dn=c) associated 
with the Pironneau methodology for the wall laws.
The value of given by the delta option is the distance between the mesh (where symmetry 
boundary condition is applied) and the fictious wall.
This boundary condition needs the definition of the dedicated source terms (Source_Robin 
or Source_Robin_Scalaire) according the equations used.

Usage:

| **paroi_decalee_robin** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **delta**  double
| }

Parameters are:

- **delta**  (*type:* double) not_set


----

.. _paroi_defilante:

**paroi_defilante**
-------------------

**Inherits from:** :ref:`condlim_base` 


Keyword to designate a condition where tangential velocity is imposed on the wall 
called bord (edge).
If the velocity components set by the user is not tangential, projection is used.

Usage:

**paroi_defilante** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_echange_contact_correlation_vdf:

**paroi_echange_contact_correlation_vdf**
-----------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class to define a thermohydraulic 1D model which will apply to a boundary of 2D or 
3D domain.

Warning : For parallel calculation, the only possible partition will be according 
the axis of the model with the keyword Tranche.

Usage:

| **paroi_echange_contact_correlation_vdf** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dir**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **tinf**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **tsup**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **rho**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dt_impr**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **debit**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dh**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **volume**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nu**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise_correlation]**  flag
| }

Parameters are:

- **dir**  (*type:* int) Direction (0 : axis X, 1 : axis Y, 2 : axis Z) of the 1D model.

- **tinf**  (*type:* double) Inlet fluid temperature of the 1D model (oC or K).

- **tsup**  (*type:* double) Outlet fluid temperature of the 1D model (oC or K).

- **Lambda | lambda_u**  (*type:* string) Thermal conductivity of the fluid (W.m-1.K-1).

- **rho**  (*type:* string) Mass density of the fluid (kg.m-3) which may be a function of the temperature  T.

- **cp**  (*type:* double) Calorific capacity value at a constant pressure of the fluid (J.kg-1.K-1).

- **dt_impr**  (*type:* double) Printing period in name_of_data_file_time.dat files of the 1D model results.

- **mu**  (*type:* string) Dynamic viscosity of the fluid (kg.m-1.s-1) which may be a function of thetemperature  T.

- **debit**  (*type:* double) Surface flow rate (kg.s-1.m-2) of the fluid into the channel.

- **dh**  (*type:* double) Hydraulic diameter may be a function f(x) with x position along the 1D  axis (xinf <= x <= xsup)

- **volume**  (*type:* string) Exact volume of the 1D domain (m3) which may be a function of the hydraulic  diameter (Dh) and the lateral surface (S) of the meshed boundary.

- **nu**  (*type:* string) Nusselt number which may be a function of the Reynolds number (Re) and the  Prandtl number (Pr).

- **[reprise_correlation]**  (*type:* flag) Keyword in the case of a resuming calculation with this correlation.


----

.. _paroi_echange_contact_correlation_vef:

**paroi_echange_contact_correlation_vef**
-----------------------------------------

**Inherits from:** :ref:`condlim_base` 


Class to define a thermohydraulic 1D model which will apply to a boundary of 2D or 
3D domain.

Warning : For parallel calculation, the only possible partition will be according 
the axis of the model with the keyword Tranche_geom.

Usage:

| **paroi_echange_contact_correlation_vef** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dir**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **tinf**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **tsup**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **rho**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dt_impr**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **debit**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dh**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **n**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **surface**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nu**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **xinf**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **xsup**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise_correlation]**  flag
| }

Parameters are:

- **dir**  (*type:* int) Direction (0 : axis X, 1 : axis Y, 2 : axis Z) of the 1D model.

- **tinf**  (*type:* double) Inlet fluid temperature of the 1D model (oC or K).

- **tsup**  (*type:* double) Outlet fluid temperature of the 1D model (oC or K).

- **Lambda | lambda_u**  (*type:* string) Thermal conductivity of the fluid (W.m-1.K-1).

- **rho**  (*type:* string) Mass density of the fluid (kg.m-3) which may be a function of the temperature  T.

- **cp**  (*type:* double) Calorific capacity value at a constant pressure of the fluid (J.kg-1.K-1).

- **dt_impr**  (*type:* double) Printing period in name_of_data_file_time.dat files of the 1D model results.

- **mu**  (*type:* string) Dynamic viscosity of the fluid (kg.m-1.s-1) which may be a function of thetemperature  T.

- **debit**  (*type:* double) Surface flow rate (kg.s-1.m-2) of the fluid into the channel.

- **dh**  (*type:* string) Hydraulic diameter may be a function f(x) with x position along the 1D axis  (xinf <= x <= xsup)

- **n**  (*type:* int) Number of 1D cells of the 1D mesh.

- **surface**  (*type:* string) Section surface of the channel which may be function f(Dh,x) of the hydraulic  diameter (Dh) and x position along the 1D axis (xinf <= x <= xsup)

- **nu**  (*type:* string) Nusselt number which may be a function of the Reynolds number (Re) and the  Prandtl number (Pr).

- **xinf**  (*type:* double) Position of the inlet of the 1D mesh on the axis direction.

- **xsup**  (*type:* double) Position of the outlet of the 1D mesh on the axis direction.

- **[emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies]**  (*type:* double) Coefficient of emissivity for radiation between two quasi infinite plates.

- **[reprise_correlation]**  (*type:* flag) Keyword in the case of a resuming calculation with this correlation.


----

.. _paroi_echange_contact_vdf:

**paroi_echange_contact_vdf**
-----------------------------

**Inherits from:** :ref:`condlim_base` 


Boundary condition type to model the heat flux between two problems.
Important: the name of the boundaries in the two problems should be the same.

Usage:

**paroi_echange_contact_vdf** **autrepb** **nameb** **temp** **h**  

Parameters are:

- **autrepb**  (*type:* :ref:`pb_base`) Name of other problem.

- **nameb**  (*type:* string) Name of bord.

- **temp**  (*type:* string) Name of field.

- **h**  (*type:* double) Value assigned to a coefficient (expressed in W.K-1m-2) that characterises  the contact between the two mediums. In order to model perfect contact, h must be taken to be infinite. This value must obviously be the same in both the two problems blocks.  The surface thermal flux exchanged between the two mediums is represented by :  fi = h (T1-T2) where 1/h = d1/lambda1 + 1/val_h_contact + d2/lambda2  where di : distance between the node where Ti and the wall is found.


----

.. _paroi_echange_externe_impose:

**paroi_echange_externe_impose**
--------------------------------

**Inherits from:** :ref:`condlim_base` 


External type exchange condition with a heat exchange coefficient and an imposed external 
temperature.

Usage:

**paroi_echange_externe_impose** **h_imp** **himpc** **text** **ch**  

Parameters are:

- **h_imp**  (*type:* string) Heat exchange coefficient value (expressed in W.m-2.K-1).

- **himpc**  (*type:* :ref:`front_field_base`) Boundary field type.

- **text**  (*type:* string) External temperature value (expressed in oC or K).

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_echange_externe_impose_h:

**paroi_echange_externe_impose_h**
----------------------------------

**Inherits from:** :ref:`condlim_base` 


Particular case of class paroi_echange_externe_impose for enthalpy equation.

Usage:

**paroi_echange_externe_impose_h** **h_imp** **himpc** **text** **ch**  

Parameters are:

- **h_imp**  (*type:* string) Heat exchange coefficient value (expressed in W.m-2.K-1).

- **himpc**  (*type:* :ref:`front_field_base`) Boundary field type.

- **text**  (*type:* string) External temperature value (expressed in oC or K).

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_echange_global_impose:

**paroi_echange_global_impose**
-------------------------------

**Inherits from:** :ref:`condlim_base` 


Global type exchange condition (internal) that is to say that diffusion on the first 
fluid mesh is not taken into consideration.

Usage:

**paroi_echange_global_impose** **h_imp** **himpc** **text** **ch**  

Parameters are:

- **h_imp**  (*type:* string) Global exchange coefficient value. The global exchange coefficient value is expressed in W.m-2.K-1.

- **himpc**  (*type:* :ref:`front_field_base`) Boundary field type.

- **text**  (*type:* string) External temperature value. The external temperature value is expressed in oC or K.

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_fixe:

**paroi_fixe**
--------------

**Inherits from:** :ref:`condlim_base` 


Keyword to designate a situation of adherence to the wall called bord (edge) (normal 
and tangential velocity at the edge is zero).

Usage:

**paroi_fixe**  


----

.. _paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets:

**paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets**
-----------------------------------------------------------------

**Inherits from:** :ref:`condlim_base` 


Boundary condition to obtain iso Geneppi2, without interest

Usage:

**paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets**  


----

.. _paroi_flux_impose:

**paroi_flux_impose**
---------------------

**Inherits from:** :ref:`condlim_base` 


Normal flux condition at the wall called bord (edge).
The surface area of the flux (W.m-1 in 2D or W.m-2 in 3D) is imposed at the boundary 
according to the following convention: a positive flux is a flux that enters into 
the domain according to convention.

Usage:

**paroi_flux_impose** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_knudsen_non_negligeable:

**paroi_knudsen_non_negligeable**
---------------------------------

**Inherits from:** :ref:`condlim_base` 


Boundary condition for number of Knudsen (Kn) above 0.001 where slip-flow condition 
appears: the velocity near the wall depends on the shear stress : Kn=l/L with l is 
the mean-free-path of the molecules and L a characteristic length scale.

U(y=0)-Uwall=k(dU/dY)

Where k is a coefficient given by several laws:

Mawxell : k=(2-s)*l/s

Bestok\&Karniadakis :k=(2-s)/s*L*Kn/(1+Kn)

Xue\&Fan :k=(2-s)/s*L*tanh(Kn)

s is a value between 0 and 2 named accomodation coefficient.
s=1 seems a good value.

Warning : The keyword is available for VDF calculation only for the moment.

Usage:

**paroi_knudsen_non_negligeable** **name_champ_1** **champ_1** **name_champ_2** **champ_2**  

Parameters are:

- **name_champ_1**  (*type:* string into ["vitesse_paroi", "k"])  Field name.

- **champ_1**  (*type:* :ref:`front_field_base`) Boundary field type.

- **name_champ_2**  (*type:* string into ["vitesse_paroi", "k"])  Field name.

- **champ_2**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _paroi_temperature_imposee:

**paroi_temperature_imposee**
-----------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed temperature condition at the wall called bord (edge).

Usage:

**paroi_temperature_imposee** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _periodic:

**periodic**
------------

**Synonyms:** periodique

**Inherits from:** :ref:`condlim_base` 


1).
For Navier-Stokes equations, this keyword is used to indicate that the horizontal 
inlet velocity values are the same as the outlet velocity values, at every moment.
As regards meshing, the inlet and outlet edges bear the same name.; 2).
For scalar transport equation, this keyword is used to set a periodic condition on 
scalar.
The two edges dealing with this periodic condition bear the same name.

Usage:

**periodic**  


----

.. _scalaire_impose_paroi:

**scalaire_impose_paroi**
-------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed temperature condition at the wall called bord (edge).

Usage:

**scalaire_impose_paroi** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _sortie_libre_temperature_imposee_h:

**sortie_libre_temperature_imposee_h**
--------------------------------------

**Inherits from:** :ref:`condlim_base` 


Open boundary for heat equation with enthalpy as unknown.

Usage:

**sortie_libre_temperature_imposee_h** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

.. _symetrie:

**symetrie**
------------

**Inherits from:** :ref:`condlim_base` 


1).
For Navier-Stokes equations, this keyword is used to designate a symmetry condition 
concerning the velocity at the boundary called bord (edge) (normal velocity at the 
edge equal to zero and tangential velocity gradient at the edge equal to zero); 2).
For scalar transport equation, this keyword is used to set a symmetry condition on 
scalar on the boundary named bord (edge).

Usage:

**symetrie**  


----

.. _temperature_imposee_paroi:

**temperature_imposee_paroi**
-----------------------------

**Inherits from:** :ref:`condlim_base` 


Imposed temperature condition at the wall called bord (edge).

Usage:

**temperature_imposee_paroi** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) Boundary field type.


----

**Keywords derived from condlims**
==================================

.. _condlims:

**condlims**
------------


Boundary conditions.


----

**Keywords derived from definition_champs**
===========================================

.. _definition_champs:

**definition_champs**
---------------------


List of definition champ


----

**Keywords derived from discretisation_base**
=============================================

.. _discretisation_base:

**discretisation_base**
-----------------------


Basic class for space discretization of thermohydraulic turbulent problems.

Usage:

| **discretisation_base** *str*
| **Read** *str* {
| }


----

.. _ef:

**ef**
------

**Inherits from:** :ref:`discretisation_base` 


Element Finite discretization.

Usage:

| **ef** *str*
| **Read** *str* {
| }


----

.. _polymac:

**polymac**
-----------

**Inherits from:** :ref:`discretisation_base` 


polymac discretization (polymac discretization that is not compatible with pb_multi).

Usage:

| **polymac** *str*
| **Read** *str* {
| }


----

.. _polymac_p0:

**polymac_p0**
--------------

**Inherits from:** :ref:`discretisation_base` 


polymac_p0 discretization (previously covimac discretization compatible with pb_multi).

Usage:

| **polymac_p0** *str*
| **Read** *str* {
| }


----

.. _polymac_p0p1nc:

**polymac_p0p1nc**
------------------

**Inherits from:** :ref:`discretisation_base` 


polymac_P0P1NC discretization (previously polymac discretization compatible with pb_multi).

Usage:

| **polymac_p0p1nc** *str*
| **Read** *str* {
| }


----

.. _vdf:

**vdf**
-------

**Inherits from:** :ref:`discretisation_base` 


Finite difference volume discretization.

Usage:

| **vdf** *str*
| **Read** *str* {
| }


----

.. _vef:

**vef**
-------

**Synonyms:** vefprep1b

**Inherits from:** :ref:`discretisation_base` 


Finite element volume discretization (P1NC/P1-bubble element).
Since the 1.5.5 version, several new discretizations are available thanks to the 
optional keyword Read.
By default, the VEFPreP1B keyword is equivalent to the former VEFPreP1B formulation 
(v1.5.4 and sooner).
P0P1 (if used with the strong formulation for imposed pressure boundary) is equivalent 
to VEFPreP1B but the convergence is slower.
VEFPreP1B dis is equivalent to VEFPreP1B dis Read dis { P0 P1 Changement_de_base_P1Bulle 
1 Cl_pression_sommet_faible 0 }

Usage:

| **vef** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p0]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p1]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pa]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[changement_de_base_p1bulle]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cl_pression_sommet_faible]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modif_div_face_dirichlet]**  int into [0,1]
| }

Parameters are:

- **[p0]**  (*type:* flag) Pressure nodes are added on element centres

- **[p1]**  (*type:* flag) Pressure nodes are added on vertices

- **[pa]**  (*type:* flag) Only available in 3D, pressure nodes are added on bones

- **[changement_de_base_p1bulle]**  (*type:* int into [0,1])  This option may be used to have the P1NC/P0P1 formulation (value  set to 0) or the P1NC/P1Bulle formulation (value set to 1, the default).

- **[cl_pression_sommet_faible]**  (*type:* int into [0,1])  This option is used to specify a strong formulation (value set  to 0, the default) or a weak formulation (value set to 1) for an imposed pressure  boundary condition. The first formulation converges quicker and is stable in general cases. The second formulation should be used if there are several outlet boundaries with  Neumann condition (see Ecoulement_Neumann test case for example).

- **[modif_div_face_dirichlet]**  (*type:* int into [0,1])  This option (by default 0) is used to extend control volumes  for the momentum equation.


----

**Keywords derived from domaine**
=================================

.. _domaine:

**domaine**
-----------


Keyword to create a domain.

Usage:

| **domaine** *str*
| **Read** *str* {
| }


----

.. _domaineaxi1d:

**domaineaxi1d**
----------------

**Inherits from:** :ref:`domaine` 


1D domain

Usage:

| **domaineaxi1d** *str*
| **Read** *str* {
| }


----

.. _ijk_grid_geometry:

**ijk_grid_geometry**
---------------------

**Inherits from:** :ref:`domaine` 


Object to define the grid that will represent the domain of the simulation in IJK 
discretization

Usage:

| **ijk_grid_geometry** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_i]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_j]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_k]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nbelem_i]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nbelem_j]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nbelem_k]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[uniform_domain_size_i]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[uniform_domain_size_j]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[uniform_domain_size_k]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[origin_i]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[origin_j]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[origin_k]**  double
| }

Parameters are:

- **[perio_i]**  (*type:* flag) flag to specify the border along the I direction is periodic

- **[perio_j]**  (*type:* flag) flag to specify the border along the J direction is periodic

- **[perio_k]**  (*type:* flag) flag to specify the border along the K direction is periodic

- **[nbelem_i]**  (*type:* int) the number of elements of the grid in the I direction

- **[nbelem_j]**  (*type:* int) the number of elements of the grid in the J direction

- **[nbelem_k]**  (*type:* int) the number of elements of the grid in the K direction

- **[uniform_domain_size_i]**  (*type:* double) the size of the elements along the I direction

- **[uniform_domain_size_j]**  (*type:* double) the size of the elements along the J direction

- **[uniform_domain_size_k]**  (*type:* double) the size of the elements along the K direction

- **[origin_i]**  (*type:* double) I-coordinate of the origin of the grid

- **[origin_j]**  (*type:* double) J-coordinate of the origin of the grid

- **[origin_k]**  (*type:* double) K-coordinate of the origin of the grid


----

**Keywords derived from field_base**
====================================

.. _champ_composite:

**champ_composite**
-------------------

**Inherits from:** :ref:`field_base` 


Composite field.
Used in multiphase problems to associate data to each phase.

Usage:

**champ_composite** **dim** **bloc**  

Parameters are:

- **dim**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) Values Various pieces of the field, defined per phase. Part 1 goes to phase 1, etc...


----

.. _champ_don_base:

**champ_don_base**
------------------

**Inherits from:** :ref:`field_base` 


Basic class for data fields (not calculated), p.e.
physics properties.

Usage:

| **champ_don_base** *str*
| **Read** *str* {
| }


----

.. _champ_don_lu:

**champ_don_lu**
----------------

**Inherits from:** :ref:`field_base` 


Field to read a data field (values located at the center of the cells) in a file.

Usage:

**champ_don_lu** **dom** **nb_comp** **file**  

Parameters are:

- **dom**  (*type:* :ref:`domaine`) Name of the domain.

- **nb_comp**  (*type:* int) Number of field components.

- **file**  (*type:* string) Name of the file.  This file has the following format:  nb_val_lues -> Number of values readen in th file  Xi Yi Zi -> Coordinates readen in the file  Ui Vi Wi -> Value of the field


----

.. _champ_fonc_fonction:

**champ_fonc_fonction**
-----------------------

**Inherits from:** :ref:`field_base` 


Field that is a function of another field.

Usage:

**champ_fonc_fonction** **problem_name** **inco** **expression**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of problem.

- **inco**  (*type:* string) Name of the field (for example: temperature).

- **expression**  (*type:* string list) Number of field components followed by the analytical expression for  each field component.


----

.. _champ_fonc_fonction_txyz:

**champ_fonc_fonction_txyz**
----------------------------

**Inherits from:** :ref:`field_base` 


this refers to a field that is a function of another field and time and/or space coordinates

Usage:

**champ_fonc_fonction_txyz** **problem_name** **inco** **expression**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of problem.

- **inco**  (*type:* string) Name of the field (for example: temperature).

- **expression**  (*type:* string list) Number of field components followed by the analytical expression for  each field component.


----

.. _champ_fonc_fonction_txyz_morceaux:

**champ_fonc_fonction_txyz_morceaux**
-------------------------------------

**Inherits from:** :ref:`field_base` 


Field defined by analytical functions in each sub-domaine.
It makes possible the definition of a field that depends on the time and the space.

Usage:

**champ_fonc_fonction_txyz_morceaux** **problem_name** **inco** **nb_comp** **data**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of the problem.

- **inco**  (*type:* string) Name of the field (for example: temperature).

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i } By default, the value val_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function,  val_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator  wishes to use a champ_fonc_fonction_txyz_morceaux type object.


----

.. _champ_fonc_interp:

**champ_fonc_interp**
---------------------

**Inherits from:** :ref:`field_base` 


Field that is interpolated from a distant domain via MEDCoupling (remapper).

Usage:

| **champ_fonc_interp** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_champ**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **pb_loc**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **pb_dist**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dom_loc]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dom_dist]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[default_value]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nature**  string
| }

Parameters are:

- **nom_champ**  (*type:* string) Name of the field (for example: temperature).

- **pb_loc**  (*type:* string) Name of the local problem.

- **pb_dist**  (*type:* string) Name of the distant problem.

- **[dom_loc]**  (*type:* string) Name of the local domain.

- **[dom_dist]**  (*type:* string) Name of the distant domain.

- **[default_value]**  (*type:* string) Name of the distant domain.

- **nature**  (*type:* string) Nature of the field (knowledge from MEDCoupling is required; IntensiveMaximum,  IntensiveConservation, ...).


----

.. _champ_fonc_med:

**champ_fonc_med**
------------------

**Inherits from:** :ref:`field_base` 


Field to read a data field in a MED-format file .med at a specified time.
It is very useful, for example, to resume a calculation with a new or refined geometry.
The field post-processed on the new geometry at med format is used as initial condition 
for the resume.

Usage:

| **champ_fonc_med** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_existing_domain]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[last_time]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[decoup]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mesh]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domain**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **file**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **field**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[loc]**  string into ["som", "elem"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[time]**  double
| }

Parameters are:

- **[use_existing_domain]**  (*type:* flag) whether to optimize the field loading by indicating that the field is supported  by the same mesh that was initially loaded as the domain

- **[last_time]**  (*type:* flag) to use the last time of the MED file instead of the specified time. Mutually exclusive with 'time' parameter.

- **[decoup]**  (*type:* string) specify a partition file.

- **[mesh]**  (*type:* string) Name of the mesh supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **domain**  (*type:* string) Name of the domain supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **file**  (*type:* string) Name of the .med file.

- **field**  (*type:* string) Name of field to load.

- **[loc]**  (*type:* string into ["som", "elem"])  To indicate where the field is localised. Default to 'elem'.

- **[time]**  (*type:* double) Timestep to load from the MED file. Mutually exclusive with 'last_time' flag.


----

.. _champ_fonc_med_table_temps:

**champ_fonc_med_table_temps**
------------------------------

**Inherits from:** :ref:`field_base` 


Field defined as a fixed spatial shape scaled by a temporal coefficient

Usage:

| **champ_fonc_med_table_temps** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[table_temps]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[table_temps_lue]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_existing_domain]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[last_time]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[decoup]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mesh]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domain**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **file**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **field**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[loc]**  string into ["som", "elem"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[time]**  double
| }

Parameters are:

- **[table_temps]**  (*type:* string) Table containing the temporal coefficient used to scale the field

- **[table_temps_lue]**  (*type:* string) Name of the file containing the values of the temporal coefficient used to  scale the field

- **[use_existing_domain]**  (*type:* flag) whether to optimize the field loading by indicating that the field is supported  by the same mesh that was initially loaded as the domain

- **[last_time]**  (*type:* flag) to use the last time of the MED file instead of the specified time. Mutually exclusive with 'time' parameter.

- **[decoup]**  (*type:* string) specify a partition file.

- **[mesh]**  (*type:* string) Name of the mesh supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **domain**  (*type:* string) Name of the domain supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **file**  (*type:* string) Name of the .med file.

- **field**  (*type:* string) Name of field to load.

- **[loc]**  (*type:* string into ["som", "elem"])  To indicate where the field is localised. Default to 'elem'.

- **[time]**  (*type:* double) Timestep to load from the MED file. Mutually exclusive with 'last_time' flag.


----

.. _champ_fonc_med_tabule:

**champ_fonc_med_tabule**
-------------------------

**Inherits from:** :ref:`field_base` 


not_set

Usage:

| **champ_fonc_med_tabule** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_existing_domain]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[last_time]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[decoup]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mesh]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domain**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **file**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **field**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[loc]**  string into ["som", "elem"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[time]**  double
| }

Parameters are:

- **[use_existing_domain]**  (*type:* flag) whether to optimize the field loading by indicating that the field is supported  by the same mesh that was initially loaded as the domain

- **[last_time]**  (*type:* flag) to use the last time of the MED file instead of the specified time. Mutually exclusive with 'time' parameter.

- **[decoup]**  (*type:* string) specify a partition file.

- **[mesh]**  (*type:* string) Name of the mesh supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **domain**  (*type:* string) Name of the domain supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create  the TRUST domain, loading can be optimized with option 'use_existing_domain'.

- **file**  (*type:* string) Name of the .med file.

- **field**  (*type:* string) Name of field to load.

- **[loc]**  (*type:* string into ["som", "elem"])  To indicate where the field is localised. Default to 'elem'.

- **[time]**  (*type:* double) Timestep to load from the MED file. Mutually exclusive with 'last_time' flag.


----

.. _champ_fonc_reprise:

**champ_fonc_reprise**
----------------------

**Inherits from:** :ref:`field_base` 


This field is used to read a data field in a save file (.xyz or .sauv) at a specified 
time.
It is very useful, for example, to run a thermohydraulic calculation with velocity 
initial condition read into a save file from a previous hydraulic calculation.

Usage:

**champ_fonc_reprise** **[format]** **filename** **pb_name** **champ** **[fonction]** **temps | time**  

Parameters are:

- **[format]**  (*type:* string into ["binaire", "formatte", "xyz", "single_hdf"])  Type of file (the file format). If xyz format is activated, the .xyz file from the previous calculation will be given  for filename, and if formatte or binaire is choosen, the .sauv file of the previous  calculation will be specified for filename. In the case of a parallel calculation, if the mesh partition does not changed between  the previous calculation and the next one, the binaire format should be preferred,  because is faster than the xyz format. If single_hdf is used, the same constraints/advantages as binaire apply, but a single  (HDF5) file is produced on the filesystem instead of having one file per processor.

- **filename**  (*type:* string) Name of the save file.

- **pb_name**  (*type:* :ref:`pb_base`) Name of the problem.

- **champ**  (*type:* string) Name of the problem unknown. It may also be the temporal average of a problem unknown (like moyenne_vitesse, moyenne_temperature,...)

- **[fonction]**  (*type:* :ref:`fonction_champ_reprise`) Optional keyword to apply a function on the field being read  in the save file (e.g. to read a temperature field in Celsius units and convert it for the calculation on  Kelvin units, you will use: fonction 1 273.+val )

- **temps | time**  (*type:* string) Time of the saved field in the save file or last_time. If you give the keyword last_time instead, the last time saved in the save file will  be used.


----

.. _champ_fonc_t:

**champ_fonc_t**
----------------

**Inherits from:** :ref:`field_base` 


Field that is constant in space and is a function of time.

Usage:

**champ_fonc_t** **val**  

Parameters are:

- **val**  (*type:* string list) Values of field components (time dependant functions).


----

.. _champ_fonc_tabule:

**champ_fonc_tabule**
---------------------

**Inherits from:** :ref:`field_base` 


Field that is tabulated as a function of another field.

Usage:

**champ_fonc_tabule** **inco** **dim** **bloc**  

Parameters are:

- **inco**  (*type:* string) Name of the field (for example: temperature).

- **dim**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) Values (the table (the value of the field at any time is calculated  by linear interpolation from this table) or the analytical expression (with keyword  expression to use an analytical expression)).


----

.. _champ_fonc_tabule_morceaux:

**champ_fonc_tabule_morceaux**
------------------------------

**Synonyms:** champ_tabule_morceaux

**Inherits from:** :ref:`field_base` 


Field defined by tabulated data in each sub-domaine.
It makes possible the definition of a field which is a function of other fields.

Usage:

**champ_fonc_tabule_morceaux** **domain_name** **nb_comp** **data**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i } By default, the value val_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function,  val_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator  wishes to use a champ_fonc_tabule_morceaux type object.


----

.. _champ_fonc_tabule_morceaux_interp:

**champ_fonc_tabule_morceaux_interp**
-------------------------------------

**Inherits from:** :ref:`field_base` 


Field defined by tabulated data in each sub-domaine.
It makes possible the definition of a field which is a function of other fields.
Here we use MEDCoupling to interpolate fields between the two domains.

Usage:

**champ_fonc_tabule_morceaux_interp** **problem_name** **nb_comp** **data**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of the problem.

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i } By default, the value val_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function,  val_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator  wishes to use a champ_fonc_tabule_morceaux type object.


----

.. _champ_init_canal_sinal:

**champ_init_canal_sinal**
--------------------------

**Inherits from:** :ref:`field_base` 


For a parabolic profile on U velocity with an unpredictable disturbance on V and W 
and a sinusoidal disturbance on V velocity.

Usage:

**champ_init_canal_sinal** **dim** **bloc**  

Parameters are:

- **dim**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lec_champ_init_canal_sinal`) Parameters for the class champ_init_canal_sinal.


----

.. _champ_input_base:

**champ_input_base**
--------------------

**Inherits from:** :ref:`field_base` 


not_set

Usage:

| **champ_input_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_comp**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_value]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **nb_comp**  (*type:* int) not_set

- **nom**  (*type:* string) not_set

- **[initial_value]**  (*type:* list) not_set

- **probleme**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _champ_input_p0:

**champ_input_p0**
------------------

**Inherits from:** :ref:`field_base` 


not_set

Usage:

| **champ_input_p0** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_comp**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_value]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **nb_comp**  (*type:* int) not_set

- **nom**  (*type:* string) not_set

- **[initial_value]**  (*type:* list) not_set

- **probleme**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _champ_input_p0_composite:

**champ_input_p0_composite**
----------------------------

**Inherits from:** :ref:`field_base` 


Field used to define a classical champ input p0 field (for ICoCo), but with a predefined 
field for the initial state.

Usage:

| **champ_input_p0_composite** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_field]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[input_field]**  :ref:`champ_input_p0`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_comp**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_value]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **[initial_field]**  (*type:* :ref:`field_base`) The field used for initialization

- **[input_field]**  (*type:* :ref:`champ_input_p0`) The input field for ICoCo

- **nb_comp**  (*type:* int) not_set

- **nom**  (*type:* string) not_set

- **[initial_value]**  (*type:* list) not_set

- **probleme**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _champ_musig:

**champ_musig**
---------------

**Inherits from:** :ref:`field_base` 


MUSIG field.
Used in multiphase problems to associate data to each phase.

Usage:

**champ_musig** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) Not set


----

.. _champ_ostwald:

**champ_ostwald**
-----------------

**Inherits from:** :ref:`field_base` 


This keyword is used to define the viscosity variation law:

Mu(T)= K(T)*(D:D/2)**((n-1)/2)

Usage:

**champ_ostwald**  


----

.. _champ_som_lu_vdf:

**champ_som_lu_vdf**
--------------------

**Inherits from:** :ref:`field_base` 


Keyword to read in a file values located at the nodes of a mesh in VDF discretization.

Usage:

**champ_som_lu_vdf** **domain_name** **dim** **tolerance** **file**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **dim**  (*type:* int) Value of the dimension of the field.

- **tolerance**  (*type:* double) Value of the tolerance to check the coordinates of the nodes.

- **file**  (*type:* string) name of the file  This file has the following format:  Xi Yi Zi -> Coordinates of the node  Ui Vi Wi -> Value of the field on this node  Xi+1 Yi+1 Zi+1 -> Next point  Ui+1 Vi+1 Zi+1 -> Next value ...


----

.. _champ_som_lu_vef:

**champ_som_lu_vef**
--------------------

**Inherits from:** :ref:`field_base` 


Keyword to read in a file values located at the nodes of a mesh in VEF discretization.

Usage:

**champ_som_lu_vef** **domain_name** **dim** **tolerance** **file**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **dim**  (*type:* int) Value of the dimension of the field.

- **tolerance**  (*type:* double) Value of the tolerance to check the coordinates of the nodes.

- **file**  (*type:* string) Name of the file.  This file has the following format:  Xi Yi Zi -> Coordinates of the node  Ui Vi Wi -> Value of the field on this node  Xi+1 Yi+1 Zi+1 -> Next point  Ui+1 Vi+1 Zi+1 -> Next value ...


----

.. _champ_tabule_temps:

**champ_tabule_temps**
----------------------

**Inherits from:** :ref:`field_base` 


Field that is constant in space and tabulated as a function of time.

Usage:

**champ_tabule_temps** **dim** **bloc**  

Parameters are:

- **dim**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) Values as a table. The value of the field at any time is calculated by linear interpolation from this  table.


----

.. _champ_uniforme_morceaux:

**champ_uniforme_morceaux**
---------------------------

**Inherits from:** :ref:`field_base` 


Field which is partly constant in space and stationary.

Usage:

**champ_uniforme_morceaux** **nom_dom** **nb_comp** **data**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of the domain to which the sub-areas belong.

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object value, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator  wishes to use a Champ_Uniforme_Morceaux(partly_uniform_field) type object.


----

.. _champ_uniforme_morceaux_tabule_temps:

**champ_uniforme_morceaux_tabule_temps**
----------------------------------------

**Inherits from:** :ref:`field_base` 


this type of field is constant in space on one or several sub_zones and tabulated 
as a function of time.

Usage:

**champ_uniforme_morceaux_tabule_temps** **nom_dom** **nb_comp** **data**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of the domain to which the sub-areas belong.

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object value, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator  wishes to use a Champ_Uniforme_Morceaux(partly_uniform_field) type object.


----

.. _field_base:

**field_base**
--------------

**Synonyms:** champ_base


Basic class of fields.

Usage:

| **field_base** *str*
| **Read** *str* {
| }


----

.. _field_func_txyz:

**field_func_txyz**
-------------------

**Synonyms:** champ_fonc_txyz

**Inherits from:** :ref:`field_base` 


Field defined by analytical functions.
It makes it possible the definition of a field that depends on the time and the space.

Usage:

**field_func_txyz** **dom** **val**  

Parameters are:

- **dom**  (*type:* :ref:`domaine`) Name of domain of calculation.

- **val**  (*type:* string list) List of functions on (t,x,y,z).


----

.. _field_func_xyz:

**field_func_xyz**
------------------

**Synonyms:** champ_fonc_xyz

**Inherits from:** :ref:`field_base` 


Field defined by analytical functions.
It makes it possible the definition of a field that depends on (x,y,z).

Usage:

**field_func_xyz** **dom** **val**  

Parameters are:

- **dom**  (*type:* :ref:`domaine`) Name of domain of calculation.

- **val**  (*type:* string list) List of functions on (x,y,z).


----

.. _init_par_partie:

**init_par_partie**
-------------------

**Inherits from:** :ref:`field_base` 


ne marche que pour n_comp=1

Usage:

**init_par_partie** **n_comp** **val1** **val2** **val3**  

Parameters are:

- **n_comp**  (*type:* int into [1])  not_set

- **val1**  (*type:* double) not_set

- **val2**  (*type:* double) not_set

- **val3**  (*type:* double) not_set


----

.. _tayl_green:

**tayl_green**
--------------

**Inherits from:** :ref:`field_base` 


Class Tayl_green.

Usage:

**tayl_green** **dim**  

Parameters are:

- **dim**  (*type:* int) Dimension.


----

.. _uniform_field:

**uniform_field**
-----------------

**Synonyms:** champ_uniforme

**Inherits from:** :ref:`field_base` 


Field that is constant in space and stationary.

Usage:

**uniform_field** **val**  

Parameters are:

- **val**  (*type:* list) Values of field components.


----

.. _valeur_totale_sur_volume:

**valeur_totale_sur_volume**
----------------------------

**Inherits from:** :ref:`field_base` 


Similar as Champ_Uniforme_Morceaux with the same syntax.
Used for source terms when we want to specify a source term with a value given for 
the volume (eg: heat in Watts) and not a value per volume unit (eg: heat in Watts/m3).

Usage:

**valeur_totale_sur_volume** **nom_dom** **nb_comp** **data**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of the domain to which the sub-areas belong.

- **nb_comp**  (*type:* int) Number of field components.

- **data**  (*type:* :ref:`bloc_lecture`) { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object value, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator  wishes to use a Champ_Uniforme_Morceaux(partly_uniform_field) type object.


----

**Keywords derived from front_field_base**
==========================================

.. _boundary_field_inward:

**boundary_field_inward**
-------------------------

**Inherits from:** :ref:`front_field_base` 


this field is used to define the normal vector field standard at the boundary in VDF 
or VEF discretization.

Usage:

| **boundary_field_inward** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **normal_value**  string
| }

Parameters are:

- **normal_value**  (*type:* string) normal vector value (positive value for a vector oriented outside to inside)  which can depend of the time.


----

.. _ch_front_input:

**ch_front_input**
------------------

**Inherits from:** :ref:`front_field_base` 


not_set

Usage:

| **ch_front_input** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_comp**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_value]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **nb_comp**  (*type:* int) not_set

- **nom**  (*type:* string) not_set

- **[initial_value]**  (*type:* list) not_set

- **probleme**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _ch_front_input_uniforme:

**ch_front_input_uniforme**
---------------------------

**Inherits from:** :ref:`front_field_base` 


for coupling, you can use ch_front_input_uniforme which is a champ_front_uniforme, 
which use an external value.
It must be used with Problem.setInputField.

Usage:

| **ch_front_input_uniforme** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_comp**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[initial_value]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **nb_comp**  (*type:* int) not_set

- **nom**  (*type:* string) not_set

- **[initial_value]**  (*type:* list) not_set

- **probleme**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _champ_front_bruite:

**champ_front_bruite**
----------------------

**Inherits from:** :ref:`front_field_base` 


Field which is variable in time and space in a random manner.

Usage:

**champ_front_bruite** **nb_comp** **bloc**  

Parameters are:

- **nb_comp**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) { [N val L val ] Moyenne m_1.....[m_i ] Amplitude A_1.....[A_ i ]}:  Random nois: If N and L are not defined, the ith component of the field varies randomly  around an average value m_i with a maximum amplitude A_i.  White noise: If N and L are defined, these two additional parameters correspond to  L, the domain length and N, the number of nodes in the domain. Noise frequency will be between 2*Pi/L and 2*Pi*N/(4*L).  For example, formula for velocity: u=U0(t) v=U1(t)Uj(t)=Mj+2*Aj*bruit_blanc where  bruit_blanc (white_noise) is the formula given in the mettre_a_jour (update) method  of the Champ_front_bruite (noise_boundary_field) (Refer to the Champ_front_bruite.cpp  file).


----

.. _champ_front_calc:

**champ_front_calc**
--------------------

**Inherits from:** :ref:`front_field_base` 


This keyword is used on a boundary to get a field from another boundary.
The local and remote boundaries should have the same mesh.
If not, the Champ_front_recyclage keyword could be used instead.
It is used in the condition block at the limits of equation which itself refers to 
a problem called pb1.
We are working under the supposition that pb1 is coupled to another problem.

Usage:

**champ_front_calc** **problem_name** **bord** **field_name**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of the other problem to which pb1 is coupled.

- **bord**  (*type:* string) Name of the side which is the boundary between the 2 domains in the domain  object description associated with the problem_name object.

- **field_name**  (*type:* string) Name of the field containing the value that the user wishes to use at the  boundary. The field_name object must be recognized by the problem_name object.


----

.. _champ_front_composite:

**champ_front_composite**
-------------------------

**Inherits from:** :ref:`front_field_base` 


Composite front field.
Used in multiphase problems to associate data to each phase.

Usage:

**champ_front_composite** **dim** **bloc**  

Parameters are:

- **dim**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) Values Various pieces of the field, defined per phase. Part 1 goes to phase 1, etc...


----

.. _champ_front_contact_vef:

**champ_front_contact_vef**
---------------------------

**Inherits from:** :ref:`front_field_base` 


This field is used on a boundary between a solid and fluid domain to exchange a calculated 
temperature at the contact face of the two domains according to the flux of the two 
problems.

Usage:

**champ_front_contact_vef** **local_pb** **local_boundary** **remote_pb** **remote_boundary**  

Parameters are:

- **local_pb**  (*type:* :ref:`pb_base`) Name of the problem.

- **local_boundary**  (*type:* string) Name of the boundary.

- **remote_pb**  (*type:* :ref:`pb_base`) Name of the second problem.

- **remote_boundary**  (*type:* string) Name of the boundary in the second problem.


----

.. _champ_front_debit:

**champ_front_debit**
---------------------

**Inherits from:** :ref:`front_field_base` 


This field is used to define a flow rate field instead of a velocity field for a Dirichlet 
boundary condition on Navier-Stokes equations.

Usage:

**champ_front_debit** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_txyz  that depends only on time.


----

.. _champ_front_debit_massique:

**champ_front_debit_massique**
------------------------------

**Inherits from:** :ref:`front_field_base` 


This field is used to define a flow rate field using the density

Usage:

**champ_front_debit_massique** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`front_field_base`) uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_txyz  that depends only on time.


----

.. _champ_front_debit_qc_vdf:

**champ_front_debit_qc_vdf**
----------------------------

**Inherits from:** :ref:`front_field_base` 


This keyword is used to define a flow rate field for quasi-compressible fluids in 
VDF discretization.
The flow rate is kept constant during a transient.

Usage:

**champ_front_debit_qc_vdf** **dimension | dim** **liste** **[moyen]** **pb_name**  

Parameters are:

- **dimension | dim**  (*type:* int) Problem dimension

- **liste**  (*type:* :ref:`bloc_lecture`) List of the mass flow rate values [kg/s/m2] with the following syntaxe:  { val1 ... valdim }

- **[moyen]**  (*type:* string) Option to use rho mean value

- **pb_name**  (*type:* string) Problem name


----

.. _champ_front_debit_qc_vdf_fonc_t:

**champ_front_debit_qc_vdf_fonc_t**
-----------------------------------

**Inherits from:** :ref:`front_field_base` 


This keyword is used to define a flow rate field for quasi-compressible fluids in 
VDF discretization.
The flow rate could be constant or time-dependent.

Usage:

**champ_front_debit_qc_vdf_fonc_t** **dimension | dim** **liste** **[moyen]** **pb_name**  

Parameters are:

- **dimension | dim**  (*type:* int) Problem dimension

- **liste**  (*type:* :ref:`bloc_lecture`) List of the mass flow rate values [kg/s/m2] with the following syntaxe:  { val1 ... valdim } where val1 ... valdim are constant or function of time.

- **[moyen]**  (*type:* string) Option to use rho mean value

- **pb_name**  (*type:* string) Problem name


----

.. _champ_front_fonc_pois_ipsn:

**champ_front_fonc_pois_ipsn**
------------------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field champ_front_fonc_pois_ipsn.

Usage:

**champ_front_fonc_pois_ipsn** **r_tube** **umoy** **r_loc**  

Parameters are:

- **r_tube**  (*type:* double) not_set

- **umoy**  (*type:* list) not_set

- **r_loc**  (*type:* double list) not_set


----

.. _champ_front_fonc_pois_tube:

**champ_front_fonc_pois_tube**
------------------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field champ_front_fonc_pois_tube.

Usage:

**champ_front_fonc_pois_tube** **r_tube** **umoy** **r_loc** **r_loc_mult**  

Parameters are:

- **r_tube**  (*type:* double) not_set

- **umoy**  (*type:* list) not_set

- **r_loc**  (*type:* double list) not_set

- **r_loc_mult**  (*type:* int list - size is dimension) not_set


----

.. _champ_front_fonc_t:

**champ_front_fonc_t**
----------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field that depends only on time.

Usage:

**champ_front_fonc_t** **val**  

Parameters are:

- **val**  (*type:* string list) Values of field components (mathematical expressions).


----

.. _champ_front_fonc_txyz:

**champ_front_fonc_txyz**
-------------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field which is not constant in space and in time.

Usage:

**champ_front_fonc_txyz** **val**  

Parameters are:

- **val**  (*type:* string list) Values of field components (mathematical expressions).


----

.. _champ_front_fonc_xyz:

**champ_front_fonc_xyz**
------------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field which is not constant in space.

Usage:

**champ_front_fonc_xyz** **val**  

Parameters are:

- **val**  (*type:* string list) Values of field components (mathematical expressions).


----

.. _champ_front_fonction:

**champ_front_fonction**
------------------------

**Inherits from:** :ref:`front_field_base` 


boundary field that is function of another field

Usage:

**champ_front_fonction** **dim** **inco** **expression**  

Parameters are:

- **dim**  (*type:* int) Number of field components.

- **inco**  (*type:* string) Name of the field (for example: temperature).

- **expression**  (*type:* string) keyword to use a analytical expression like 10.*EXP(-0.1*val) where val be  the keyword for the field.


----

.. _champ_front_lu:

**champ_front_lu**
------------------

**Inherits from:** :ref:`front_field_base` 


boundary field which is given from data issued from a read file.
The format of this file has to be the same that the one generated by Ecrire_fichier_xyz_valeur

Example for K and epsilon quantities to be defined for inlet condition in a boundary 
named 'entree': NL2entree frontiere_ouverte_K_Eps_impose Champ_Front_lu dom 2pb_K_EPS_PERIO_1006.306198.dat

Usage:

**champ_front_lu** **domaine | domain** **dim** **file**  

Parameters are:

- **domaine | domain**  (*type:* :ref:`domaine`) Name of domain

- **dim**  (*type:* int) number of components

- **file**  (*type:* string) path for the read file


----

.. _champ_front_med:

**champ_front_med**
-------------------

**Inherits from:** :ref:`front_field_base` 


Field allowing the loading of a boundary condition from a MED file using Champ_fonc_med

Usage:

**champ_front_med** **champ_fonc_med**  

Parameters are:

- **champ_fonc_med**  (*type:* :ref:`field_base`) a champ_fonc_med loading the values of the unknown on a domain boundary


----

.. _champ_front_musig:

**champ_front_musig**
---------------------

**Inherits from:** :ref:`front_field_base` 


MUSIG front field.
Used in multiphase problems to associate data to each phase.

Usage:

**champ_front_musig** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) Not set


----

.. _champ_front_normal_vef:

**champ_front_normal_vef**
--------------------------

**Inherits from:** :ref:`front_field_base` 


Field to define the normal vector field standard at the boundary in VEF discretization.

Usage:

**champ_front_normal_vef** **mot** **vit_tan**  

Parameters are:

- **mot**  (*type:* string into ["valeur_normale"])  Name of vector field.

- **vit_tan**  (*type:* double) normal vector value (positive value for a vector oriented outside to inside).


----

.. _champ_front_pression_from_u:

**champ_front_pression_from_u**
-------------------------------

**Inherits from:** :ref:`front_field_base` 


this field is used to define a pressure field depending of a velocity field.

Usage:

**champ_front_pression_from_u** **expression**  

Parameters are:

- **expression**  (*type:* string) value depending of a velocity (like $2*u_moy^2$).


----

.. _champ_front_recyclage:

**champ_front_recyclage**
-------------------------

**Inherits from:** :ref:`front_field_base` 


\input{{champfrontrecyclage}}

Usage:

**champ_front_recyclage** **bloc**  

Parameters are:

- **bloc**  (*type:* string) not_set


----

.. _champ_front_tabule:

**champ_front_tabule**
----------------------

**Inherits from:** :ref:`front_field_base` 


Constant field on the boundary, tabulated as a function of time.

Usage:

**champ_front_tabule** **nb_comp** **bloc**  

Parameters are:

- **nb_comp**  (*type:* int) Number of field components.

- **bloc**  (*type:* :ref:`bloc_lecture`) {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ... un [vn wn ...] }  Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1. The value of a field at a given time is calculated by linear interpolation from this  table.


----

.. _champ_front_tabule_lu:

**champ_front_tabule_lu**
-------------------------

**Inherits from:** :ref:`front_field_base` 


Constant field on the boundary, tabulated from a specified column file.
Lines starting with # are ignored.

Usage:

**champ_front_tabule_lu** **nb_comp** **column_file**  

Parameters are:

- **nb_comp**  (*type:* int) Number of field components.

- **column_file**  (*type:* string) Name of the column file.


----

.. _champ_front_tangentiel_vef:

**champ_front_tangentiel_vef**
------------------------------

**Inherits from:** :ref:`front_field_base` 


Field to define the tangential velocity vector field standard at the boundary in VEF 
discretization.

Usage:

**champ_front_tangentiel_vef** **mot** **vit_tan**  

Parameters are:

- **mot**  (*type:* string into ["vitesse_tangentielle"])  Name of vector field.

- **vit_tan**  (*type:* double) Vector field standard [m/s].


----

.. _champ_front_uniforme:

**champ_front_uniforme**
------------------------

**Inherits from:** :ref:`front_field_base` 


Boundary field which is constant in space and stationary.

Usage:

**champ_front_uniforme** **val**  

Parameters are:

- **val**  (*type:* list) Values of field components.


----

.. _champ_front_xyz_debit:

**champ_front_xyz_debit**
-------------------------

**Inherits from:** :ref:`front_field_base` 


This field is used to define a flow rate field with a velocity profil which will be 
normalized to match the flow rate chosen.

Usage:

| **champ_front_xyz_debit** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[velocity_profil]**  :ref:`front_field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **flow_rate**  :ref:`front_field_base`
| }

Parameters are:

- **[velocity_profil]**  (*type:* :ref:`front_field_base`) velocity_profil 0 velocity field to define the profil of velocity.

- **flow_rate**  (*type:* :ref:`front_field_base`) flow_rate 1 uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_t


----

.. _champ_front_xyz_tabule:

**champ_front_xyz_tabule**
--------------------------

**Inherits from:** :ref:`front_field_base` 


Space dependent field on the boundary, tabulated as a function of time.

Usage:

**champ_front_xyz_tabule** **val** **bloc**  

Parameters are:

- **val**  (*type:* string list) Values of field components (mathematical expressions).

- **bloc**  (*type:* :ref:`bloc_lecture`) {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ... un [vn wn ...] }  Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1. The value of a field at a given time is calculated by linear interpolation from this  table.


----

.. _front_field_base:

**front_field_base**
--------------------

**Synonyms:** champ_front_base


Basic class for fields at domain boundaries.

Usage:

| **front_field_base** *str*
| **Read** *str* {
| }


----

**Keywords derived from ijk_splitting**
=======================================

.. _ijk_splitting:

**ijk_splitting**
-----------------


Object to specify how the domain will be divided between processors in IJK discretization

Usage:

| **ijk_splitting** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **ijk_grid_geometry**  :ref:`ijk_grid_geometry`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nproc_i**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nproc_j**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nproc_k**  int
| }

Parameters are:

- **ijk_grid_geometry**  (*type:* :ref:`ijk_grid_geometry`) the grid that will be splitted

- **nproc_i**  (*type:* int) the number of processors into which we will divide the grid following the  I direction

- **nproc_j**  (*type:* int) the number of processors into which we will divide the grid following the  J direction

- **nproc_k**  (*type:* int) the number of processors into which we will divide the grid following the  K direction


----

**Keywords derived from interpolation_ibm_base**
================================================

.. _interpolation_ibm_aucune:

**interpolation_ibm_aucune**
----------------------------

**Synonyms:** ibm_aucune

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): no interpolation.

Usage:

**interpolation_ibm_aucune** **[impr]** **[nb_histo_boxes_impr]**  

Parameters are:

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_base:

**interpolation_ibm_base**
--------------------------


Base class for all the interpolation methods available in the Immersed Boundary Method 
(IBM).

Usage:

**interpolation_ibm_base** **[impr]** **[nb_histo_boxes_impr]**  

Parameters are:

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_elem_fluid:

**interpolation_ibm_elem_fluid**
--------------------------------

**Synonyms:** ibm_element_fluide, interpolation_ibm_element_fluide

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): fluid element interpolation.

Usage:

| **interpolation_ibm_elem_fluid** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **correspondance_elements**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_histo_boxes_impr]**  int
| }

Parameters are:

- **points_fluides**  (*type:* :ref:`field_base`) Node field giving the projection of the point below (points_solides)  falling into the pure cell fluid

- **points_solides**  (*type:* :ref:`field_base`) Node field giving the projection of the node on the immersed boundary

- **elements_fluides**  (*type:* :ref:`field_base`) Node field giving the number of the element (cell) containing the pure  fluid point

- **correspondance_elements**  (*type:* :ref:`field_base`) Cell field giving the SALOME cell number

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_hybride:

**interpolation_ibm_hybride**
-----------------------------

**Synonyms:** ibm_hybride

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): hybrid (fluid/mean gradient) interpolation.

Usage:

| **interpolation_ibm_hybride** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **est_dirichlet**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **correspondance_elements**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_histo_boxes_impr]**  int
| }

Parameters are:

- **est_dirichlet**  (*type:* :ref:`field_base`) Node field of booleans indicating whether the node belong to an element  where the interface is

- **elements_solides**  (*type:* :ref:`field_base`) Node field giving the element number containing the solid point

- **points_fluides**  (*type:* :ref:`field_base`) Node field giving the projection of the point below (points_solides)  falling into the pure cell fluid

- **points_solides**  (*type:* :ref:`field_base`) Node field giving the projection of the node on the immersed boundary

- **elements_fluides**  (*type:* :ref:`field_base`) Node field giving the number of the element (cell) containing the pure  fluid point

- **correspondance_elements**  (*type:* :ref:`field_base`) Cell field giving the SALOME cell number

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_mean_gradient:

**interpolation_ibm_mean_gradient**
-----------------------------------

**Synonyms:** ibm_gradient_moyen, interpolation_ibm_gradient_moyen

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): mean gradient interpolation.

Usage:

| **interpolation_ibm_mean_gradient** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **est_dirichlet**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **correspondance_elements**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_histo_boxes_impr]**  int
| }

Parameters are:

- **points_solides**  (*type:* :ref:`field_base`) Node field giving the projection of the node on the immersed boundary

- **est_dirichlet**  (*type:* :ref:`field_base`) Node field of booleans indicating whether the node belong to an element  where the interface is

- **correspondance_elements**  (*type:* :ref:`field_base`) Cell field giving the SALOME cell number

- **elements_solides**  (*type:* :ref:`field_base`) Node field giving the element number containing the solid point

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_power_law_tbl:

**interpolation_ibm_power_law_tbl**
-----------------------------------

**Synonyms:** ibm_power_law_tbl

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): power law interpolation.

Usage:

| **interpolation_ibm_power_law_tbl** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[formulation_linear_pwl]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_fluides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **correspondance_elements**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_histo_boxes_impr]**  int
| }

Parameters are:

- **[formulation_linear_pwl]**  (*type:* int) Choix formulation lineaire ou non

- **points_fluides**  (*type:* :ref:`field_base`) Node field giving the projection of the point below (points_solides)  falling into the pure cell fluid

- **points_solides**  (*type:* :ref:`field_base`) Node field giving the projection of the node on the immersed boundary

- **elements_fluides**  (*type:* :ref:`field_base`) Node field giving the number of the element (cell) containing the pure  fluid point

- **correspondance_elements**  (*type:* :ref:`field_base`) Cell field giving the SALOME cell number

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

.. _interpolation_ibm_power_law_tbl_u_star:

**interpolation_ibm_power_law_tbl_u_star**
------------------------------------------

**Synonyms:** ibm_power_law_tbl_u_star

**Inherits from:** :ref:`interpolation_ibm_base` 


Immersed Boundary Method (IBM): law u star.

Usage:

| **interpolation_ibm_power_law_tbl_u_star** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **points_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **est_dirichlet**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **correspondance_elements**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **elements_solides**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_histo_boxes_impr]**  int
| }

Parameters are:

- **points_solides**  (*type:* :ref:`field_base`) Node field giving the projection of the node on the immersed boundary

- **est_dirichlet**  (*type:* :ref:`field_base`) Node field of booleans indicating whether the node belong to an element  where the interface is

- **correspondance_elements**  (*type:* :ref:`field_base`) Cell field giving the SALOME cell number

- **elements_solides**  (*type:* :ref:`field_base`) Node field giving the element number containing the solid point

- **[impr]**  (*type:* flag) To print IBM-related data

- **[nb_histo_boxes_impr]**  (*type:* int) number of histogram boxes for printed data


----

**Keywords derived from interprete**
====================================

.. _interprete:

**interprete**
--------------

Class 'interprete_Tru' has nothing special in itself, except that it needs
to be known early, so that the Dataset_Tru class can test whether a given class
is a child of 'interprete_Tru'
So force its definition here, and hence avoid its automatic generation from the
TRAD2 file.

Usage:

**interprete**  


----

.. _analyse_angle:

**analyse_angle**
-----------------

**Inherits from:** :ref:`interprete` 


Keyword Analyse_angle prints the histogram of the largest angle of each mesh elements 
of the domain named name_domain.
nb_histo is the histogram number of bins.
It is called by default during the domain discretization with nb_histo set to 18.
Useful to check the number of elements with angles above 90 degrees.

Usage:

**analyse_angle** **domain_name** **nb_histo**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain to resequence.

- **nb_histo**  (*type:* int) not_set


----

.. _associate:

**associate**
-------------

**Synonyms:** associer

**Inherits from:** :ref:`interprete` 


This interpretor allows one object to be associated with another.
The order of the two objects in this instruction is not important.
The object objet_2 is associated to objet_1 if this makes sense; if not either objet_1 
is associated to objet_2 or the program exits with error because it cannot execute 
the Associate (Associer) instruction.
For example, to calculate water flow in a pipe, a Pb_Hydraulique type object needs 
to be defined.
But also a Domaine type object to represent the pipe, a Scheme_euler_explicit type 
object for time discretization, a discretization type object (VDF or VEF) and a Fluide_Incompressible 
type object which will contain the water properties.
These objects must then all be associated with the problem.

Usage:

**associate** **objet_1** **objet_2**  

Parameters are:

- **objet_1**  (*type:* string) Objet_1

- **objet_2**  (*type:* string) Objet_2


----

.. _axi:

**axi**
-------

**Inherits from:** :ref:`interprete` 


This keyword allows a 3D calculation to be executed using cylindrical coordinates 
(R,$\jolitheta$,Z).
If this instruction is not included, calculations are carried out using Cartesian 
coordinates.

Usage:

**axi**  


----

.. _bidim_axi:

**bidim_axi**
-------------

**Inherits from:** :ref:`interprete` 


Keyword allowing a 2D calculation to be executed using axisymetric coordinates (R, 
Z).
If this instruction is not included, calculations are carried out using Cartesian 
coordinates.

Usage:

**bidim_axi**  


----

.. _calculer_moments:

**calculer_moments**
--------------------

**Inherits from:** :ref:`interprete` 


Calculates and prints the torque (moment of force) exerted by the fluid on each boundary 
in output files (.out) of the domain nom_dom.

Usage:

**calculer_moments** **nom_dom** **mot**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of domain.

- **mot**  (*type:* :ref:`lecture_bloc_moment_base`) Keyword.


----

.. _corriger_frontiere_periodique:

**corriger_frontiere_periodique**
---------------------------------

**Inherits from:** :ref:`interprete` 


The Corriger_frontiere_periodique keyword is mandatory to first define the periodic 
boundaries, to reorder the faces and eventually fix unaligned nodes of these boundaries.
Faces on one side of the periodic domain are put first, then the faces on the opposite 
side, in the same order.
It must be run in sequential before mesh splitting.

Usage:

| **corriger_frontiere_periodique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **bord**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[direction]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_post]**  string
| }

Parameters are:

- **domaine**  (*type:* string) Name of domain.

- **bord**  (*type:* string) the name of the boundary (which must contain two opposite sides of the domain)

- **[direction]**  (*type:* list) defines the periodicity direction vector (a vector that points from one node  on one side to the opposite node on the other side). This vector must be given if the automatic algorithm fails, that is:NL2 - when the  node coordinates are not perfectly periodic  - when the periodic direction is not aligned with the normal vector of the boundary  faces

- **[fichier_post]**  (*type:* string) .


----

.. _create_domain_from_sous_zone:

**create_domain_from_sous_zone**
--------------------------------

**Synonyms:** create_domain_from_sub_domains, create_domain_from_sub_domain

**Inherits from:** :ref:`interprete` 


kept for backward compatibility.
please use Create_domain_from_sub_domain

Usage:

| **create_domain_from_sous_zone** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine_final]**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[par_sous_zone]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine_init**  :ref:`domaine`
| }

Parameters are:

- **[domaine_final]**  (*type:* :ref:`domaine`) new domain in which faces are stored

- **[par_sous_zone]**  (*type:* string) a sub-area allowing to choose the elements

- **domaine_init**  (*type:* :ref:`domaine`) initial domain


----

.. _create_domain_from_sub_domain:

**create_domain_from_sub_domain**
---------------------------------

**Synonyms:** create_domain_from_sub_domains, create_domain_from_sous_zone

**Inherits from:** :ref:`interprete` 


This keyword fills the domain domaine_final with the subdomaine par_sous_zone from 
the domain domaine_init.
It is very useful when meshing several mediums with Gmsh.
Each medium will be defined as a subdomaine into Gmsh.
A MED mesh file will be saved from Gmsh and read with Lire_Med keyword by the TRUST 
data file.
And with this keyword, a domain will be created for each medium in the TRUST data 
file.

Usage:

| **create_domain_from_sub_domain** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine_final]**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[par_sous_zone]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine_init**  :ref:`domaine`
| }

Parameters are:

- **[domaine_final]**  (*type:* :ref:`domaine`) new domain in which faces are stored

- **[par_sous_zone]**  (*type:* string) a sub-area allowing to choose the elements

- **domaine_init**  (*type:* :ref:`domaine`) initial domain


----

.. _criteres_convergence:

**criteres_convergence**
------------------------

**Inherits from:** :ref:`interprete` 


convergence criteria

Usage:

**criteres_convergence** **aco** **[inco]** **[val]** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **[inco]**  (*type:* string) Unknown (i.e: alpha, temperature, velocity and pressure)

- **[val]**  (*type:* double) Convergence threshold

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _debog:

**debog**
---------

**Inherits from:** :ref:`interprete` 


Class to debug some differences between two TRUST versions on a same data file.

If you want to compare the results of the same code in sequential and parallel calculation, 
first run (mode=0) in sequential mode (the files fichier1 and fichier2 will be written 
first) then the second run in parallel calculation (mode=1).

During the first run (mode=0), it prints into the file DEBOG, values at different 
points of the code thanks to the C++ instruction call.
see for example in Kernel/Framework/Resoudre.cpp file the instruction: Debog::verifier(msg,value); 
Where msg is a string and value may be a double, an integer or an array.

During the second run (mode=1), it prints into a file Err_Debog.dbg the same messages 
than in the DEBOG file and checks if the differences between results from both codes 
are less than a given value (error).
If not, it prints Ok else show the differences and the lines where it occured.

Usage:

**debog** **pb** **fichier1 | file1** **fichier2 | file2** **seuil** **mode**  

Parameters are:

- **pb**  (*type:* :ref:`pb_gen_base`) Name of the problem to debug.

- **fichier1 | file1**  (*type:* string) Name of the file where domain will be written in sequential calculation.

- **fichier2 | file2**  (*type:* string) Name of the file where faces will be written in sequential calculation.

- **seuil**  (*type:* double) Minimal value (by default 1.e-20) for the differences between the two  codes.

- **mode**  (*type:* int) By default -1 (nothing is written in the different files), you will set 0  for the sequential run, and 1 for the parallel run.


----

.. _debut_bloc:

**debut_bloc**
--------------

**Synonyms:** {

**Inherits from:** :ref:`interprete` 


Block's beginning.

Usage:

**debut_bloc**  


----

.. _decoupebord_pour_rayonnement:

**decoupebord_pour_rayonnement**
--------------------------------

**Synonyms:** decoupebord

**Inherits from:** :ref:`interprete` 


To subdivide the external boundary of a domain into several parts (may be useful for 
better accuracy when using radiation model in transparent medium).
To specify the boundaries of the fine_domain_name domain to be splitted.
These boundaries will be cut according the coarse mesh defined by either the keyword 
domaine_grossier (each boundary face of the coarse mesh coarse_domain_name will be 
used to group boundary faces of the fine mesh to define a new boundary), either by 
the keyword nb_parts_naif (each boundary of the fine mesh is splitted into a partition 
with nx*ny*nz elements), either by a geometric condition given by a formulae with 
the keyword condition_geometrique.
If used, the coarse_domain_name domain should have the same boundaries name of the 
fine_domain_name domain.

A mesh file (ASCII format, except if binaire option is specified) named by default 
newgeom (or specified by the nom_fichier_sortie keyword) will be created and will 
contain the fine_domain_name domain with the splitted boundaries named boundary_name%I 
(where I is between from 0 and n-1).
Furthermore, several files named boundary_name%I and boundary_name_xv will be created, 
containing the definition of the subdived boundaries.
newgeom will be used to calculate view factors with geom2ansys script whereas only 
the boundary_name_xv files will be necessary for the radiation calculation.
The file listb will contain the list of the boundaries boundary_name%I.

Usage:

| **decoupebord_pour_rayonnement** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine_grossier]**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts_naif]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts_geom]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **bords_a_decouper**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_fichier_sortie]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[condition_geometrique]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[binaire]**  int
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) not_set

- **[domaine_grossier]**  (*type:* :ref:`domaine`) not_set

- **[nb_parts_naif]**  (*type:* int list) not_set

- **[nb_parts_geom]**  (*type:* int list) not_set

- **bords_a_decouper**  (*type:* string list) not_set

- **[nom_fichier_sortie]**  (*type:* string) not_set

- **[condition_geometrique]**  (*type:* string list) not_set

- **[binaire]**  (*type:* int) not_set


----

.. _decouper_bord_coincident:

**decouper_bord_coincident**
----------------------------

**Inherits from:** :ref:`interprete` 


In case of non-coincident meshes and a paroi_contact condition, run is stopped and 
two external files are automatically generated in VEF (connectivity_failed_boundary_name 
and connectivity_failed_pb_name.med).
In 2D, the keyword Decouper_bord_coincident associated to the connectivity_failed_boundary_name 
file allows to generate a new coincident mesh.

Usage:

**decouper_bord_coincident** **domain_name** **bord**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.

- **bord**  (*type:* string) connectivity_failed_boundary_name


----

.. _dilate:

**dilate**
----------

**Inherits from:** :ref:`interprete` 


Keyword to multiply the whole coordinates of the geometry.

Usage:

**dilate** **domain_name** **alpha**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.

- **alpha**  (*type:* double) Value of dilatation coefficient.


----

.. _dimension:

**dimension**
-------------

**Inherits from:** :ref:`interprete` 


Keyword allowing calculation dimensions to be set (2D or 3D), where dim is an integer 
set to 2 or 3.
This instruction is mandatory.

Usage:

**dimension** **dim**  

Parameters are:

- **dim**  (*type:* int into [2,3])  Number of dimensions.


----

.. _disable_tu:

**disable_tu**
--------------

**Inherits from:** :ref:`interprete` 


Flag to disable the writing of the .TU files

Usage:

**disable_tu**  


----

.. _discretiser_domaine:

**discretiser_domaine**
-----------------------

**Inherits from:** :ref:`interprete` 


Useful to discretize the domain domain_name (faces will be created) without defining 
a problem.

Usage:

**discretiser_domaine** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.


----

.. _discretize:

**discretize**
--------------

**Synonyms:** discretiser

**Inherits from:** :ref:`interprete` 


Keyword to discretise a problem problem_name according to the discretization dis.

IMPORTANT: A number of objects must be already associated (a domain, time scheme, 
central object) prior to invoking the Discretize (Discretiser) keyword.
The physical properties of this central object must also have been read.

Usage:

**discretize** **problem_name** **dis**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_gen_base`) Name of problem.

- **dis**  (*type:* :ref:`discretisation_base`) Name of the discretization object.


----

.. _distance_paroi:

**distance_paroi**
------------------

**Inherits from:** :ref:`interprete` 


Class to generate external file Wall_length.xyz devoted for instance, for mixing length 
modelling.
In this file, are saved the coordinates of each element (center of gravity) of dom 
domain and minimum distance between this point and boundaries (specified bords) that 
user specifies in data file (typically, those associated to walls).
A field Distance_paroi is available to post process the distance to the wall.

Usage:

**distance_paroi** **dom** **bords** **format**  

Parameters are:

- **dom**  (*type:* :ref:`domaine`) Name of domain.

- **bords**  (*type:* string list) Boundaries.

- **format**  (*type:* string into ["binaire", "formatte"])  Value for format may be binaire (a binary file  Wall_length.xyz is written) or formatte (moreover, a formatted file Wall_length_formatted.xyz  is written).


----

.. _ecrire_champ_med:

**ecrire_champ_med**
--------------------

**Inherits from:** :ref:`interprete` 


Keyword to write a field to MED format into a file.

Usage:

**ecrire_champ_med** **nom_dom** **nom_chp** **file**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) domain name

- **nom_chp**  (*type:* :ref:`field_base`) field name

- **file**  (*type:* string) file name


----

.. _ecrire_fichier_formatte:

**ecrire_fichier_formatte**
---------------------------

**Inherits from:** :ref:`interprete` 


Keyword to write the object of name name_obj to a file filename in ASCII format.

Usage:

**ecrire_fichier_formatte** **name_obj** **filename**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be written.

- **filename**  (*type:* string) Name of the file.


----

.. _ecrire_med:

**ecrire_med**
--------------

**Synonyms:** write_med

**Inherits from:** :ref:`interprete` 


Write a domain to MED format into a file.

Usage:

**ecrire_med** **nom_dom** **file**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of domain.

- **file**  (*type:* string) Name of file.


----

.. _ecriturelecturespecial:

**ecriturelecturespecial**
--------------------------

**Inherits from:** :ref:`interprete` 


Class to write or not to write a .xyz file on the disk at the end of the calculation.

Usage:

**ecriturelecturespecial** **type**  

Parameters are:

- **type**  (*type:* string) If set to 0, no xyz file is created. If set to EFichierBin, it uses prior 1.7.0 way of reading xyz files (now LecFicDiffuseBin). If set to EcrFicPartageBin, it uses prior 1.7.0 way of writing xyz files (now EcrFicPartageMPIIO).


----

.. _espece:

**espece**
----------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

| **espece** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **masse_molaire**  double
| }

Parameters are:

- **mu**  (*type:* :ref:`field_base`) Species dynamic viscosity value (kg.m-1.s-1).

- **cp**  (*type:* :ref:`field_base`) Species specific heat value (J.kg-1.K-1).

- **masse_molaire**  (*type:* double) Species molar mass.


----

.. _execute_parallel:

**execute_parallel**
--------------------

**Inherits from:** :ref:`interprete` 


This keyword allows to run several computations in parallel on processors allocated 
to TRUST.
The set of processors is split in N subsets and each subset will read and execute 
a different data file.
Error messages usualy written to stderr and stdout are redirected to .log files (journaling 
must be activated).

Usage:

| **execute_parallel** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **liste_cas**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_procs]**  int list
| }

Parameters are:

- **liste_cas**  (*type:* string list) N datafile1 ... datafileN. datafileX the name of a TRUST data file without the .data extension.

- **[nb_procs]**  (*type:* int list) nb_procs is the number of processors needed to run each data file. If not given, TRUST assumes that computations are sequential.


----

.. _export:

**export**
----------

**Inherits from:** :ref:`interprete` 


Class to make the object have a global range, if not its range will apply to the block 
only (the associated object will be destroyed on exiting the block).

Usage:

**export**  


----

.. _extract_2d_from_3d:

**extract_2d_from_3d**
----------------------

**Inherits from:** :ref:`interprete` 


Keyword to extract a 2D mesh by selecting a boundary of the 3D mesh.
To generate a 2D axisymmetric mesh prefer Extract_2Daxi_from_3D keyword.

Usage:

**extract_2d_from_3d** **dom3d** **bord** **dom2d**  

Parameters are:

- **dom3d**  (*type:* :ref:`domaine`) Domain name of the 3D mesh

- **bord**  (*type:* string) Boundary name. This boundary becomes the new 2D mesh and all the boundaries, in 3D, attached to  the selected boundary, give their name to the new boundaries, in 2D.

- **dom2d**  (*type:* string) Domain name of the new 2D mesh


----

.. _extract_2daxi_from_3d:

**extract_2daxi_from_3d**
-------------------------

**Inherits from:** :ref:`interprete` 


Keyword to extract a 2D axisymetric mesh by selecting a boundary of the 3D mesh.

Usage:

**extract_2daxi_from_3d** **dom3d** **bord** **dom2d**  

Parameters are:

- **dom3d**  (*type:* :ref:`domaine`) Domain name of the 3D mesh

- **bord**  (*type:* string) Boundary name. This boundary becomes the new 2D mesh and all the boundaries, in 3D, attached to  the selected boundary, give their name to the new boundaries, in 2D.

- **dom2d**  (*type:* string) Domain name of the new 2D mesh


----

.. _extraire_domaine:

**extraire_domaine**
--------------------

**Inherits from:** :ref:`interprete` 


Keyword to create a new domain built with the domain elements of the pb_name problem 
verifying the two conditions given by Condition_elements.
The problem pb_name should have been discretized.

Usage:

| **extraire_domaine** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  :ref:`pb_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[condition_elements]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) Domain in which faces are saved

- **probleme**  (*type:* :ref:`pb_base`) Problem from which faces should be extracted

- **[condition_elements]**  (*type:* string) not_set

- **[sous_zone]**  (*type:* :ref:`sous_zone`) not_set


----

.. _extraire_plan:

**extraire_plan**
-----------------

**Inherits from:** :ref:`interprete` 


This keyword extracts a plane mesh named domain_name (this domain should have been 
declared before) from the mesh of the pb_name problem.
The plane can be either a triangle (defined by the keywords Origine, Point1, Point2 
and Triangle), either a regular quadrangle (with keywords Origine, Point1 and Point2), 
or either a generalized quadrangle (with keywords Origine, Point1, Point2, Point3).
The keyword Epaisseur specifies the thickness of volume around the plane which contains 
the faces of the extracted mesh.
The keyword via_extraire_surface will create a plan and use Extraire_surface algorithm.
Inverse_condition_element keyword then will be used in the case where the plane is 
a boundary not well oriented, and avec_certains_bords_pour_extraire_surface is the 
option related to the Extraire_surface option named avec_certains_bords.

Usage:

| **extraire_plan** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  :ref:`pb_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **epaisseur**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **origine**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **point1**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **point2**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[point3]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[triangle]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[via_extraire_surface]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[inverse_condition_element]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[avec_certains_bords_pour_extraire_surface]**  string list
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) domain_namme

- **probleme**  (*type:* :ref:`pb_base`) pb_name

- **epaisseur**  (*type:* double) not_set

- **origine**  (*type:* list) not_set

- **point1**  (*type:* list) not_set

- **point2**  (*type:* list) not_set

- **[point3]**  (*type:* list) not_set

- **[triangle]**  (*type:* flag) not_set

- **[via_extraire_surface]**  (*type:* flag) not_set

- **[inverse_condition_element]**  (*type:* flag) not_set

- **[avec_certains_bords_pour_extraire_surface]**  (*type:* string list) not_set


----

.. _extraire_surface:

**extraire_surface**
--------------------

**Inherits from:** :ref:`interprete` 


This keyword extracts a surface mesh named domain_name (this domain should have been 
declared before) from the mesh of the pb_name problem.
The surface mesh is defined by one or two conditions.
The first condition is about elements with Condition_elements.
For example: Condition_elements x*x+y*y+z*z<1

Will define a surface mesh with external faces of the mesh elements inside the sphere 
of radius 1 located at (0,0,0).
The second condition Condition_faces is useful to give a restriction.NL2 By default, 
the faces from the boundaries are not added to the surface mesh excepted if option 
avec_les_bords is given (all the boundaries are added), or if the option avec_certains_bords 
is used to add only some boundaries.

Usage:

| **extraire_surface** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **probleme**  :ref:`pb_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[condition_elements]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[condition_faces]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[avec_les_bords]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[avec_certains_bords]**  string list
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) Domain in which faces are saved

- **probleme**  (*type:* :ref:`pb_base`) Problem from which faces should be extracted

- **[condition_elements]**  (*type:* string) not_set

- **[condition_faces]**  (*type:* string) not_set

- **[avec_les_bords]**  (*type:* flag) not_set

- **[avec_certains_bords]**  (*type:* string list) not_set


----

.. _extrudebord:

**extrudebord**
---------------

**Inherits from:** :ref:`interprete` 


Class to generate an extruded mesh from a boundary of a tetrahedral or an hexahedral 
mesh.

Warning: If the initial domain is a tetrahedral mesh, the boundary will be moved 
in the XY plane then extrusion will be applied (you should maybe use the Transformer 
keyword on the final domain to have the domain you really want).
You can use the keyword Ecrire_Fichier_Meshtv to generate a meshtv file to visualize 
your initial and final meshes.

This keyword can be used for example to create a periodic box extracted from a boundary 
of a tetrahedral or a hexaedral mesh.
This periodic box may be used then to engender turbulent inlet flow condition for 
the main domain.NL2 Note that ExtrudeBord in VEF generates 3 or 14 tetrahedra from 
extruded prisms.

Usage:

| **extrudebord** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine_init**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  double list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_tranches**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine_final**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_bord**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[hexa_old]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[trois_tetra]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[vingt_tetra]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sans_passer_par_le2d]**  int
| }

Parameters are:

- **domaine_init**  (*type:* :ref:`domaine`) Initial domain with hexaedras or tetrahedras.

- **direction**  (*type:* double list) Directions for the extrusion.

- **nb_tranches**  (*type:* int) Number of elements in the extrusion direction.

- **domaine_final**  (*type:* string) Extruded domain.

- **nom_bord**  (*type:* string) Name of the boundary of the initial domain where extrusion will be applied.

- **[hexa_old]**  (*type:* flag) Old algorithm for boundary extrusion from a hexahedral mesh.

- **[trois_tetra]**  (*type:* flag) To extrude in 3 tetrahedras instead of 14 tetrahedras.

- **[vingt_tetra]**  (*type:* flag) To extrude in 20 tetrahedras instead of 14 tetrahedras.

- **[sans_passer_par_le2d]**  (*type:* int) Only for non-regression


----

.. _extrudeparoi:

**extrudeparoi**
----------------

**Inherits from:** :ref:`interprete` 


Keyword dedicated in 3D (VEF) to create prismatic layer at wall.
Each prism is cut into 3 tetraedra.

Usage:

| **extrudeparoi** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine | domain_name**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_bord**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[epaisseur]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[critere_absolu]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_normale_bord]**  flag
| }

Parameters are:

- **domaine | domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **nom_bord**  (*type:* string) Name of the (no-slip) boundary for creation of prismatic layers.

- **[epaisseur]**  (*type:* list) n r1 r2 .... rn : (relative or absolute) width for each layer.

- **[critere_absolu]**  (*type:* int) relative (0, the default) or absolute (1) width for each layer.

- **[projection_normale_bord]**  (*type:* flag) keyword to project layers on the same plane that contiguous boundaries. defaut values are : epaisseur_relative 1 0.5 projection_normale_bord 1


----

.. _extruder:

**extruder**
------------

**Inherits from:** :ref:`interprete` 


Class to create a 3D tetrahedral/hexahedral mesh (a prism is cut in 14) from a 2D 
triangular/quadrangular mesh.

Usage:

| **extruder** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine | domain_name**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  :ref:`troisf`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_tranches**  int
| }

Parameters are:

- **domaine | domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **direction**  (*type:* :ref:`troisf`) Direction of the extrude operation.

- **nb_tranches**  (*type:* int) Number of elements in the extrusion direction.


----

.. _extruder_en20:

**extruder_en20**
-----------------

**Inherits from:** :ref:`interprete` 


It does the same task as Extruder except that a prism is cut into 20 tetraedra instead 
of 3.
The name of the boundaries will be devant (front) and derriere (back).
But you can change these names with the keyword RegroupeBord.

Usage:

| **extruder_en20** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine | domain_name**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[direction]**  :ref:`troisf`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_tranches**  int
| }

Parameters are:

- **domaine | domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **[direction]**  (*type:* :ref:`troisf`) 0 Direction of the extrude operation.

- **nb_tranches**  (*type:* int) Number of elements in the extrusion direction.


----

.. _extruder_en3:

**extruder_en3**
----------------

**Inherits from:** :ref:`interprete` 


Class to create a 3D tetrahedral/hexahedral mesh (a prism is cut in 3) from a 2D triangular/quadrangular 
mesh.
The names of the boundaries (by default, devant (front) and derriere (back)) may 
be edited by the keyword nom_cl_devant and nom_cl_derriere.
If NULL is written for nom_cl, then no boundary condition is generated at this place.

Recommendation : to ensure conformity between meshes (in case of fluid/solid coupling) 
it is recommended to extrude all the domains at the same time.

Usage:

| **extruder_en3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine | domain_name**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_cl_devant]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_cl_derriere]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  :ref:`troisf`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_tranches**  int
| }

Parameters are:

- **domaine | domain_name**  (*type:* string list) List of the domains

- **[nom_cl_devant]**  (*type:* string) New name of the first boundary.

- **[nom_cl_derriere]**  (*type:* string) New name of the second boundary.

- **direction**  (*type:* :ref:`troisf`) Direction of the extrude operation.

- **nb_tranches**  (*type:* int) Number of elements in the extrusion direction.


----

.. _fin_bloc:

**fin_bloc**
------------

**Synonyms:** }

**Inherits from:** :ref:`interprete` 


Block's end.

Usage:

**fin_bloc**  


----

.. _imprimer_flux:

**imprimer_flux**
-----------------

**Inherits from:** :ref:`interprete` 


This keyword prints the flux per face at the specified domain boundaries in the data 
set.
The fluxes are written to the .face files at a frequency defined by dt_impr, the 
evaluation printing frequency (refer to time scheme keywords).
By default, fluxes are incorporated onto the edges before being displayed.

Usage:

**imprimer_flux** **domain_name** **noms_bord**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **noms_bord**  (*type:* :ref:`bloc_lecture`) List of boundaries, for ex: { Bord1 Bord2 }


----

.. _imprimer_flux_sum:

**imprimer_flux_sum**
---------------------

**Inherits from:** :ref:`interprete` 


This keyword prints the sum of the flux per face at the domain boundaries defined 
by the user in the data set.
The fluxes are written into the .out files at a frequency defined by dt_impr, the 
evaluation printing frequency (refer to time scheme keywords).

Usage:

**imprimer_flux_sum** **domain_name** **noms_bord**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of the domain.

- **noms_bord**  (*type:* :ref:`bloc_lecture`) List of boundaries, for ex: { Bord1 Bord2 }


----

.. _integrer_champ_med:

**integrer_champ_med**
----------------------

**Inherits from:** :ref:`interprete` 


his keyword is used to calculate a flow rate from a velocity MED field read before.
The method is either debit_total to calculate the flow rate on the whole surface, 
either integrale_en_z to calculate flow rates between z=zmin and z=zmax on nb_tranche 
surfaces.
The output file indicates first the flow rate for the whole surface and then lists 
for each tranche : the height z, the surface average value, the surface area and the 
flow rate.
For the debit_total method, only one tranche is considered.NL2 file :z Sum(u.dS)/Sum(dS) 
Sum(dS) Sum(u.dS)

Usage:

| **integrer_champ_med** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **champ_med**  :ref:`champ_fonc_med`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **methode**  string into ["integrale_en_z", "debit_total"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[zmin]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[zmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_tranche]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_sortie]**  string
| }

Parameters are:

- **champ_med**  (*type:* :ref:`champ_fonc_med`) not_set

- **methode**  (*type:* string into ["integrale_en_z", "debit_total"])  to choose between the integral following  z or over the entire height (debit_total corresponds to zmin=-DMAXFLOAT, ZMax=DMAXFLOAT,  nb_tranche=1)

- **[zmin]**  (*type:* double) not_set

- **[zmax]**  (*type:* double) not_set

- **[nb_tranche]**  (*type:* int) not_set

- **[fichier_sortie]**  (*type:* string) name of the output file, by default: integrale.


----

.. _interprete_geometrique_base:

**interprete_geometrique_base**
-------------------------------

**Inherits from:** :ref:`interprete` 


Class for interpreting a data file

Usage:

**interprete_geometrique_base**  


----

.. _lata_to_med:

**lata_to_med**
---------------

**Inherits from:** :ref:`interprete` 


To convert results file written with LATA format to MED file.
Warning: Fields located on faces are not supported yet.

Usage:

**lata_to_med** **[format]** **file** **file_med**  

Parameters are:

- **[format]**  (*type:* :ref:`format_lata_to_med`) generated file post_med.data use format (MED or LATA or LML keyword).

- **file**  (*type:* string) LATA file to convert to the new format.

- **file_med**  (*type:* string) Name of the MED file.


----

.. _lata_to_other:

**lata_to_other**
-----------------

**Inherits from:** :ref:`interprete` 


To convert results file written with LATA format to MED or LML format.
Warning: Fields located at faces are not supported yet.

Usage:

**lata_to_other** **[format]** **file** **file_post**  

Parameters are:

- **[format]**  (*type:* string into ["lml", "lata", "lata_v2", "med"])  Results format (MED or LATA or LML keyword).

- **file**  (*type:* string) LATA file to convert to the new format.

- **file_post**  (*type:* string) Name of file post.


----

.. _lire_ideas:

**lire_ideas**
--------------

**Inherits from:** :ref:`interprete` 


Read a geom in a unv file.
3D tetra mesh elements only may be read by TRUST.

Usage:

**lire_ideas** **nom_dom** **file**  

Parameters are:

- **nom_dom**  (*type:* :ref:`domaine`) Name of domain.

- **file**  (*type:* string) Name of file.


----

.. _lml_to_lata:

**lml_to_lata**
---------------

**Inherits from:** :ref:`interprete` 


To convert results file written with LML format to a single LATA file.

Usage:

**lml_to_lata** **file_lml** **file_lata**  

Parameters are:

- **file_lml**  (*type:* string) LML file to convert to the new format.

- **file_lata**  (*type:* string) Name of the single LATA file.


----

.. _mailler:

**mailler**
-----------

**Inherits from:** :ref:`interprete` 


The Mailler (Mesh) interpretor allows a Domain type object domaine to be meshed with 
objects objet_1, objet_2, etc...

Usage:

**mailler** **domaine** **bloc**  

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) Name of domain.

- **bloc**  (*type:* :ref:`list_bloc_mailler`) Instructions to mesh.


----

.. _maillerparallel:

**maillerparallel**
-------------------

**Inherits from:** :ref:`interprete` 


creates a parallel distributed hexaedral mesh of a parallelipipedic box.
It is equivalent to creating a mesh with a single Pave, splitting it with Decouper 
and reloading it in parallel with Scatter.
It only works in 3D at this time.
It can also be used for a sequential computation (with all NPARTS=1)}

Usage:

| **maillerparallel** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domain**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nb_nodes**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **splitting**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **ghost_thickness**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_x]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_y]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[perio_z]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[function_coord_x]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[function_coord_y]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[function_coord_z]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[file_coord_x]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[file_coord_y]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[file_coord_z]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_xmin]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_xmax]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_ymin]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_ymax]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_zmin]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundary_zmax]**  string
| }

Parameters are:

- **domain**  (*type:* :ref:`domaine`) the name of the domain to mesh (it must be an empty domain object).

- **nb_nodes**  (*type:* int list) dimension defines the spatial dimension (currently only dimension=3 is  supported), and nX, nY and nZ defines the total number of nodes in the mesh in each  direction.

- **splitting**  (*type:* int list) dimension is the spatial dimension and npartsX, npartsY and npartsZ are  the number of parts created. The product of the number of parts must be equal to the number of processors used  for the computation.

- **ghost_thickness**  (*type:* int) he number of ghost cells (equivalent to the epaisseur_joint parameter of  Decouper.

- **[perio_x]**  (*type:* flag) change the splitting method to provide a valid mesh for periodic boundary conditions.

- **[perio_y]**  (*type:* flag) change the splitting method to provide a valid mesh for periodic boundary conditions.

- **[perio_z]**  (*type:* flag) change the splitting method to provide a valid mesh for periodic boundary conditions.

- **[function_coord_x]**  (*type:* string) By default, the meshing algorithm creates nX nY nZ coordinates ranging between  0 and 1 (eg a unity size box). If function_coord_x} is specified, it is used to transform the [0,1] segment to the  coordinates of the nodes. funcX must be a function of the x variable only.

- **[function_coord_y]**  (*type:* string) like function_coord_x for y

- **[function_coord_z]**  (*type:* string) like function_coord_x for z

- **[file_coord_x]**  (*type:* string) Keyword to read the Nx floating point values used as nodes coordinates in  the file.

- **[file_coord_y]**  (*type:* string) idem file_coord_x for y

- **[file_coord_z]**  (*type:* string) idem file_coord_x for z

- **[boundary_xmin]**  (*type:* string) the name of the boundary at the minimum X direction. If it not provided, the default boundary names are xmin, xmax, ymin, ymax, zmin and  zmax. If the mesh is periodic in a given direction, only the MIN boundary name is used,  for both sides of the box.

- **[boundary_xmax]**  (*type:* string) not_set

- **[boundary_ymin]**  (*type:* string) not_set

- **[boundary_ymax]**  (*type:* string) not_set

- **[boundary_zmin]**  (*type:* string) not_set

- **[boundary_zmax]**  (*type:* string) not_set


----

.. _merge_med:

**merge_med**
-------------

**Inherits from:** :ref:`interprete` 


This keyword allows to merge multiple MED files produced during a parallel computation 
into a single MED file.

Usage:

**merge_med** **med_files_base_name** **time_iterations**  

Parameters are:

- **med_files_base_name**  (*type:* string) Base name of multiple med files that should appear as base_name_xxxxx.med,  where xxxxx denotes the MPI rank number. If you specify NOM_DU_CAS, it will automatically take the basename from your datafile's  name.

- **time_iterations**  (*type:* string into ["all_times", "last_time"])  Identifies whether to merge all time iterations  present in the MED files or only the last one.


----

.. _modif_bord_to_raccord:

**modif_bord_to_raccord**
-------------------------

**Inherits from:** :ref:`interprete` 


Keyword to convert a boundary of domain_name domain of kind Bord to a boundary of 
kind Raccord (named boundary_name).
It is useful when using meshes with boundaries of kind Bord defined and to run a 
coupled calculation.

Usage:

**modif_bord_to_raccord** **domaine | domain** **nom_bord**  

Parameters are:

- **domaine | domain**  (*type:* :ref:`domaine`) Name of domain

- **nom_bord**  (*type:* string) Name of the boundary to transform.


----

.. _modifydomaineaxi1d:

**modifydomaineaxi1d**
----------------------

**Synonyms:** convert_1d_to_1daxi

**Inherits from:** :ref:`interprete` 


Convert a 1D mesh to 1D axisymmetric mesh

Usage:

**modifydomaineaxi1d** **dom** **bloc**  

Parameters are:

- **dom**  (*type:* string) not_set

- **bloc**  (*type:* :ref:`bloc_lecture`) not_set


----

.. _moyenne_volumique:

**moyenne_volumique**
---------------------

**Inherits from:** :ref:`interprete` 


This keyword should be used after Resoudre keyword.
It computes the convolution product of one or more fields with a given filtering 
function.

Usage:

| **moyenne_volumique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_pb**  :ref:`pb_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **nom_domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **noms_champs**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_fichier_post]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[format_post]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[localisation]**  string into ["elem", "som"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fonction_filtre**  :ref:`bloc_lecture`
| }

Parameters are:

- **nom_pb**  (*type:* :ref:`pb_base`) name of the problem where the source fields will be searched.

- **nom_domaine**  (*type:* :ref:`domaine`) name of the destination domain (for example, it can be a coarser mesh,  but for optimal performance in parallel, the domain should be split with the same  algorithm as the computation mesh, eg, same tranche parameters for example)

- **noms_champs**  (*type:* string list) name of the source fields (these fields must be accessible from the postraitement)  N source_field1 source_field2 ... source_fieldN

- **[nom_fichier_post]**  (*type:* string) indicates the filename where the result is written

- **[format_post]**  (*type:* string) gives the fileformat for the result (by default : lata)

- **[localisation]**  (*type:* string into ["elem", "som"])  indicates where the convolution product should be computed:  either on the elements or on the nodes of the destination domain.

- **fonction_filtre**  (*type:* :ref:`bloc_lecture`) to specify the given filter  Fonction_filtre {NL2 type filter_typeNL2 demie-largeur lNL2 [ omega w ]  [ expression string ]NL2 }  NL2 type filter_type : This parameter specifies the filtering function. Valid filter_type are:NL2 Boite is a box filter, $f(x,y,z)=(abs(x)<l)*(abs(y) <l)*(abs(z)  <l) / (8 l^3)$NL2 Chapeau is a hat filter (product of hat filters in each direction)  centered on the origin, the half-width of the filter being l and its integral being  1.NL2 Quadra is a 2nd order filter.NL2 Gaussienne is a normalized gaussian filter  of standard deviation sigma in each direction (all field elements outside a cubic  box defined by clipping_half_width are ignored, hence, taking clipping_half_width=2.5*sigma  yields an integral of 0.99 for a uniform unity field).NL2 Parser allows a user defined  function of the x,y,z variables. All elements outside a cubic box defined by clipping_half_width are ignored. The parser is much slower than the equivalent c++ coded function...NL2  demie-largeur l : This parameter specifies the half width of the filterNL2 [ omega  w ] : This parameter must be given for the gaussienne filter. It defines the standard deviation of the gaussian filter.NL2 [ expression string]  : This parameter must be given for the parser filter type. This expression will be interpreted by the math parser with the predefined variables  x, y and z.


----

.. _multigrid_solver:

**multigrid_solver**
--------------------

**Inherits from:** :ref:`interprete` 


Object defining a multigrid solver in IJK discretization

Usage:

| **multigrid_solver** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coarsen_operators]**  :ref:`coarsen_operators`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ghost_size]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[relax_jacobi]**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pre_smooth_steps]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[smooth_steps]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_full_mg_steps]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_grossier]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solver_precision]**  string into ["mixed", "double"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[iterations_mixed_solver]**  int
| }

Parameters are:

- **[coarsen_operators]**  (*type:* :ref:`coarsen_operators`) Definition of the number of grids that will be used, in addition  to the finest (original) grid, followed by the list of the coarsen operators that  will be applied to get those grids

- **[ghost_size]**  (*type:* int) Number of ghost cells known by each processor in each of the three directions

- **[relax_jacobi]**  (*type:* list) Parameter between 0 and 1 that will be used in the Jacobi method to solve equation  on each grid. Should be around 0.7

- **[pre_smooth_steps]**  (*type:* int list) First integer of the list indicates the numbers of integers that has  to be read next. Following integers define the numbers of iterations done before solving the equation  on each grid. For example, 2 7 8 means that we have a list of 2 integers, the first one tells us  to perform 7 pre-smooth steps on the first grid, the second one tells us to perform  8 pre-smooth steps on the second grid. If there are more than 2 grids in the solver, then the remaining ones will have as  many pre-smooth steps as the last mentionned number (here, 8)

- **[smooth_steps]**  (*type:* int list) First integer of the list indicates the numbers of integers that has  to be read next. Following integers define the numbers of iterations done after solving the equation  on each grid. Same behavior as pre_smooth_steps

- **[nb_full_mg_steps]**  (*type:* int list) Number of multigrid iterations at each level

- **[solveur_grossier]**  (*type:* :ref:`solveur_sys_base`) Name of the iterative solver that will be used to solve the system  on the coarsest grid. This resolution must be more precise than the ones occurring on the fine grids. The threshold of this solver must therefore be lower than seuil defined above.

- **[seuil]**  (*type:* double) Define an upper bound on the norm of the final residue (i.e. the one obtained after applying the multigrid solver). With hybrid precision, as long as we have not obtained a residue whose norm is lower  than the imposed threshold, we keep applying the solver

- **[impr]**  (*type:* flag) Flag to display some info on the resolution on eahc grid

- **[solver_precision]**  (*type:* string into ["mixed", "double"])  Precision with which the variables at stake during  the resolution of the system will be stored. We can have a simple or double precision or both. In the case of a hybrid precision, the multigrid solver is launched in simple precision,  but the residual is calculated in double precision.

- **[iterations_mixed_solver]**  (*type:* int) Define the maximum number of iterations in mixed precision solver


----

.. _multiplefiles:

**multiplefiles**
-----------------

**Inherits from:** :ref:`interprete` 


Change MPI rank limit for multiple files during I/O

Usage:

**multiplefiles** **type**  

Parameters are:

- **type**  (*type:* int) New MPI rank limit


----

.. _nettoiepasnoeuds:

**nettoiepasnoeuds**
--------------------

**Inherits from:** :ref:`interprete` 


Keyword NettoiePasNoeuds does not delete useless nodes (nodes without elements) from 
a domain.

Usage:

**nettoiepasnoeuds** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _op_conv_ef_stab_polymac_face:

**op_conv_ef_stab_polymac_face**
--------------------------------

**Inherits from:** :ref:`interprete` 


Class Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC

Usage:

| **op_conv_ef_stab_polymac_face** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha]**  double
| }

Parameters are:

- **[alpha]**  (*type:* double) parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)


----

.. _op_conv_ef_stab_polymac_p0_face:

**op_conv_ef_stab_polymac_p0_face**
-----------------------------------

**Inherits from:** :ref:`interprete` 


Class Op_Conv_EF_Stab_PolyMAC_P0_Face

Usage:

| **op_conv_ef_stab_polymac_p0_face** *str*
| **Read** *str* {
| }


----

.. _op_conv_ef_stab_polymac_p0p1nc_elem:

**op_conv_ef_stab_polymac_p0p1nc_elem**
---------------------------------------

**Synonyms:** op_conv_ef_stab_polymac_p0_elem

**Inherits from:** :ref:`interprete` 


Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem

Usage:

| **op_conv_ef_stab_polymac_p0p1nc_elem** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha]**  double
| }

Parameters are:

- **[alpha]**  (*type:* double) parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)


----

.. _op_conv_ef_stab_polymac_p0p1nc_face:

**op_conv_ef_stab_polymac_p0p1nc_face**
---------------------------------------

**Inherits from:** :ref:`interprete` 


Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face

Usage:

| **op_conv_ef_stab_polymac_p0p1nc_face** *str*
| **Read** *str* {
| }


----

.. _option_polymac:

**option_polymac**
------------------

**Inherits from:** :ref:`interprete` 


Class of PolyMAC options.

Usage:

| **option_polymac** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_osqp]**  flag
| }

Parameters are:

- **[use_osqp]**  (*type:* flag) Flag to use the old formulation of the M2 matrix provided by the OSQP library


----

.. _option_polymac_p0:

**option_polymac_p0**
---------------------

**Inherits from:** :ref:`interprete` 


Class of PolyMAC_P0 options.

Usage:

| **option_polymac_p0** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[interp_ve1]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_axi]**  flag
| }

Parameters are:

- **[interp_ve1]**  (*type:* flag) Flag to enable a first order velocity face-to-element interpolation (the default  value is 0 which means a second order interpolation)

- **[traitement_axi]**  (*type:* flag) Flag used to relax the time-step stability criterion in case of a thin slice  geometry while modelling an axi-symetrical case


----

.. _option_vdf:

**option_vdf**
--------------

**Inherits from:** :ref:`interprete` 


Class of VDF options.

Usage:

| **option_vdf** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_coins]**  string into ["oui", "non"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_gradients]**  string into ["oui", "non"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p_imposee_aux_faces]**  string into ["oui", "non"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[all_options | toutes_les_options]**  flag
| }

Parameters are:

- **[traitement_coins]**  (*type:* string into ["oui", "non"])  Treatment of corners (yes or no). This option modifies slightly the calculations at the outlet of the plane channel. It supposes that the boundary continues after channel outlet (i.e. velocity vector remains parallel to the boundary).

- **[traitement_gradients]**  (*type:* string into ["oui", "non"])  Treatment of gradient calculations (yes or no). This option modifies slightly the gradient calculation at the corners and activates  also the corner treatment option.

- **[p_imposee_aux_faces]**  (*type:* string into ["oui", "non"])  Pressure imposed at the faces (yes or no).

- **[all_options | toutes_les_options]**  (*type:* flag) Activates all Option_VDF options. If used, must be used alone without specifying the other options, nor combinations.


----

.. _orientefacesbord:

**orientefacesbord**
--------------------

**Inherits from:** :ref:`interprete` 


Keyword to modify the order of the boundary vertices included in a domain, such that 
the surface normals are outer pointing.

Usage:

**orientefacesbord** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _parallel_io_parameters:

**parallel_io_parameters**
--------------------------

**Inherits from:** :ref:`interprete` 


Object to handle parallel files in IJK discretization

Usage:

| **parallel_io_parameters** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[block_size_bytes]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[block_size_megabytes]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[writing_processes]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[bench_ijk_splitting_write]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[bench_ijk_splitting_read]**  string
| }

Parameters are:

- **[block_size_bytes]**  (*type:* int) File writes will be performed by chunks of this size (in bytes). This parameter will not be taken into account if block_size_megabytes has been defined

- **[block_size_megabytes]**  (*type:* int) File writes will be performed by chunks of this size (in megabytes). The size should be a multiple of the GPFS block size or lustre stripping size (typically  several megabytes)

- **[writing_processes]**  (*type:* int) This is the number of processes that will write concurrently to the file  system (this must be set according to the capacity of the filesystem, set to 1 on  small computers, can be up to 64 or 128 on very large systems).

- **[bench_ijk_splitting_write]**  (*type:* string) Name of the splitting object we want to use to run a parallel write bench  (optional parameter)

- **[bench_ijk_splitting_read]**  (*type:* string) Name of the splitting object we want to use to run a parallel read bench  (optional parameter)


----

.. _partition:

**partition**
-------------

**Synonyms:** decouper

**Inherits from:** :ref:`interprete` 


Class for parallel calculation to cut a domain for each processor.
By default, this keyword is commented in the reference test cases.

Usage:

**partition** **domaine** **bloc_decouper**  

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) Name of the domain to be cut.

- **bloc_decouper**  (*type:* :ref:`bloc_decouper`) Description how to cut a domain.


----

.. _partition_multi:

**partition_multi**
-------------------

**Synonyms:** decouper_multi

**Inherits from:** :ref:`interprete` 


allows to partition multiple domains in contact with each other in parallel: necessary 
for resolution monolithique in implicit schemes and for all coupled problems using 
PolyMAC_P0P1NC.
By default, this keyword is commented in the reference test cases.

Usage:

**partition_multi** **aco** **domaine1** **dom** **blocdecoupdom1** **domaine2** **dom2** **blocdecoupdom2** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **domaine1**  (*type:* string into ["domaine"])  not set.

- **dom**  (*type:* :ref:`domaine`) Name of the first domain to be cut.

- **blocdecoupdom1**  (*type:* :ref:`bloc_decouper`) Partition bloc for the first domain.

- **domaine2**  (*type:* string into ["domaine"])  not set.

- **dom2**  (*type:* :ref:`domaine`) Name of the second domain to be cut.

- **blocdecoupdom2**  (*type:* :ref:`bloc_decouper`) Partition bloc for the second domain.

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _pilote_icoco:

**pilote_icoco**
----------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

| **pilote_icoco** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **pb_name**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **main**  string
| }

Parameters are:

- **pb_name**  (*type:* string) not_set

- **main**  (*type:* string) not_set


----

.. _polyedriser:

**polyedriser**
---------------

**Inherits from:** :ref:`interprete` 


cast hexahedra into polyhedra so that the indexing of the mesh vertices is compatible 
with PolyMAC_P0P1NC discretization.
Must be used in PolyMAC_P0P1NC discretization if a hexahedral mesh has been produced 
with TRUST's internal mesh generator.

Usage:

**polyedriser** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _postraiter_domaine:

**postraiter_domaine**
----------------------

**Inherits from:** :ref:`interprete` 


To write one or more domains in a file with a specified format (MED,LML,LATA,SINGLE_LATA).

Usage:

| **postraiter_domaine** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **format**  string into ["lml", "lata", "single_lata", "lata_v2", "med"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier | file]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine | domain]**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  :ref:`sous_zone`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaines]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[joints_non_postraites]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[binaire]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_frontiere]**  int into [0,1]
| }

Parameters are:

- **format**  (*type:* string into ["lml", "lata", "single_lata", "lata_v2", "med"])  File format.

- **[fichier | file]**  (*type:* string) The file name can be changed with the fichier option.

- **[domaine | domain]**  (*type:* :ref:`domaine`) Name of domain

- **[sous_zone]**  (*type:* :ref:`sous_zone`) Name of the sub_zone

- **[domaines]**  (*type:* :ref:`bloc_lecture`) Names of domains : { name1 name2 }

- **[joints_non_postraites]**  (*type:* int into [0,1])  The joints_non_postraites (1 by default) will not write the boundaries  between the partitioned mesh.

- **[binaire]**  (*type:* int into [0,1])  Binary (binaire 1) or ASCII (binaire 0) may be used. By default, it is 0 for LATA and only ASCII is available for LML and only binary  is available for MED.

- **[ecrire_frontiere]**  (*type:* int into [0,1])  This option will write (if set to 1, the default) or not (if  set to 0) the boundaries as fields into the file (it is useful to not add the boundaries  when writing a domain extracted from another domain)


----

.. _precisiongeom:

**precisiongeom**
-----------------

**Inherits from:** :ref:`interprete` 


Class to change the way floating-point number comparison is done.
By default, two numbers are equal if their absolute difference is smaller than 1e-10.
The keyword is useful to modify this value.
Moreover, nodes coordinates will be written in .geom files with this same precision.

Usage:

**precisiongeom** **precision**  

Parameters are:

- **precision**  (*type:* double) New value of precision.


----

.. _raffiner_anisotrope:

**raffiner_anisotrope**
-----------------------

**Inherits from:** :ref:`interprete` 


Only for VEF discretizations, allows to cut triangle elements in 3, or tetrahedra 
in 4 parts, by defining a new summit located at the center of the element: \includepng{{raffineranisotri.pdf}}{{4}} 
\includepng{{raffineranisotetra.jpeg}}{{6}}

Note that such a cut creates flat elements (anisotropic).

Usage:

**raffiner_anisotrope** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _raffiner_isotrope:

**raffiner_isotrope**
---------------------

**Synonyms:** raffiner_simplexes

**Inherits from:** :ref:`interprete` 


For VDF and VEF discretizations, allows to cut triangles/quadrangles or tetrahedral/hexaedras 
elements respectively in 4 or 8 new ones by defining new summits located at the middle 
of edges (and center of faces and elements for quadrangles and hexaedra).
Such a cut preserves the shape of original elements (isotropic).
For 2D elements: \includetabfig{{raffinerisotri.pdf}}{{4}}{{raffinerisorect.pdf}}{{4}} 
For 3D elements: \includetabfig{{raffinerisotetra.jpeg}}{{6}}{{raffinerisohexa.jpeg}}{{5}}.

Usage:

**raffiner_isotrope** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _raffiner_isotrope_parallele:

**raffiner_isotrope_parallele**
-------------------------------

**Inherits from:** :ref:`interprete` 


Refine parallel mesh in parallel

Usage:

| **raffiner_isotrope_parallele** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **name_of_initial_domaines | name_of_initial_zones**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **name_of_new_domaines | name_of_new_zones**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ascii]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[single_hdf]**  flag
| }

Parameters are:

- **name_of_initial_domaines | name_of_initial_zones**  (*type:* string) name of initial Domaines

- **name_of_new_domaines | name_of_new_zones**  (*type:* string) name of new Domaines

- **[ascii]**  (*type:* flag) writing Domaines in ascii format

- **[single_hdf]**  (*type:* flag) writing Domaines in hdf format


----

.. _read:

**read**
--------

**Synonyms:** lire

**Inherits from:** :ref:`interprete` 


Interpretor to read the a_object objet defined between the braces.

Usage:

**read** **a_object** **bloc**  

Parameters are:

- **a_object**  (*type:* string) Object to be read.

- **bloc**  (*type:* string) Definition of the object.


----

.. _read_file:

**read_file**
-------------

**Synonyms:** lire_fichier

**Inherits from:** :ref:`interprete` 


Keyword to read the object name_obj contained in the file filename.

This is notably used when the calculation domain has already been meshed and the 
mesh contains the file filename, simply write read_file dom filename (where dom is 
the name of the meshed domain).

If the filename is ;, is to execute a data set given in the file of name name_obj 
(a space must be entered between the semi-colon and the file name).

Usage:

**read_file** **name_obj** **filename**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be read.

- **filename**  (*type:* string) Name of the file.


----

.. _read_file_bin:

**read_file_bin**
-----------------

**Synonyms:** lire_fichier_bin, read_file_binary

**Inherits from:** :ref:`interprete` 


Keyword to read an object name_obj in the unformatted type file filename.

Usage:

**read_file_bin** **name_obj** **filename**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be read.

- **filename**  (*type:* string) Name of the file.


----

.. _read_med:

**read_med**
------------

**Synonyms:** lire_med

**Inherits from:** :ref:`interprete` 


Keyword to read MED mesh files where 'domain' corresponds to the domain name, 'file' 
corresponds to the file (written in the MED format) containing the mesh named mesh_name.

Note about naming boundaries: When reading 'file', TRUST will detect boundaries between 
domains (Raccord) when the name of the boundary begins by type_raccord_.
For example, a boundary named type_raccord_wall in 'file' will be considered by TRUST 
as a boundary named 'wall' between two domains.

NB: To read several domains from a mesh issued from a MED file, use Read_Med to read 
the mesh then use Create_domain_from_sub_domain keyword.

NB: If the MED file contains one or several subdomaine defined as a group of volumes, 
then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo 
defining the subdomaines for sequential and/or parallel calculations.
These subdomaines will be read in sequential in the datafile by including (after 
Read_Med keyword) something like:

Read_Med ....

Read_file domain_name_ssz.geo ;

During the parallel calculation, you will include something:

Scatter { ...
}

Read_file domain_name_ssz_par.geo ;

Usage:

| **read_med** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convertalltopoly]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domain | domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **file | fichier**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mesh | maillage]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[exclude_groups | exclure_groupes]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[include_additional_face_groups | inclure_groupes_faces_additionnels]**  string list
| }

Parameters are:

- **[convertalltopoly]**  (*type:* flag) Option to convert mesh with mixed cells into polyhedral/polygonal cells

- **domain | domaine**  (*type:* :ref:`domaine`) Corresponds to the domain name.

- **file | fichier**  (*type:* string) File (written in the MED format, with extension '.med') containing the mesh

- **[mesh | maillage]**  (*type:* string) Name of the mesh in med file. If not specified, the first mesh will be read.

- **[exclude_groups | exclure_groupes]**  (*type:* string list) List of face groups to skip in the MED file.

- **[include_additional_face_groups | inclure_groupes_faces_additionnels]**  (*type:* string list) List of face groups to read and register in the MED file.


----

.. _read_tgrid:

**read_tgrid**
--------------

**Synonyms:** lire_tgrid

**Inherits from:** :ref:`interprete` 


Keyword to reaf Tgrid/Gambit mesh files.
2D (triangles or quadrangles) and 3D (tetra or hexa elements) meshes, may be read 
by TRUST.

Usage:

**read_tgrid** **dom** **filename**  

Parameters are:

- **dom**  (*type:* :ref:`domaine`) Name of domaine.

- **filename**  (*type:* string) Name of file containing the mesh.


----

.. _read_unsupported_ascii_file_from_icem:

**read_unsupported_ascii_file_from_icem**
-----------------------------------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**read_unsupported_ascii_file_from_icem** **name_obj** **filename**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be read.

- **filename**  (*type:* string) Name of the file.


----

.. _rectify_mesh:

**rectify_mesh**
----------------

**Synonyms:** orienter_simplexes

**Inherits from:** :ref:`interprete` 


Keyword to raffine a mesh

Usage:

**rectify_mesh** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _redresser_hexaedres_vdf:

**redresser_hexaedres_vdf**
---------------------------

**Inherits from:** :ref:`interprete` 


Keyword to convert a domain (named domain_name) with quadrilaterals/VEF hexaedras 
which looks like rectangles/VDF hexaedras into a domain with real rectangles/VDF hexaedras.

Usage:

**redresser_hexaedres_vdf** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain to resequence.


----

.. _refine_mesh:

**refine_mesh**
---------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**refine_mesh** **domaine**  

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) not_set


----

.. _regroupebord:

**regroupebord**
----------------

**Inherits from:** :ref:`interprete` 


Keyword to build one boundary new_bord with several boundaries of the domain named 
domaine.

Usage:

**regroupebord** **domaine | domain** **new_bord** **bords**  

Parameters are:

- **domaine | domain**  (*type:* :ref:`domaine`) Name of domain

- **new_bord**  (*type:* string) Name of the new boundary

- **bords**  (*type:* :ref:`bloc_lecture`) { Bound1 Bound2 }


----

.. _remove_elem:

**remove_elem**
---------------

**Inherits from:** :ref:`interprete` 


Keyword to remove element from a VDF mesh (named domaine_name), either from an explicit 
list of elements or from a geometric condition defined by a condition f(x,y)>0 in 
2D and f(x,y,z)>0 in 3D.
All the new borders generated are gathered in one boundary called : newBord (to rename 
it, use RegroupeBord keyword.
To split it to different boundaries, use DecoupeBord_Pour_Rayonnement keyword).
Example of a removed zone of radius 0.2 centered at (x,y)=(0.5,0.5):

Remove_elem dom { fonction $0.2*0.2-(x-0.5)^2-(y-0.5)^2>0$ }

Warning : the thickness of removed zone has to be large enough to avoid singular 
nodes as decribed below : \includepng{{removeelem.png}}{{11.234}}

Usage:

**remove_elem** **domaine | domain** **bloc**  

Parameters are:

- **domaine | domain**  (*type:* :ref:`domaine`) Name of domain

- **bloc**  (*type:* :ref:`remove_elem_bloc`) not_set


----

.. _remove_invalid_internal_boundaries:

**remove_invalid_internal_boundaries**
--------------------------------------

**Inherits from:** :ref:`interprete` 


Keyword to suppress an internal boundary of the domain_name domain.
Indeed, some mesh tools may define internal boundaries (eg: for post processing task 
after the calculation) but TRUST does not support it yet.

Usage:

**remove_invalid_internal_boundaries** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _reorienter_tetraedres:

**reorienter_tetraedres**
-------------------------

**Inherits from:** :ref:`interprete` 


This keyword is mandatory for front-tracking computations with the VEF discretization.
For each tetrahedral element of the domain, it checks if it has a positive volume.
If the volume (determinant of the three vectors) is negative, it swaps two nodes 
to reverse the orientation of this tetrahedron.

Usage:

**reorienter_tetraedres** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _reorienter_triangles:

**reorienter_triangles**
------------------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**reorienter_triangles** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _resequencing:

**resequencing**
----------------

**Synonyms:** reordonner

**Inherits from:** :ref:`interprete` 


The Reordonner interpretor is required sometimes for a VDF mesh which is not produced 
by the internal mesher.
Example where this is used:

Read_file dom fichier.geom

Reordonner dom

Observations: This keyword is redundant when the mesh that is read is correctly sequenced 
in the TRUST sense.
This significant mesh operation may take some time...
The message returned by TRUST is not explicit when the Reordonner (Resequencing) 
keyword is required but not included in the data set...

Usage:

**resequencing** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain to resequence.


----

.. _residuals:

**residuals**
-------------

**Inherits from:** :ref:`interprete` 


To specify how the residuals will be computed.

Usage:

| **residuals** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[norm]**  string into ["l2", "max"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[relative]**  string into ["0", "1", "2"]
| }

Parameters are:

- **[norm]**  (*type:* string into ["l2", "max"])  allows to choose the norm we want to use (max norm by  default). Possible to specify L2-norm.

- **[relative]**  (*type:* string into ["0", "1", "2"])  This is the old keyword seuil_statio_relatif_deconseille. If it is set to 1, it will normalize the residuals with the residuals of the first  5 timesteps (default is 0). if set to 2, residual will be computed as R/(max-min).


----

.. _rotation:

**rotation**
------------

**Inherits from:** :ref:`interprete` 


Keyword to rotate the geometry of an arbitrary angle around an axis aligned with Ox, 
Oy or Oz axis.

Usage:

**rotation** **domain_name** **dir** **coord1** **coord2** **angle**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain to wich the transformation is applied.

- **dir**  (*type:* string into ["x", "y", "z"])  X, Y or Z to indicate the direction of the rotation axis

- **coord1**  (*type:* double) coordinates of the center of rotation in the plane orthogonal to the rotation  axis. These coordinates must be specified in the direct triad sense.

- **coord2**  (*type:* double) not_set

- **angle**  (*type:* double) angle of rotation (in degrees)


----

.. _scatter:

**scatter**
-----------

**Inherits from:** :ref:`interprete` 


Class to read a partionned mesh in the files during a parallel calculation.
The files are in binary format.

Usage:

**scatter** **file** **domaine**  

Parameters are:

- **file**  (*type:* string) Name of file.

- **domaine**  (*type:* :ref:`domaine`) Name of domain.


----

.. _scattermed:

**scattermed**
--------------

**Inherits from:** :ref:`interprete` 


This keyword will read the partition of the domain_name domain into a the MED format 
files file.med created by Medsplitter.

Usage:

**scattermed** **file** **domaine**  

Parameters are:

- **file**  (*type:* string) Name of file.

- **domaine**  (*type:* :ref:`domaine`) Name of domain.


----

.. _solve:

**solve**
---------

**Synonyms:** resoudre

**Inherits from:** :ref:`interprete` 


Interpretor to start calculation with TRUST.

Usage:

**solve** **pb**  

Parameters are:

- **pb**  (*type:* :ref:`pb_gen_base`) Name of problem to be solved.


----

.. _supprime_bord:

**supprime_bord**
-----------------

**Inherits from:** :ref:`interprete` 


Keyword to remove boundaries (named Boundary_name1 Boundary_name2 ) of the domain 
named domain_name.

Usage:

**supprime_bord** **domaine | domain** **bords**  

Parameters are:

- **domaine | domain**  (*type:* :ref:`domaine`) Name of domain

- **bords**  (*type:* :ref:`list_nom`) { Boundary_name1 Boundaray_name2 }


----

.. _system:

**system**
----------

**Inherits from:** :ref:`interprete` 


To run Unix commands from the data file.
Example: System 'echo The End | mail trust@cea.fr'

Usage:

**system** **cmd**  

Parameters are:

- **cmd**  (*type:* string) command to execute.


----

.. _test_solveur:

**test_solveur**
----------------

**Inherits from:** :ref:`interprete` 


To test several solvers

Usage:

| **test_solveur** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_secmem]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_matrice]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_solution]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_test]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier_solveur]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[genere_fichier_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pas_de_solution_initiale]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ascii]**  flag
| }

Parameters are:

- **[fichier_secmem]**  (*type:* string) Filename containing the second member B

- **[fichier_matrice]**  (*type:* string) Filename containing the matrix A

- **[fichier_solution]**  (*type:* string) Filename containing the solution x

- **[nb_test]**  (*type:* int) Number of tests to measure the time resolution (one preconditionnement)

- **[impr]**  (*type:* flag) To print the convergence solver

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) To specify a solver

- **[fichier_solveur]**  (*type:* string) To specify a file containing a list of solvers

- **[genere_fichier_solveur]**  (*type:* double) To create a file of the solver with a threshold convergence

- **[seuil_verification]**  (*type:* double) Check if the solution satisfy ||Ax-B||<precision

- **[pas_de_solution_initiale]**  (*type:* flag) Resolution isn't initialized with the solution x

- **[ascii]**  (*type:* flag) Ascii files


----

.. _test_sse_kernels:

**test_sse_kernels**
--------------------

**Inherits from:** :ref:`interprete` 


Object to test the different kernel methods used in the multigrid solver in IJK discretization

Usage:

| **test_sse_kernels** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nmax]**  int
| }

Parameters are:

- **[nmax]**  (*type:* int) Number of tests we want to perform


----

.. _testeur:

**testeur**
-----------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**testeur** **data**  

Parameters are:

- **data**  (*type:* :ref:`bloc_lecture`) not_set


----

.. _testeur_medcoupling:

**testeur_medcoupling**
-----------------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**testeur_medcoupling** **pb_name** **field_name | filed_name**  

Parameters are:

- **pb_name**  (*type:* string) Name of domain.

- **field_name | filed_name**  (*type:* string) Name of domain.


----

.. _tetraedriser:

**tetraedriser**
----------------

**Inherits from:** :ref:`interprete` 


To achieve a tetrahedral mesh based on a mesh comprising blocks, the Tetraedriser 
(Tetrahedralise) interpretor is used in VEF discretization.
Initial block is divided in 6 tetrahedra: \includepng{{tetraedriser.jpeg}}{{5}}

Usage:

**tetraedriser** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _tetraedriser_homogene:

**tetraedriser_homogene**
-------------------------

**Inherits from:** :ref:`interprete` 


Use the Tetraedriser_homogene (Homogeneous_Tetrahedralisation) interpretor in VEF 
discretization to mesh a block in tetrahedrals.
Each block hexahedral is no longer divided into 6 tetrahedrals (keyword Tetraedriser 
(Tetrahedralise)), it is now broken down into 40 tetrahedrals.
Thus a block defined with 11 nodes in each X, Y, Z direction will contain 10*10*10*40=40,000 
tetrahedrals.
This also allows problems in the mesh corners with the P1NC/P1iso/P1bulle or P1/P1 
discretization items to be avoided.
Initial block is divided in 40 tetrahedra: \includepng{{tetraedriserhomogene.jpeg}}{{5}}

Usage:

**tetraedriser_homogene** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _tetraedriser_homogene_compact:

**tetraedriser_homogene_compact**
---------------------------------

**Inherits from:** :ref:`interprete` 


This new discretization generates tetrahedral elements from cartesian or non-cartesian 
hexahedral elements.
The process cut each hexahedral in 6 pyramids, each of them being cut then in 4 tetrahedral.
So, in comparison with tetra_homogene, less elements (*24 instead of*40) with more 
homogeneous volumes are generated.
Moreover, this process is done in a faster way.
Initial block is divided in 24 tetrahedra: \includepng{{tetraedriserhomogenecompact.jpeg}}{{5}}

Usage:

**tetraedriser_homogene_compact** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _tetraedriser_homogene_fin:

**tetraedriser_homogene_fin**
-----------------------------

**Inherits from:** :ref:`interprete` 


Tetraedriser_homogene_fin is the recommended option to tetrahedralise blocks.
As an extension (subdivision) of Tetraedriser_homogene_compact, this last one cut 
each initial block in 48 tetrahedra (against 24, previously).
This cutting ensures :

- a correct cutting in the corners (in respect to pressure discretization PreP1B),

- a better isotropy of elements than with Tetraedriser_homogene_compact,

- a better alignment of summits (this could have a benefit effect on calculation 
near walls since first elements in contact with it are all contained in the same constant 
thickness and ii/ by the way, a 3D cartesian grid based on summits can be engendered 
and used to realise spectral analysis in HIT for instance).
Initial block is divided in 48 tetrahedra: \includepng{{tetraedriserhomogenefin.jpeg}}{{5}}

Usage:

**tetraedriser_homogene_fin** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _tetraedriser_par_prisme:

**tetraedriser_par_prisme**
---------------------------

**Inherits from:** :ref:`interprete` 


Tetraedriser_par_prisme generates 6 iso-volume tetrahedral element from primary hexahedral 
one (contrarily to the 5 elements ordinarily generated by tetraedriser).
This element is suitable for calculation of gradients at the summit (coincident with 
the gravity centre of the jointed elements related with) and spectra (due to a better 
alignment of the points).
\includetabfig{{tetraedriserparprisme.jpeg}}{{5}}{{tetraedriserparprisme2.jpeg}}{{5}} 
Initial block is divided in 6 prismes.

Usage:

**tetraedriser_par_prisme** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _transformer:

**transformer**
---------------

**Inherits from:** :ref:`interprete` 


Keyword to transform the coordinates of the geometry.

Exemple to rotate your mesh by a 90o rotation and to scale the z coordinates by a 
factor 2: Transformer domain_name -y -x 2*z

Usage:

**transformer** **domain_name** **formule**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.

- **formule**  (*type:* string list) Function_for_x Function_for_y \[ Function_for z \]


----

.. _triangulate:

**triangulate**
---------------

**Synonyms:** trianguler

**Inherits from:** :ref:`interprete` 


To achieve a triangular mesh from a mesh comprising rectangles (2 triangles per rectangle).
Should be used in VEF discretization.
Principle:

\includepng{{trianguler.pdf}}{{10}}

Usage:

**triangulate** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _trianguler_fin:

**trianguler_fin**
------------------

**Inherits from:** :ref:`interprete` 


Trianguler_fin is the recommended option to triangulate rectangles.

As an extension (subdivision) of Triangulate_h option, this one cut each initial 
rectangle in 8 triangles (against 4, previously).
This cutting ensures :

- a correct cutting in the corners (in respect to pressure discretization PreP1B).

- a better isotropy of elements than with Trianguler_h option.

- a better alignment of summits (this could have a benefit effect on calculation 
near walls since first elements in contact with it are all contained in the same constant 
thickness, and, by this way, a 2D cartesian grid based on summits can be engendered 
and used to realize statistical analysis in plane channel configuration for instance).
Principle:

\includepng{{triangulerfin.pdf}}{{10}}

Usage:

**trianguler_fin** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _trianguler_h:

**trianguler_h**
----------------

**Inherits from:** :ref:`interprete` 


To achieve a triangular mesh from a mesh comprising rectangles (4 triangles per rectangle).
Should be used in VEF discretization.
Principle:

\includepng{{triangulerh.pdf}}{{10}}

Usage:

**trianguler_h** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _verifier_qualite_raffinements:

**verifier_qualite_raffinements**
---------------------------------

**Inherits from:** :ref:`interprete` 


not_set

Usage:

**verifier_qualite_raffinements** **domain_names**  

Parameters are:

- **domain_names**  (*type:* :ref:`vect_nom`) not_set


----

.. _verifier_simplexes:

**verifier_simplexes**
----------------------

**Inherits from:** :ref:`interprete` 


Keyword to raffine a simplexes

Usage:

**verifier_simplexes** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _verifiercoin:

**verifiercoin**
----------------

**Inherits from:** :ref:`interprete` 


This keyword subdivides inconsistent 2D/3D cells used with VEFPreP1B discretization.
Must be used before the mesh is discretized.
NL1 The Read_file option can be used only if the file.decoupage_som was previously 
created by TRUST.
This option, only in 2D, reverses the common face at two cells (at least one is inconsistent), 
through the nodes opposed.
In 3D, the option has no effect.

The expert_only option deactivates, into the VEFPreP1B divergence operator, the test 
of inconsistent cells.

Usage:

**verifiercoin** **domain_name | dom** **bloc**  

Parameters are:

- **domain_name | dom**  (*type:* :ref:`domaine`) Name of the domaine

- **bloc**  (*type:* :ref:`verifiercoin_bloc`) not_set


----

.. _write:

**write**
---------

**Synonyms:** ecrire

**Inherits from:** :ref:`interprete` 


Keyword to write the object of name name_obj to a standard outlet.

Usage:

**write** **name_obj**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be written.


----

.. _write_file:

**write_file**
--------------

**Synonyms:** ecrire_fichier, ecrire_fichier_bin

**Inherits from:** :ref:`interprete` 


Keyword to write the object of name name_obj to a file filename.
Since the v1.6.3, the default format is now binary format file.

Usage:

**write_file** **name_obj** **filename**  

Parameters are:

- **name_obj**  (*type:* string) Name of the object to be written.

- **filename**  (*type:* string) Name of the file.


----

**Keywords derived from list_bloc_mailler**
===========================================

.. _list_bloc_mailler:

**list_bloc_mailler**
---------------------


List of block mesh.


----

**Keywords derived from list_bord**
===================================

.. _list_bord:

**list_bord**
-------------


The block sides.


----

**Keywords derived from list_info_med**
=======================================

.. _list_info_med:

**list_info_med**
-----------------


not_set


----

**Keywords derived from list_list_nom**
=======================================

.. _list_list_nom:

**list_list_nom**
-----------------


pour les groupes


----

**Keywords derived from list_nom**
==================================

.. _list_nom:

**list_nom**
------------


List of name.


----

**Keywords derived from list_nom_virgule**
==========================================

.. _list_nom_virgule:

**list_nom_virgule**
--------------------


List of name.


----

**Keywords derived from list_stat_post**
========================================

.. _list_stat_post:

**list_stat_post**
------------------


Post-processing for statistics


----

**Keywords derived from list_un_pb**
====================================

.. _list_un_pb:

**list_un_pb**
--------------


pour les groupes


----

**Keywords derived from listchamp_generique**
=============================================

.. _listchamp_generique:

**listchamp_generique**
-----------------------


XXX


----

**Keywords derived from listdeuxmots_sacc**
===========================================

.. _listdeuxmots_sacc:

**listdeuxmots_sacc**
---------------------


List of groups of two words (without curly brackets).


----

**Keywords derived from liste_mil**
===================================

.. _liste_mil:

**liste_mil**
-------------


MUSIG medium made of several sub mediums.


----

**Keywords derived from liste_post**
====================================

.. _liste_post:

**liste_post**
--------------


Keyword to use several results files.
List of objects of post-processing (with name)


----

**Keywords derived from liste_post_ok**
=======================================

.. _liste_post_ok:

**liste_post_ok**
-----------------


Keyword to use several results files.
List of objects of post-processing (with name)


----

**Keywords derived from liste_sonde_tble**
==========================================

.. _liste_sonde_tble:

**liste_sonde_tble**
--------------------


not_set


----

**Keywords derived from listeqn**
=================================

.. _listeqn:

**listeqn**
-----------


List of equations.


----

**Keywords derived from listobj**
=================================

.. _listobj:

**listobj**
-----------


List of objects.

Usage:

| **listobj** *str*
| **Read** *str* {
| }


----

**Keywords derived from listpoints**
====================================

.. _listpoints:

**listpoints**
--------------


Points.


----

**Keywords derived from listsous_zone_valeur**
==============================================

.. _listsous_zone_valeur:

**listsous_zone_valeur**
------------------------


List of groups of two words.


----

**Keywords derived from loi_etat_base**
=======================================

.. _binaire_gaz_parfait_qc:

**binaire_gaz_parfait_qc**
--------------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas binary mixtures state law used with a quasi-compressible fluid 
under the iso-thermal and iso-bar assumptions.

Usage:

| **binaire_gaz_parfait_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **molar_mass1**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **molar_mass2**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu1**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu2**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **temperature**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diffusion_coeff**  double
| }

Parameters are:

- **molar_mass1**  (*type:* double) Molar mass of species 1 (in kg/mol).

- **molar_mass2**  (*type:* double) Molar mass of species 2 (in kg/mol).

- **mu1**  (*type:* double) Dynamic viscosity of species 1 (in kg/m.s).

- **mu2**  (*type:* double) Dynamic viscosity of species 2 (in kg/m.s).

- **temperature**  (*type:* double) Temperature (in Kelvin) which will be constant during the simulation since  this state law only works for iso-thermal conditions.

- **diffusion_coeff**  (*type:* double) Diffusion coefficient assumed the same for both species (in m2/s).


----

.. _binaire_gaz_parfait_wc:

**binaire_gaz_parfait_wc**
--------------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas binary mixtures state law used with a weakly-compressible fluid 
under the iso-thermal and iso-bar assumptions.

Usage:

| **binaire_gaz_parfait_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **molar_mass1**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **molar_mass2**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu1**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu2**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **temperature**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diffusion_coeff**  double
| }

Parameters are:

- **molar_mass1**  (*type:* double) Molar mass of species 1 (in kg/mol).

- **molar_mass2**  (*type:* double) Molar mass of species 2 (in kg/mol).

- **mu1**  (*type:* double) Dynamic viscosity of species 1 (in kg/m.s).

- **mu2**  (*type:* double) Dynamic viscosity of species 2 (in kg/m.s).

- **temperature**  (*type:* double) Temperature (in Kelvin) which will be constant during the simulation since  this state law only works for iso-thermal conditions.

- **diffusion_coeff**  (*type:* double) Diffusion coefficient assumed the same for both species (in m2/s).


----

.. _loi_etat_base:

**loi_etat_base**
-----------------


Basic class for state laws used with a dilatable fluid.

Usage:

| **loi_etat_base** *str*
| **Read** *str* {
| }


----

.. _loi_etat_gaz_parfait_base:

**loi_etat_gaz_parfait_base**
-----------------------------

**Inherits from:** :ref:`loi_etat_base` 


Basic class for perfect gases state laws used with a dilatable fluid.

Usage:

| **loi_etat_gaz_parfait_base** *str*
| **Read** *str* {
| }


----

.. _loi_etat_gaz_reel_base:

**loi_etat_gaz_reel_base**
--------------------------

**Inherits from:** :ref:`loi_etat_base` 


Basic class for real gases state laws used with a dilatable fluid.

Usage:

| **loi_etat_gaz_reel_base** *str*
| **Read** *str* {
| }


----

.. _multi_gaz_parfait_qc:

**multi_gaz_parfait_qc**
------------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas multi-species mixtures state law used with a quasi-compressible 
fluid.

Usage:

| **multi_gaz_parfait_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **sc**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **prandtl**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cp]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dtol_fraction]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_fraction]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ignore_check_fraction]**  flag
| }

Parameters are:

- **sc**  (*type:* double) Schmidt number of the gas Sc=nu/D (D: diffusion coefficient of the mixing).

- **prandtl**  (*type:* double) Prandtl number of the gas Pr=mu*Cp/lambda

- **[cp]**  (*type:* double) Specific heat at constant pressure of the gas Cp.

- **[dtol_fraction]**  (*type:* double) Delta tolerance on mass fractions for check testing (default value 1.e-6).

- **[correction_fraction]**  (*type:* flag) To force mass fractions between 0. and 1.

- **[ignore_check_fraction]**  (*type:* flag) Not to check if mass fractions between 0. and 1.


----

.. _multi_gaz_parfait_wc:

**multi_gaz_parfait_wc**
------------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas multi-species mixtures state law used with a weakly-compressible 
fluid.

Usage:

| **multi_gaz_parfait_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **species_number**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diffusion_coeff**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **molar_mass**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **mu**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **prandtl**  double
| }

Parameters are:

- **species_number**  (*type:* int) Number of species you are considering in your problem.

- **diffusion_coeff**  (*type:* :ref:`field_base`) Diffusion coefficient of each species, defined with a Champ_uniforme  of dimension equals to the species_number.

- **molar_mass**  (*type:* :ref:`field_base`) Molar mass of each species, defined with a Champ_uniforme of dimension  equals to the species_number.

- **mu**  (*type:* :ref:`field_base`) Dynamic viscosity of each species, defined with a Champ_uniforme of dimension  equals to the species_number.

- **cp**  (*type:* :ref:`field_base`) Specific heat at constant pressure of the gas Cp, defined with a Champ_uniforme  of dimension equals to the species_number..

- **prandtl**  (*type:* double) Prandtl number of the gas Pr=mu*Cp/lambda.


----

.. _perfect_gaz_qc:

**perfect_gaz_qc**
------------------

**Synonyms:** gaz_parfait_qc

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas state law used with a quasi-compressible fluid.

Usage:

| **perfect_gaz_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gamma]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **prandtl**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho_constant_pour_debug]**  :ref:`field_base`
| }

Parameters are:

- **cp**  (*type:* double) Specific heat at constant pressure (J/kg/K).

- **[cv]**  (*type:* double) Specific heat at constant volume (J/kg/K).

- **[gamma]**  (*type:* double) Cp/Cv

- **prandtl**  (*type:* double) Prandtl number of the gas Pr=mu*Cp/lambda

- **[rho_constant_pour_debug]**  (*type:* :ref:`field_base`) For developers to debug the code with a constant rho.


----

.. _perfect_gaz_wc:

**perfect_gaz_wc**
------------------

**Synonyms:** gaz_parfait_wc

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas state law used with a weakly-compressible fluid.

Usage:

| **perfect_gaz_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gamma]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **prandtl**  double
| }

Parameters are:

- **cp**  (*type:* double) Specific heat at constant pressure (J/kg/K).

- **[cv]**  (*type:* double) Specific heat at constant volume (J/kg/K).

- **[gamma]**  (*type:* double) Cp/Cv

- **prandtl**  (*type:* double) Prandtl number of the gas Pr=mu*Cp/lambda


----

.. _rhot_gaz_parfait_qc:

**rhot_gaz_parfait_qc**
-----------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for perfect gas used with a quasi-compressible fluid where the state equation 
is defined as rho = f(T).

Usage:

| **rhot_gaz_parfait_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **cp**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[prandtl]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho_xyz]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho_t]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[t_min]**  double
| }

Parameters are:

- **cp**  (*type:* double) Specific heat at constant pressure of the gas Cp.

- **[prandtl]**  (*type:* double) Prandtl number of the gas Pr=mu*Cp/lambda

- **[rho_xyz]**  (*type:* :ref:`field_base`) Defined with a Champ_Fonc_xyz to define a constant rho with time (space  dependent)

- **[rho_t]**  (*type:* string) Expression of T used to calculate rho. This can lead to a variable rho, both in space and in time.

- **[t_min]**  (*type:* double) Temperature may, in some cases, locally and temporarily be very small  (and negative) even though computation converges. T_min keyword allows to set a lower limit of temperature (in Kelvin, -1000 by default). WARNING: DO NOT USE THIS KEYWORD WITHOUT CHECKING CAREFULY YOUR RESULTS!


----

.. _rhot_gaz_reel_qc:

**rhot_gaz_reel_qc**
--------------------

**Inherits from:** :ref:`loi_etat_base` 


Class for real gas state law used with a quasi-compressible fluid.

Usage:

**rhot_gaz_reel_qc** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) Description.


----

**Keywords derived from loi_fermeture_base**
============================================

.. _loi_fermeture_base:

**loi_fermeture_base**
----------------------


Class for appends fermeture to problem

Usage:

| **loi_fermeture_base** *str*
| **Read** *str* {
| }


----

.. _loi_fermeture_test:

**loi_fermeture_test**
----------------------

**Inherits from:** :ref:`loi_fermeture_base` 


Loi for test only

Usage:

| **loi_fermeture_test** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coef]**  double
| }

Parameters are:

- **[coef]**  (*type:* double) coefficient


----

**Keywords derived from loi_horaire**
=====================================

.. _loi_horaire:

**loi_horaire**
---------------


to define the movement with a time-dependant law for the solid interface.

Usage:

| **loi_horaire** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **position**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **vitesse**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rotation]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[derivee_rotation]**  string list
| }

Parameters are:

- **position**  (*type:* string list) not_set

- **vitesse**  (*type:* string list) not_set

- **[rotation]**  (*type:* string list) not_set

- **[derivee_rotation]**  (*type:* string list) not_set


----

**Keywords derived from milieu_base**
=====================================

.. _constituant:

**constituant**
---------------

**Inherits from:** :ref:`milieu_base` 


Constituent.

Usage:

| **constituant** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cp]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coefficient_diffusion]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[rho]**  (*type:* :ref:`field_base`) Density (kg.m-3).

- **[cp]**  (*type:* :ref:`field_base`) Specific heat (J.kg-1.K-1).

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[coefficient_diffusion]**  (*type:* :ref:`field_base`) Constituent diffusion coefficient value (m2.s-1). If a multi-constituent problem is being processed, the diffusivite will be a vectorial  and each components will be the diffusion of the constituent.

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_base:

**fluide_base**
---------------

**Inherits from:** :ref:`milieu_base` 


Basic class for fluids.

Usage:

| **fluide_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_dilatable_base:

**fluide_dilatable_base**
-------------------------

**Inherits from:** :ref:`milieu_base` 


Basic class for dilatable fluids.

Usage:

| **fluide_dilatable_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_incompressible:

**fluide_incompressible**
-------------------------

**Inherits from:** :ref:`milieu_base` 


Class for non-compressible fluids.

Usage:

| **fluide_incompressible** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta_th]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mu]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta_co]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cp]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| }

Parameters are:

- **[beta_th]**  (*type:* :ref:`field_base`) Thermal expansion (K-1).

- **[mu]**  (*type:* :ref:`field_base`) Dynamic viscosity (kg.m-1.s-1).

- **[beta_co]**  (*type:* :ref:`field_base`) Volume expansion coefficient values in concentration.

- **[rho]**  (*type:* :ref:`field_base`) Density (kg.m-3).

- **[cp]**  (*type:* :ref:`field_base`) Specific heat (J.kg-1.K-1).

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[porosites]**  (*type:* :ref:`bloc_lecture`) Porosity (optional)

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).


----

.. _fluide_ostwald:

**fluide_ostwald**
------------------

**Inherits from:** :ref:`milieu_base` 


Non-Newtonian fluids governed by Ostwald's law.
The law applicable to stress tensor is:

tau=K(T)*(D:D/2)**((n-1)/2)*D Where:

D refers to the deformation tensor

K refers to fluid consistency (may be a function of the temperature T)

n refers to the fluid structure index n=1 for a Newtonian fluid, n<1 for a rheofluidifier 
fluid, n>1 for a rheothickening fluid.

Usage:

| **fluide_ostwald** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[k]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[n]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta_th]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mu]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta_co]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cp]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| }

Parameters are:

- **[k]**  (*type:* :ref:`field_base`) Fluid consistency.

- **[n]**  (*type:* :ref:`field_base`) Fluid structure index.

- **[beta_th]**  (*type:* :ref:`field_base`) Thermal expansion (K-1).

- **[mu]**  (*type:* :ref:`field_base`) Dynamic viscosity (kg.m-1.s-1).

- **[beta_co]**  (*type:* :ref:`field_base`) Volume expansion coefficient values in concentration.

- **[rho]**  (*type:* :ref:`field_base`) Density (kg.m-3).

- **[cp]**  (*type:* :ref:`field_base`) Specific heat (J.kg-1.K-1).

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[porosites]**  (*type:* :ref:`bloc_lecture`) Porosity (optional)

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).


----

.. _fluide_quasi_compressible:

**fluide_quasi_compressible**
-----------------------------

**Inherits from:** :ref:`milieu_base` 


Quasi-compressible flow with a low mach number assumption; this means that the thermo-dynamic 
pressure (used in state law) is uniform in space.

Usage:

| **fluide_quasi_compressible** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sutherland]**  :ref:`bloc_sutherland`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pression]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[loi_etat]**  :ref:`loi_etat_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_pth]**  string into ["edo", "constant", "conservation_masse"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_rho_gravite]**  string into ["standard", "moins_rho_moyen"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[temps_debut_prise_en_compte_drho_dt]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[omega_relaxation_drho_dt]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mu]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[sutherland]**  (*type:* :ref:`bloc_sutherland`) Sutherland law for viscosity and for conductivity.

- **[pression]**  (*type:* double) Initial thermo-dynamic pressure used in the assosciated state law.

- **[loi_etat]**  (*type:* :ref:`loi_etat_base`) The state law that will be associated to the Quasi-compressible fluid.

- **[traitement_pth]**  (*type:* string into ["edo", "constant", "conservation_masse"])  Particular treatment for the  thermodynamic pressure Pth ; there are three possibilities:  1) with the keyword 'edo' the code computes Pth solving an O.D.E. ; in this case, the mass is not strictly conserved (it is the default case for quasi  compressible computation):  2) the keyword 'conservation_masse' forces the conservation of the mass (closed geometry  or with periodic boundaries condition)  3) the keyword 'constant' makes it possible to have a constant Pth ; it's the good  choice when the flow is open (e.g. with pressure boundary conditions).  It is possible to monitor the volume averaged value for temperature and density,  plus Pth evolution in the .evol_glob file.

- **[traitement_rho_gravite]**  (*type:* string into ["standard", "moins_rho_moyen"])  It may be :1) \`standard\` : the gravity  term is evaluted with rho*g (It is the default). 2) \`moins_rho_moyen\` : the gravity term is evaluated with (rho-rhomoy) *g. Unknown pressure is then P*=P+rhomoy*g*z. It is useful when you apply uniforme pressure boundary condition like P*=0.

- **[temps_debut_prise_en_compte_drho_dt]**  (*type:* double) While time<value, dRho/dt is set to zero (Rho, volumic mass). Useful for some calculation during the first time steps with big variation of temperature  and volumic mass.

- **[omega_relaxation_drho_dt]**  (*type:* double) Optional option to have a relaxed algorithm to solve the mass equation. value is used (1 per default) to specify omega.

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[mu]**  (*type:* :ref:`field_base`) Dynamic viscosity (kg.m-1.s-1).

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_reel_base:

**fluide_reel_base**
--------------------

**Inherits from:** :ref:`milieu_base` 


Class for real fluids.

Usage:

| **fluide_reel_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_sodium_gaz:

**fluide_sodium_gaz**
---------------------

**Inherits from:** :ref:`milieu_base` 


Class for Fluide_sodium_liquide

Usage:

| **fluide_sodium_gaz** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p_ref]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[t_ref]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[p_ref]**  (*type:* double) Use to set the pressure value in the closure law. If not specified, the value of the pressure unknown will be used

- **[t_ref]**  (*type:* double) Use to set the temperature value in the closure law. If not specified, the value of the temperature unknown will be used

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_sodium_liquide:

**fluide_sodium_liquide**
-------------------------

**Inherits from:** :ref:`milieu_base` 


Class for Fluide_sodium_liquide

Usage:

| **fluide_sodium_liquide** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p_ref]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[t_ref]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[p_ref]**  (*type:* double) Use to set the pressure value in the closure law. If not specified, the value of the pressure unknown will be used

- **[t_ref]**  (*type:* double) Use to set the temperature value in the closure law. If not specified, the value of the temperature unknown will be used

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_stiffened_gas:

**fluide_stiffened_gas**
------------------------

**Inherits from:** :ref:`milieu_base` 


Class for Stiffened Gas

Usage:

| **fluide_stiffened_gas** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gamma]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pinf]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mu]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[q]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[q_prim]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[gamma]**  (*type:* double) Heat capacity ratio (Cp/Cv)

- **[pinf]**  (*type:* double) Stiffened gas pressure constant (if set to zero, the state law becomes  identical to that of perfect gases)

- **[mu]**  (*type:* double) Dynamic viscosity

- **[Lambda]**  (*type:* double) Thermal conductivity

- **[cv]**  (*type:* double) Thermal capacity at constant volume

- **[q]**  (*type:* double) Reference energy

- **[q_prim]**  (*type:* double) Model constant

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _fluide_weakly_compressible:

**fluide_weakly_compressible**
------------------------------

**Inherits from:** :ref:`milieu_base` 


Weakly-compressible flow with a low mach number assumption; this means that the thermo-dynamic 
pressure (used in state law) can vary in space.

Usage:

| **fluide_weakly_compressible** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[loi_etat]**  :ref:`loi_etat_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sutherland]**  :ref:`bloc_sutherland`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_pth]**  string into ["constant"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mu]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pression_thermo]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pression_xyz]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_total_pressure]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_hydrostatic_pressure]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_grad_pression_eos]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[time_activate_ptot]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[indice]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kappa]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[loi_etat]**  (*type:* :ref:`loi_etat_base`) The state law that will be associated to the Weakly-compressible fluid.

- **[sutherland]**  (*type:* :ref:`bloc_sutherland`) Sutherland law for viscosity and for conductivity.

- **[traitement_pth]**  (*type:* string into ["constant"])  Particular treatment for the thermodynamic pressure Pth  ; there is currently one possibility:  1) the keyword 'constant' makes it possible to have a constant Pth but not uniform  in space ; it's the good choice when the flow is open (e.g. with pressure boundary conditions).

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[mu]**  (*type:* :ref:`field_base`) Dynamic viscosity (kg.m-1.s-1).

- **[pression_thermo]**  (*type:* double) Initial thermo-dynamic pressure used in the assosciated state law.

- **[pression_xyz]**  (*type:* :ref:`field_base`) Initial thermo-dynamic pressure used in the assosciated state law. It should be defined with as a Champ_Fonc_xyz.

- **[use_total_pressure]**  (*type:* int) Flag (0 or 1) used to activate and use the total pressure in the assosciated  state law. The default value of this Flag is 0.

- **[use_hydrostatic_pressure]**  (*type:* int) Flag (0 or 1) used to activate and use the hydro-static pressure in the assosciated  state law. The default value of this Flag is 0.

- **[use_grad_pression_eos]**  (*type:* int) Flag (0 or 1) used to specify whether or not the gradient of the thermo-dynamic  pressure will be taken into account in the source term of the temperature equation  (case of a non-uniform pressure). The default value of this Flag is 1 which means that the gradient is used in the  source.

- **[time_activate_ptot]**  (*type:* double) Time (in seconds) at which the total pressure will be used in the assosciated  state law.

- **[indice]**  (*type:* :ref:`field_base`) Refractivity of fluid.

- **[kappa]**  (*type:* :ref:`field_base`) Absorptivity of fluid (m-1).

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _milieu_base:

**milieu_base**
---------------


Basic class for medium (physics properties of medium).

Usage:

| **milieu_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

.. _solide:

**solide**
----------

**Inherits from:** :ref:`milieu_base` 


Solid with cp and/or rho non-uniform.

Usage:

| **solide** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rho]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[cp]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[Lambda | lambda_u]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[user_field]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gravite]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diametre_hyd_champ]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[porosites]**  :ref:`porosites`
| }

Parameters are:

- **[rho]**  (*type:* :ref:`field_base`) Density (kg.m-3).

- **[cp]**  (*type:* :ref:`field_base`) Specific heat (J.kg-1.K-1).

- **[Lambda | lambda_u]**  (*type:* :ref:`field_base`) Conductivity (W.m-1.K-1).

- **[user_field]**  (*type:* :ref:`field_base`) user defined field.

- **[gravite]**  (*type:* :ref:`field_base`) Gravity field (optional).

- **[porosites_champ]**  (*type:* :ref:`field_base`) The porosity is given at each element and the porosity at each face,  Psi(face), is calculated by the average of the porosities of the two neighbour elements  Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)). This keyword is optional.

- **[diametre_hyd_champ]**  (*type:* :ref:`field_base`) Hydraulic diameter field (optional).

- **[porosites]**  (*type:* :ref:`porosites`) Porosities.


----

**Keywords derived from modele_turbulence_scal_base**
=====================================================

.. _modele_turbulence_scal_base:

**modele_turbulence_scal_base**
-------------------------------


Basic class for turbulence model for energy equation.

Usage:

| **modele_turbulence_scal_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **turbulence_paroi**  :ref:`turbulence_paroi_scalaire_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_nusselt]**  double
| }

Parameters are:

- **turbulence_paroi**  (*type:* :ref:`turbulence_paroi_scalaire_base`) Keyword to set the wall law.

- **[dt_impr_nusselt]**  (*type:* double) Keyword to print local values of Nusselt number and temperature near a  wall during a turbulent calculation. The values will be printed in the _Nusselt.face file each dt_impr_nusselt time period. The local Nusselt expression is as follows : Nu = ((lambda+lambda_t)/lambda)*d_wall/d_eq  where d_wall is the distance from the first mesh to the wall and d_eq is given by  the wall law. This option also gives the value of d_eq and h = (lambda+lambda_t)/d_eq and the fluid  temperature of the first mesh near the wall.  For the Neumann boundary conditions (flux_impose), the <<equivalent>> wall temperature  given by the wall law is also printed (Tparoi equiv.) preceded for VEF calculation  by the edge temperature <<T face de bord>>.


----

.. _modele_turbulence_scal_nul:

**modele_turbulence_scal_nul**
------------------------------

**Synonyms:** null

**Inherits from:** :ref:`modele_turbulence_scal_base` 


Nul scalar turbulence model (turbulent diffusivity = 0) which can be used with a turbulent 
problem.

Usage:

| **modele_turbulence_scal_nul** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_nusselt]**  double
| }

Parameters are:

- **[dt_impr_nusselt]**  (*type:* double) Keyword to print local values of Nusselt number and temperature near a  wall during a turbulent calculation. The values will be printed in the _Nusselt.face file each dt_impr_nusselt time period. The local Nusselt expression is as follows : Nu = ((lambda+lambda_t)/lambda)*d_wall/d_eq  where d_wall is the distance from the first mesh to the wall and d_eq is given by  the wall law. This option also gives the value of d_eq and h = (lambda+lambda_t)/d_eq and the fluid  temperature of the first mesh near the wall.  For the Neumann boundary conditions (flux_impose), the <<equivalent>> wall temperature  given by the wall law is also printed (Tparoi equiv.) preceded for VEF calculation  by the edge temperature <<T face de bord>>.


----

**Keywords derived from mor_eqn**
=================================

.. _conduction:

**conduction**
--------------

**Inherits from:** :ref:`mor_eqn` 


Heat equation.

Usage:

| **conduction** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_chaleur_qc:

**convection_diffusion_chaleur_qc**
-----------------------------------

**Inherits from:** :ref:`mor_eqn` 


Temperature equation for a quasi-compressible fluid.

Usage:

| **convection_diffusion_chaleur_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mode_calcul_convection]**  string into ["ancien", "divut_moins_tdivu", "divrhout_moins_tdivrhou"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[mode_calcul_convection]**  (*type:* string into ["ancien", "divut_moins_tdivu", "divrhout_moins_tdivrhou"])  Option to  set the form of the convective operatorNL2 divrhouT_moins_Tdivrhou (the default since  1.6.8): rho.u.gradT = div(rho.u.T )- Tdiv(rho.u.1) NL2ancien: u.gradT = div(u.T) -  T.div(u)  divuT_moins_Tdivu : u.gradT = div(u.T) - Tdiv(u.1)

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_chaleur_turbulent_qc:

**convection_diffusion_chaleur_turbulent_qc**
---------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Temperature equation for a quasi-compressible fluid as well as the associated turbulence 
model equations.

Usage:

| **convection_diffusion_chaleur_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_scal_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[mode_calcul_convection]**  string into ["ancien", "divut_moins_tdivu", "divrhout_moins_tdivrhou"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_scal_base`) Turbulence model for the temperature (energy) conservation  equation.

- **[mode_calcul_convection]**  (*type:* string into ["ancien", "divut_moins_tdivu", "divrhout_moins_tdivrhou"])  Option to  set the form of the convective operatorNL2 divrhouT_moins_Tdivrhou (the default since  1.6.8): rho.u.gradT = div(rho.u.T )- Tdiv(rho.u.1) NL2ancien: u.gradT = div(u.T) -  T.div(u)  divuT_moins_Tdivu : u.gradT = div(u.T) - Tdiv(u.1)

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_chaleur_wc:

**convection_diffusion_chaleur_wc**
-----------------------------------

**Inherits from:** :ref:`mor_eqn` 


Temperature equation for a weakly-compressible fluid.

Usage:

| **convection_diffusion_chaleur_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_concentration:

**convection_diffusion_concentration**
--------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Constituent transport vectorial equation (concentration diffusion convection).

Usage:

| **convection_diffusion_concentration** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_inconnue]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[masse_molaire]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alias]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[nom_inconnue]**  (*type:* string) Keyword Nom_inconnue will rename the unknown of this equation with the given  name. In the postprocessing part, the concentration field will be accessible with this  name. This is usefull if you want to track more than one concentration (otherwise, only  the concentration field in the first concentration equation can be accessed).

- **[masse_molaire]**  (*type:* double) not_set

- **[alias]**  (*type:* string) not_set

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_concentration_turbulent:

**convection_diffusion_concentration_turbulent**
------------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Constituent transport equations (concentration diffusion convection) as well as the 
associated turbulence model equations.

Usage:

| **convection_diffusion_concentration_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_scal_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_inconnue]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[masse_molaire]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alias]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_scal_base`) Turbulence model to be used in the constituent transport  equations. The only model currently available is Schmidt.

- **[nom_inconnue]**  (*type:* string) Keyword Nom_inconnue will rename the unknown of this equation with the given  name. In the postprocessing part, the concentration field will be accessible with this  name. This is usefull if you want to track more than one concentration (otherwise, only  the concentration field in the first concentration equation can be accessed).

- **[masse_molaire]**  (*type:* double) not_set

- **[alias]**  (*type:* string) not_set

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_binaire_qc:

**convection_diffusion_espece_binaire_qc**
------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Species conservation equation for a binary quasi-compressible fluid.

Usage:

| **convection_diffusion_espece_binaire_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_binaire_turbulent_qc:

**convection_diffusion_espece_binaire_turbulent_qc**
----------------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Species conservation equation for a binary quasi-compressible fluid as well as the 
associated turbulence model equations.

Usage:

| **convection_diffusion_espece_binaire_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_scal_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_scal_base`) Turbulence model for the species conservation equation.

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_binaire_wc:

**convection_diffusion_espece_binaire_wc**
------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Species conservation equation for a binary weakly-compressible fluid.

Usage:

| **convection_diffusion_espece_binaire_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_multi_qc:

**convection_diffusion_espece_multi_qc**
----------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Species conservation equation for a multi-species quasi-compressible fluid.

Usage:

| **convection_diffusion_espece_multi_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[espece]**  :ref:`espece`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[espece]**  (*type:* :ref:`espece`) Assosciate a species (with its properties) to the equation

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_multi_turbulent_qc:

**convection_diffusion_espece_multi_turbulent_qc**
--------------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


not_set

Usage:

| **convection_diffusion_espece_multi_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_scal_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **espece**  :ref:`espece`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_scal_base`) Turbulence model to be used.

- **espece**  (*type:* :ref:`espece`) not_set

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_espece_multi_wc:

**convection_diffusion_espece_multi_wc**
----------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Species conservation equation for a multi-species weakly-compressible fluid.

Usage:

| **convection_diffusion_espece_multi_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_temperature:

**convection_diffusion_temperature**
------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Energy equation (temperature diffusion convection).

Usage:

| **convection_diffusion_temperature** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[penalisation_l2_ftd]**  :ref:`penalisation_l2_ftd`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[penalisation_l2_ftd]**  (*type:* :ref:`penalisation_l2_ftd`) to activate or not (the default is Direct Forcing method) the  Penalized Direct Forcing method to impose the specified temperature on the solid-fluid  interface.

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _convection_diffusion_temperature_turbulent:

**convection_diffusion_temperature_turbulent**
----------------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Energy equation (temperature diffusion convection) as well as the associated turbulence 
model equations.

Usage:

| **convection_diffusion_temperature_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_scal_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_scal_base`) Turbulence model for the energy equation.

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _echelle_temporelle_turbulente:

**echelle_temporelle_turbulente**
---------------------------------

**Inherits from:** :ref:`mor_eqn` 


Turbulent Dissipation time scale equation for a turbulent mono/multi-phase problem 
(available in TrioCFD)

Usage:

| **echelle_temporelle_turbulente** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _energie_cinetique_turbulente:

**energie_cinetique_turbulente**
--------------------------------

**Inherits from:** :ref:`mor_eqn` 


Turbulent kinetic Energy conservation equation for a turbulent mono/multi-phase problem 
(available in TrioCFD)

Usage:

| **energie_cinetique_turbulente** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _energie_cinetique_turbulente_wit:

**energie_cinetique_turbulente_wit**
------------------------------------

**Inherits from:** :ref:`mor_eqn` 


Bubble Induced Turbulent kinetic Energy equation for a turbulent multi-phase problem 
(available in TrioCFD)

Usage:

| **energie_cinetique_turbulente_wit** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _energie_multiphase:

**energie_multiphase**
----------------------

**Inherits from:** :ref:`mor_eqn` 


Internal energy conservation equation for a multi-phase problem where the unknown 
is the temperature

Usage:

| **energie_multiphase** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _eqn_base:

**eqn_base**
------------

**Inherits from:** :ref:`mor_eqn` 


Basic class for equations.

Usage:

| **eqn_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _masse_multiphase:

**masse_multiphase**
--------------------

**Inherits from:** :ref:`mor_eqn` 


Mass consevation equation for a multi-phase problem where the unknown is the alpha 
(void fraction)

Usage:

| **masse_multiphase** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _mor_eqn:

**mor_eqn**
-----------


Class of equation pieces (morceaux d'equation).

Usage:

| **mor_eqn** *str*
| **Read** *str* {
| }


----

.. _navier_stokes_qc:

**navier_stokes_qc**
--------------------

**Inherits from:** :ref:`mor_eqn` 


Navier-Stokes equation for a quasi-compressible fluid.

Usage:

| **navier_stokes_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode_calcul_pression_initiale]**  string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_bar]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_projection]**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_divu]**  :ref:`floatfloat`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_particulier]**  :ref:`traitement_particulier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_calcul_pression_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_pression]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gradient_pression_qdm_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_pression_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraiter_gradient_pression_sans_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[methode_calcul_pression_initiale]**  (*type:* string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"])   Keyword to select an option for the pressure calculation before the fist time step. Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions  on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions  and f integrating the source terms of the Navier-Stokes equations) and avec_sources_et_operateurs  (lapP=f is solved as with the previous option avec_sources but f integrating also  some operators of the Navier-Stokes equations). The two last options are useful and sometime necessary when source terms are implicited  when using an implicit time scheme to solve the Navier-Stokes equations.

- **[projection_initiale]**  (*type:* int) Keyword to suppress, if boolean equals 0, the initial projection which checks  DivU=0. By default, boolean equals 1.

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[solveur_bar]**  (*type:* :ref:`solveur_sys_base`) This keyword is used to define when filtering operation is called  (typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup  ). A file (solveur.bar) is then created and used for inversion procedure. Syntax is the same then for pressure solver (GCP is required for multi-processor  calculations and, in a general way, for big meshes).

- **[dt_projection]**  (*type:* :ref:`deuxmots`) nb value : This keyword checks every nb time-steps the equality of velocity  divergence to zero. value is the criteria convergency for the solver used.

- **[seuil_divu]**  (*type:* :ref:`floatfloat`) value factor : this keyword is intended to minimise the number of iterations  during the pressure system resolution. The convergence criteria during this step ('seuil' in solveur_pression) is dynamically  adapted according to the mass conservation. At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn). For tn+1, the threshold value seuil(tn+1) will be evualated as:  If ( |max(DivU)*dt|<value )  Seuil(tn+1)= Seuil(tn)*factor  Else  Seuil(tn+1)= Seuil(tn)*factor  Endif  The first parameter (value) is the mass evolution the user is ready to accept per  timestep, and the second one (factor) is the factor of evolution for 'seuil' (for  example 1.1, so 10% per timestep). Investigations has to be lead to know more about the effects of these two last parameters  on the behaviour of the simulations.

- **[traitement_particulier]**  (*type:* :ref:`traitement_particulier`) Keyword to post-process particular values.

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[postraiter_gradient_pression_sans_masse]**  (*type:* flag) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _navier_stokes_standard:

**navier_stokes_standard**
--------------------------

**Inherits from:** :ref:`mor_eqn` 


Navier-Stokes equations.

Usage:

| **navier_stokes_standard** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode_calcul_pression_initiale]**  string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_bar]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_projection]**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_divu]**  :ref:`floatfloat`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_particulier]**  :ref:`traitement_particulier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_calcul_pression_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_pression]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gradient_pression_qdm_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_pression_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraiter_gradient_pression_sans_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[methode_calcul_pression_initiale]**  (*type:* string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"])   Keyword to select an option for the pressure calculation before the fist time step. Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions  on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions  and f integrating the source terms of the Navier-Stokes equations) and avec_sources_et_operateurs  (lapP=f is solved as with the previous option avec_sources but f integrating also  some operators of the Navier-Stokes equations). The two last options are useful and sometime necessary when source terms are implicited  when using an implicit time scheme to solve the Navier-Stokes equations.

- **[projection_initiale]**  (*type:* int) Keyword to suppress, if boolean equals 0, the initial projection which checks  DivU=0. By default, boolean equals 1.

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[solveur_bar]**  (*type:* :ref:`solveur_sys_base`) This keyword is used to define when filtering operation is called  (typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup  ). A file (solveur.bar) is then created and used for inversion procedure. Syntax is the same then for pressure solver (GCP is required for multi-processor  calculations and, in a general way, for big meshes).

- **[dt_projection]**  (*type:* :ref:`deuxmots`) nb value : This keyword checks every nb time-steps the equality of velocity  divergence to zero. value is the criteria convergency for the solver used.

- **[seuil_divu]**  (*type:* :ref:`floatfloat`) value factor : this keyword is intended to minimise the number of iterations  during the pressure system resolution. The convergence criteria during this step ('seuil' in solveur_pression) is dynamically  adapted according to the mass conservation. At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn). For tn+1, the threshold value seuil(tn+1) will be evualated as:  If ( |max(DivU)*dt|<value )  Seuil(tn+1)= Seuil(tn)*factor  Else  Seuil(tn+1)= Seuil(tn)*factor  Endif  The first parameter (value) is the mass evolution the user is ready to accept per  timestep, and the second one (factor) is the factor of evolution for 'seuil' (for  example 1.1, so 10% per timestep). Investigations has to be lead to know more about the effects of these two last parameters  on the behaviour of the simulations.

- **[traitement_particulier]**  (*type:* :ref:`traitement_particulier`) Keyword to post-process particular values.

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[postraiter_gradient_pression_sans_masse]**  (*type:* flag) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _navier_stokes_turbulent:

**navier_stokes_turbulent**
---------------------------

**Inherits from:** :ref:`mor_eqn` 


Navier-Stokes equations as well as the associated turbulence model equations.

Usage:

| **navier_stokes_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_hyd_deriv`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode_calcul_pression_initiale]**  string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_bar]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_projection]**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_divu]**  :ref:`floatfloat`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_particulier]**  :ref:`traitement_particulier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_calcul_pression_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_pression]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gradient_pression_qdm_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_pression_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraiter_gradient_pression_sans_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_hyd_deriv`) Turbulence model for Navier-Stokes equations.

- **[methode_calcul_pression_initiale]**  (*type:* string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"])   Keyword to select an option for the pressure calculation before the fist time step. Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions  on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions  and f integrating the source terms of the Navier-Stokes equations) and avec_sources_et_operateurs  (lapP=f is solved as with the previous option avec_sources but f integrating also  some operators of the Navier-Stokes equations). The two last options are useful and sometime necessary when source terms are implicited  when using an implicit time scheme to solve the Navier-Stokes equations.

- **[projection_initiale]**  (*type:* int) Keyword to suppress, if boolean equals 0, the initial projection which checks  DivU=0. By default, boolean equals 1.

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[solveur_bar]**  (*type:* :ref:`solveur_sys_base`) This keyword is used to define when filtering operation is called  (typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup  ). A file (solveur.bar) is then created and used for inversion procedure. Syntax is the same then for pressure solver (GCP is required for multi-processor  calculations and, in a general way, for big meshes).

- **[dt_projection]**  (*type:* :ref:`deuxmots`) nb value : This keyword checks every nb time-steps the equality of velocity  divergence to zero. value is the criteria convergency for the solver used.

- **[seuil_divu]**  (*type:* :ref:`floatfloat`) value factor : this keyword is intended to minimise the number of iterations  during the pressure system resolution. The convergence criteria during this step ('seuil' in solveur_pression) is dynamically  adapted according to the mass conservation. At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn). For tn+1, the threshold value seuil(tn+1) will be evualated as:  If ( |max(DivU)*dt|<value )  Seuil(tn+1)= Seuil(tn)*factor  Else  Seuil(tn+1)= Seuil(tn)*factor  Endif  The first parameter (value) is the mass evolution the user is ready to accept per  timestep, and the second one (factor) is the factor of evolution for 'seuil' (for  example 1.1, so 10% per timestep). Investigations has to be lead to know more about the effects of these two last parameters  on the behaviour of the simulations.

- **[traitement_particulier]**  (*type:* :ref:`traitement_particulier`) Keyword to post-process particular values.

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[postraiter_gradient_pression_sans_masse]**  (*type:* flag) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _navier_stokes_turbulent_qc:

**navier_stokes_turbulent_qc**
------------------------------

**Inherits from:** :ref:`mor_eqn` 


Navier-Stokes equations under low Mach number as well as the associated turbulence 
model equations.

Usage:

| **navier_stokes_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[modele_turbulence]**  :ref:`modele_turbulence_hyd_deriv`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode_calcul_pression_initiale]**  string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_bar]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_projection]**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_divu]**  :ref:`floatfloat`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_particulier]**  :ref:`traitement_particulier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_calcul_pression_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_pression]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gradient_pression_qdm_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_pression_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraiter_gradient_pression_sans_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[modele_turbulence]**  (*type:* :ref:`modele_turbulence_hyd_deriv`) Turbulence model for Navier-Stokes equations.

- **[methode_calcul_pression_initiale]**  (*type:* string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"])   Keyword to select an option for the pressure calculation before the fist time step. Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions  on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions  and f integrating the source terms of the Navier-Stokes equations) and avec_sources_et_operateurs  (lapP=f is solved as with the previous option avec_sources but f integrating also  some operators of the Navier-Stokes equations). The two last options are useful and sometime necessary when source terms are implicited  when using an implicit time scheme to solve the Navier-Stokes equations.

- **[projection_initiale]**  (*type:* int) Keyword to suppress, if boolean equals 0, the initial projection which checks  DivU=0. By default, boolean equals 1.

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[solveur_bar]**  (*type:* :ref:`solveur_sys_base`) This keyword is used to define when filtering operation is called  (typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup  ). A file (solveur.bar) is then created and used for inversion procedure. Syntax is the same then for pressure solver (GCP is required for multi-processor  calculations and, in a general way, for big meshes).

- **[dt_projection]**  (*type:* :ref:`deuxmots`) nb value : This keyword checks every nb time-steps the equality of velocity  divergence to zero. value is the criteria convergency for the solver used.

- **[seuil_divu]**  (*type:* :ref:`floatfloat`) value factor : this keyword is intended to minimise the number of iterations  during the pressure system resolution. The convergence criteria during this step ('seuil' in solveur_pression) is dynamically  adapted according to the mass conservation. At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn). For tn+1, the threshold value seuil(tn+1) will be evualated as:  If ( |max(DivU)*dt|<value )  Seuil(tn+1)= Seuil(tn)*factor  Else  Seuil(tn+1)= Seuil(tn)*factor  Endif  The first parameter (value) is the mass evolution the user is ready to accept per  timestep, and the second one (factor) is the factor of evolution for 'seuil' (for  example 1.1, so 10% per timestep). Investigations has to be lead to know more about the effects of these two last parameters  on the behaviour of the simulations.

- **[traitement_particulier]**  (*type:* :ref:`traitement_particulier`) Keyword to post-process particular values.

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[postraiter_gradient_pression_sans_masse]**  (*type:* flag) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _navier_stokes_wc:

**navier_stokes_wc**
--------------------

**Inherits from:** :ref:`mor_eqn` 


Navier-Stokes equation for a weakly-compressible fluid.

Usage:

| **navier_stokes_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[methode_calcul_pression_initiale]**  string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_bar]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_projection]**  :ref:`deuxmots`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_divu]**  :ref:`floatfloat`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[traitement_particulier]**  :ref:`traitement_particulier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_calcul_pression_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_projection_initiale]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_matrice_pression]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_vitesse_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gradient_pression_qdm_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_pression_modifie]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraiter_gradient_pression_sans_masse]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[methode_calcul_pression_initiale]**  (*type:* string into ["avec_les_cl", "avec_sources", "avec_sources_et_operateurs", "sans_rien"])   Keyword to select an option for the pressure calculation before the fist time step. Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions  on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions  and f integrating the source terms of the Navier-Stokes equations) and avec_sources_et_operateurs  (lapP=f is solved as with the previous option avec_sources but f integrating also  some operators of the Navier-Stokes equations). The two last options are useful and sometime necessary when source terms are implicited  when using an implicit time scheme to solve the Navier-Stokes equations.

- **[projection_initiale]**  (*type:* int) Keyword to suppress, if boolean equals 0, the initial projection which checks  DivU=0. By default, boolean equals 1.

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[solveur_bar]**  (*type:* :ref:`solveur_sys_base`) This keyword is used to define when filtering operation is called  (typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup  ). A file (solveur.bar) is then created and used for inversion procedure. Syntax is the same then for pressure solver (GCP is required for multi-processor  calculations and, in a general way, for big meshes).

- **[dt_projection]**  (*type:* :ref:`deuxmots`) nb value : This keyword checks every nb time-steps the equality of velocity  divergence to zero. value is the criteria convergency for the solver used.

- **[seuil_divu]**  (*type:* :ref:`floatfloat`) value factor : this keyword is intended to minimise the number of iterations  during the pressure system resolution. The convergence criteria during this step ('seuil' in solveur_pression) is dynamically  adapted according to the mass conservation. At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn). For tn+1, the threshold value seuil(tn+1) will be evualated as:  If ( |max(DivU)*dt|<value )  Seuil(tn+1)= Seuil(tn)*factor  Else  Seuil(tn+1)= Seuil(tn)*factor  Endif  The first parameter (value) is the mass evolution the user is ready to accept per  timestep, and the second one (factor) is the factor of evolution for 'seuil' (for  example 1.1, so 10% per timestep). Investigations has to be lead to know more about the effects of these two last parameters  on the behaviour of the simulations.

- **[traitement_particulier]**  (*type:* :ref:`traitement_particulier`) Keyword to post-process particular values.

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[postraiter_gradient_pression_sans_masse]**  (*type:* flag) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _qdm_multiphase:

**qdm_multiphase**
------------------

**Inherits from:** :ref:`mor_eqn` 


Momentum conservation equation for a multi-phase problem where the unknown is the 
velocity

Usage:

| **qdm_multiphase** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur_pression]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[evanescence]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[solveur_pression]**  (*type:* :ref:`solveur_sys_base`) Linear pressure system resolution method.

- **[evanescence]**  (*type:* :ref:`bloc_lecture`) Management of the vanishing phase (when alpha tends to 0 or 1)

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

.. _taux_dissipation_turbulent:

**taux_dissipation_turbulent**
------------------------------

**Inherits from:** :ref:`mor_eqn` 


Turbulent Dissipation frequency equation for a turbulent mono/multi-phase problem 
(available in TrioCFD)

Usage:

| **taux_dissipation_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_equation_residual]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection]**  :ref:`bloc_convection`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion]**  :ref:`bloc_diffusion`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_limites | boundary_conditions]**  :ref:`condlims`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conditions_initiales | initial_conditions]**  :ref:`condinits`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sources]**  :ref:`sources`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur_bin]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_fichier_xyz_valeur]**  :ref:`ecrire_fichier_xyz_valeur_param`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parametre_equation]**  :ref:`parametre_equation_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  string
| }

Parameters are:

- **[disable_equation_residual]**  (*type:* string) The equation residual will not be used for the problem residual used when  checking time convergence or computing dynamic time-step

- **[convection]**  (*type:* :ref:`bloc_convection`) Keyword to alter the convection scheme.

- **[diffusion]**  (*type:* :ref:`bloc_diffusion`) Keyword to specify the diffusion operator.

- **[conditions_limites | boundary_conditions]**  (*type:* :ref:`condlims`) Boundary conditions.

- **[conditions_initiales | initial_conditions]**  (*type:* :ref:`condinits`) Initial conditions.

- **[sources]**  (*type:* :ref:`sources`) To introduce a source term into an equation (in case of several source terms  into the same equation, the blocks corresponding to the various terms need to be separated  by a comma)

- **[ecrire_fichier_xyz_valeur_bin]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a binary file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[ecrire_fichier_xyz_valeur]**  (*type:* :ref:`ecrire_fichier_xyz_valeur_param`) This keyword is used to write the values of a field  only for some boundaries in a text file with the following format: n_valeur  x_1 y_1 [z_1] val_1  ...  x_n y_n [z_n] val_n  The created files are named : pbname_fieldname_[boundaryname]_time.dat

- **[parametre_equation]**  (*type:* :ref:`parametre_equation_base`) Keyword used to specify additional parameters for the equation

- **[equation_non_resolue]**  (*type:* string) The equation will not be solved while condition(t) is verified if equation_non_resolue  keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1.  Navier_Sokes_Standard  { equation_non_resolue (t>t0)*(t<t1) }


----

**Keywords derived from nom**
=============================

.. _nom:

**nom**
-------


Class to name the TRUST objects.

Usage:

**nom** **[mot]**  

Parameters are:

- **[mot]**  (*type:* string) Chain of characters.


----

.. _nom_anonyme:

**nom_anonyme**
---------------

**Inherits from:** :ref:`nom` 


not_set

Usage:

**nom_anonyme** **[mot]**  

Parameters are:

- **[mot]**  (*type:* string) Chain of characters.


----

**Keywords derived from objet_lecture**
=======================================

.. _bloc_convection:

**bloc_convection**
-------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**bloc_convection** **aco** **operateur** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **operateur**  (*type:* :ref:`convection_deriv`) not_set

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _bloc_couronne:

**bloc_couronne**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Class to create a couronne (2D).

Usage:

**bloc_couronne** **name** **origin | origine** **name3** **ri** **name4** **re**  

Parameters are:

- **name**  (*type:* string into ["origine"])  Keyword to define the center of the circle.

- **origin | origine**  (*type:* double list) Center of the circle.

- **name3**  (*type:* string into ["ri"])  Keyword to define the interior radius.

- **ri**  (*type:* double) Interior radius.

- **name4**  (*type:* string into ["re"])  Keyword to define the exterior radius.

- **re**  (*type:* double) Exterior radius.


----

.. _bloc_criteres_convergence:

**bloc_criteres_convergence**
-----------------------------

**Inherits from:** :ref:`objet_lecture` 


Not set

Usage:

**bloc_criteres_convergence** **bloc_lecture**  

Parameters are:

- **bloc_lecture**  (*type:* string) not_set


----

.. _bloc_decouper:

**bloc_decouper**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Auxiliary class to cut a domain.

Usage:

| **bloc_decouper** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[partitionneur | partition_tool]**  :ref:`partitionneur_deriv`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[larg_joint]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nom_zones | zones_name]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_decoupage]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ecrire_lata]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts_tot]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periodique]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reorder]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[single_hdf]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[print_more_infos]**  int
| }

Parameters are:

- **[partitionneur | partition_tool]**  (*type:* :ref:`partitionneur_deriv`) Defines the partitionning algorithm (the effective C++ object  used is 'Partitionneur_ALGORITHM_NAME').

- **[larg_joint]**  (*type:* int) This keyword specifies the thickness of the virtual ghost domaine (data known  by one processor though not owned by it). The default value is 1 and is generally correct for all algorithms except the QUICK  convection scheme that require a thickness of 2. Since the 1.5.5 version, the VEF discretization imply also a thickness of 2 (except  VEF P0). Any non-zero positive value can be used, but the amount of data to store and exchange  between processors grows quickly with the thickness.

- **[nom_zones | zones_name]**  (*type:* string) Name of the files containing the different partition of the domain. The files will be :  name_0001.Zones  name_0002.Zones  ...  name_000n.Zones. If this keyword is not specified, the geometry is not written on disk (you might  just want to generate a 'ecrire_decoupage' or 'ecrire_lata').

- **[ecrire_decoupage]**  (*type:* string) After having called the partitionning algorithm, the resulting partition  is written on disk in the specified filename. See also partitionneur Fichier_Decoupage. This keyword is useful to change the partition numbers: first, you write the partition  into a file with the option ecrire_decoupage. This file contains the domaine number for each element's mesh. Then you can easily permute domaine numbers in this file. Then read the new partition to create the .Zones files with the Fichier_Decoupage  keyword.

- **[ecrire_lata]**  (*type:* string) not_set

- **[nb_parts_tot]**  (*type:* int) Keyword to generates N .Domaine files, instead of the default number M obtained  after the partitionning algorithm. N must be greater or equal to M. This option might be used to perform coupled parallel computations. Supplemental empty domaines from M to N-1 are created. This keyword is used when you want to run a parallel calculation on several domains  with for example, 2 processors on a first domain and 10 on the second domain because  the first domain is very small compare to second one. You will write Nb_parts 2 and Nb_parts_tot 10 for the first domain and Nb_parts 10  for the second domain.

- **[periodique]**  (*type:* string list) N BOUNDARY_NAME_1 BOUNDARY_NAME_2 ... : N is the number of boundary names given. Periodic boundaries must be declared by this method. The partitionning algorithm will ensure that facing nodes and faces in the periodic  boundaries are located on the same processor.

- **[reorder]**  (*type:* int) If this option is set to 1 (0 by default), the partition is renumbered in  order that the processes which communicate the most are nearer on the network. This may slighlty improves parallel performance.

- **[single_hdf]**  (*type:* flag) Optional keyword to enable you to write the partitioned domaines in a single  file in hdf5 format.

- **[print_more_infos]**  (*type:* int) If this option is set to 1 (0 by default), print infos about number of remote  elements (ghosts) and additional infos about the quality of partitionning. Warning, it slows down the cutting operations.


----

.. _bloc_diffusion:

**bloc_diffusion**
------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**bloc_diffusion** **aco** **[operateur]** **[op_implicite]** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **[operateur]**  (*type:* :ref:`diffusion_deriv`) if none is specified, the diffusive scheme used is a 2nd-order scheme.

- **[op_implicite]**  (*type:* :ref:`op_implicite`) To have diffusive implicitation, it use Uzawa algorithm. Very useful when viscosity has large variations.

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _bloc_diffusion_standard:

**bloc_diffusion_standard**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


grad_Ubar 1 makes the gradient calculated through the filtered values of velocity 
(P1-conform).NL2 nu 1 (respectively nut 1) takes the molecular viscosity (eddy viscosity) 
into account in the velocity gradient part of the diffusion expression.

nu_transp 1 (respectively nut_transp 1) takes the molecular viscosity (eddy viscosity) 
into account according in the TRANSPOSED velocity gradient part of the diffusion expression.NL2 
filtrer_resu 1 allows to filter the resulting diffusive fluxes contribution.

Usage:

**bloc_diffusion_standard** **mot1** **val1** **mot2** **val2** **mot3** **val3** **mot4** **val4** **mot5** **val5** **mot6** **val6**  

Parameters are:

- **mot1**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val1**  (*type:* int into [0,1])  not_set

- **mot2**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val2**  (*type:* int into [0,1])  not_set

- **mot3**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val3**  (*type:* int into [0,1])  not_set

- **mot4**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val4**  (*type:* int into [0,1])  not_set

- **mot5**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val5**  (*type:* int into [0,1])  not_set

- **mot6**  (*type:* string into ["grad_ubar", "nu", "nut", "nu_transp", "nut_transp", "filtrer_resu"])  not_set

- **val6**  (*type:* int into [0,1])  not_set


----

.. _bloc_ef:

**bloc_ef**
-----------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**bloc_ef** **mot1** **val1** **mot2** **val2** **mot3** **val3** **mot4** **val4**  

Parameters are:

- **mot1**  (*type:* string into ["transportant_bar", "transporte_bar", "filtrer_resu", "antisym"])  not_set

- **val1**  (*type:* int into [0,1])  not_set

- **mot2**  (*type:* string into ["transportant_bar", "transporte_bar", "filtrer_resu", "antisym"])  not_set

- **val2**  (*type:* int into [0,1])  not_set

- **mot3**  (*type:* string into ["transportant_bar", "transporte_bar", "filtrer_resu", "antisym"])  not_set

- **val3**  (*type:* int into [0,1])  not_set

- **mot4**  (*type:* string into ["transportant_bar", "transporte_bar", "filtrer_resu", "antisym"])  not_set

- **val4**  (*type:* int into [0,1])  not_set


----

.. _bloc_lec_champ_init_canal_sinal:

**bloc_lec_champ_init_canal_sinal**
-----------------------------------

**Inherits from:** :ref:`objet_lecture` 


Parameters for the class champ_init_canal_sinal.

in 2D:

U=ucent*y(2h-y)/h/h

V=ampli_bruit*rand+ampli_sin*sin(omega*x)

rand: unpredictable value between -1 and 1.

in 3D:

U=ucent*y(2h-y)/h/h

V=ampli_bruit*rand1+ampli_sin*sin(omega*x)

W=ampli_bruit*rand2

rand1 and rand2: unpredictables values between -1 and 1.

Usage:

| **bloc_lec_champ_init_canal_sinal** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **ucent**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **h**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **ampli_bruit**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ampli_sin]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **omega**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dir_flow]**  int into [0,1,2]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dir_wall]**  int into [0,1,2]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[min_dir_flow]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[min_dir_wall]**  double
| }

Parameters are:

- **ucent**  (*type:* double) Velocity value at the center of the channel.

- **h**  (*type:* double) Half hength of the channel.

- **ampli_bruit**  (*type:* double) Amplitude for the disturbance.

- **[ampli_sin]**  (*type:* double) Amplitude for the sinusoidal disturbance (by default equals to ucent/10).

- **omega**  (*type:* double) Value of pulsation for the of the sinusoidal disturbance.

- **[dir_flow]**  (*type:* int into [0,1,2])  Flow direction for the initialization of the flow in a channel.  - if dir_flow=0, the flow direction is X  - if dir_flow=1, the flow direction is Y  - if dir_flow=2, the flow direction is Z  Default value for dir_flow is 0

- **[dir_wall]**  (*type:* int into [0,1,2])  Wall direction for the initialization of the flow in a channel.  - if dir_wall=0, the normal to the wall is in X direction  - if dir_wall=1, the normal to the wall is in Y direction  - if dir_wall=2, the normal to the wall is in Z direction  Default value for dir_flow is 1

- **[min_dir_flow]**  (*type:* double) Value of the minimum coordinate in the flow direction for the initialization  of the flow in a channel. Default value for dir_flow is 0.

- **[min_dir_wall]**  (*type:* double) Value of the minimum coordinate in the wall direction for the initialization  of the flow in a channel. Default value for dir_flow is 0.


----

.. _bloc_lecture:

**bloc_lecture**
----------------

**Inherits from:** :ref:`objet_lecture` 


to read between two braces

Usage:

**bloc_lecture** **bloc_lecture**  

Parameters are:

- **bloc_lecture**  (*type:* string) not_set


----

.. _bloc_lecture_poro:

**bloc_lecture_poro**
---------------------

**Inherits from:** :ref:`objet_lecture` 


Surface and volume porosity values.

Usage:

| **bloc_lecture_poro** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **volumique**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **surfacique**  list
| }

Parameters are:

- **volumique**  (*type:* double) Volume porosity value.

- **surfacique**  (*type:* list) Surface porosity values (in X, Y, Z directions).


----

.. _bloc_origine_cotes:

**bloc_origine_cotes**
----------------------

**Inherits from:** :ref:`objet_lecture` 


Class to create a rectangle (or a box).

Usage:

**bloc_origine_cotes** **name** **origin | origine** **name2** **cotes**  

Parameters are:

- **name**  (*type:* string into ["origine"])  Keyword to define the origin of the rectangle (or the box).

- **origin | origine**  (*type:* double list) Coordinates of the origin of the rectangle (or the box).

- **name2**  (*type:* string into ["cotes"])  Keyword to define the length along the axes.

- **cotes**  (*type:* double list) Length along the axes.


----

.. _bloc_pave:

**bloc_pave**
-------------

**Inherits from:** :ref:`objet_lecture` 


Class to create a pave.

Usage:

| **bloc_pave** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[origine]**  double list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[longueurs]**  double list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nombre_de_noeuds]**  int list - size is dimension
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facteurs]**  double list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[symx]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[symy]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[symz]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[xtanh]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[xtanh_dilatation]**  int into [-1,0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[xtanh_taille_premiere_maille]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ytanh]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ytanh_dilatation]**  int into [-1,0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ytanh_taille_premiere_maille]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ztanh]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ztanh_dilatation]**  int into [-1,0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ztanh_taille_premiere_maille]**  double
| }

Parameters are:

- **[origine]**  (*type:* double list) Keyword to define the pave (block) origin, that is to say one of the 8 block  points (or 4 in a 2D coordinate system).

- **[longueurs]**  (*type:* double list) Keyword to define the block dimensions, that is to say knowing the origin,  length along the axes.

- **[nombre_de_noeuds]**  (*type:* int list - size is dimension) Keyword to define the discretization (nodenumber) in each direction.

- **[facteurs]**  (*type:* double list) Keyword to define stretching factors for mesh discretization in each direction. This is a real number which must be positive (by default 1.0). A stretching factor other than 1 allows refinement on one edge in one direction.

- **[symx]**  (*type:* flag) Keyword to define a block mesh that is symmetrical with respect to the YZ plane  (respectively Y-axis in 2D) passing through the block centre.

- **[symy]**  (*type:* flag) Keyword to define a block mesh that is symmetrical with respect to the XZ plane  (respectively X-axis in 2D) passing through the block centre.

- **[symz]**  (*type:* flag) Keyword defining a block mesh that is symmetrical with respect to the XY plane  passing through the block centre.

- **[xtanh]**  (*type:* double) Keyword to generate mesh with tanh (hyperbolic tangent) variation in the  X-direction.

- **[xtanh_dilatation]**  (*type:* int into [-1,0,1])  Keyword to generate mesh with tanh (hyperbolic tangent) variation  in the X-direction. xtanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle  of the channel and smaller near the walls -1: coarse mesh at the left side of the  channel and smaller at the right side 1: coarse mesh at the right side of the channel  and smaller near the left side of the channel.

- **[xtanh_taille_premiere_maille]**  (*type:* double) Size of the first cell of the mesh with tanh (hyperbolic tangent) variation  in the X-direction.

- **[ytanh]**  (*type:* double) Keyword to generate mesh with tanh (hyperbolic tangent) variation in the  Y-direction.

- **[ytanh_dilatation]**  (*type:* int into [-1,0,1])  Keyword to generate mesh with tanh (hyperbolic tangent) variation  in the Y-direction. ytanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle  of the channel and smaller near the walls -1: coarse mesh at the bottom of the channel  and smaller near the top 1: coarse mesh at the top of the channel and smaller near  the bottom.

- **[ytanh_taille_premiere_maille]**  (*type:* double) Size of the first cell of the mesh with tanh (hyperbolic tangent) variation  in the Y-direction.

- **[ztanh]**  (*type:* double) Keyword to generate mesh with tanh (hyperbolic tangent) variation in the  Z-direction.

- **[ztanh_dilatation]**  (*type:* int into [-1,0,1])  Keyword to generate mesh with tanh (hyperbolic tangent) variation  in the Z-direction. tanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle  of the channel and smaller near the walls -1: coarse mesh at the back of the channel  and smaller near the front 1: coarse mesh at the front of the channel and smaller  near the back.

- **[ztanh_taille_premiere_maille]**  (*type:* double) Size of the first cell of the mesh with tanh (hyperbolic tangent) variation  in the Z-direction.


----

.. _bloc_pdf_model:

**bloc_pdf_model**
------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **bloc_pdf_model** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **eta**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[temps_relaxation_coefficient_pdf]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[echelle_relaxation_coefficient_pdf]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[local]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[vitesse_imposee_data]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[vitesse_imposee_fonction]**  :ref:`troismots`
| }

Parameters are:

- **eta**  (*type:* double) penalization coefficient

- **[temps_relaxation_coefficient_pdf]**  (*type:* double) time relaxation on the forcing term to help

- **[echelle_relaxation_coefficient_pdf]**  (*type:* double) time relaxation on the forcing term to help convergence

- **[local]**  (*type:* flag) rien whether the prescribed velocity is expressed in the global or local basis

- **[vitesse_imposee_data]**  (*type:* :ref:`field_base`) Prescribed velocity as a field

- **[vitesse_imposee_fonction]**  (*type:* :ref:`troismots`) Prescribed velocity as a set of ananlytical component


----

.. _bloc_sutherland:

**bloc_sutherland**
-------------------

**Inherits from:** :ref:`objet_lecture` 


Sutherland law for viscosity mu(T)=mu0*((T0+C)/(T+C))*(T/T0)**1.5 and (optional) for 
conductivity lambda(T)=mu0*Cp/Prandtl*((T0+Slambda)/(T+Slambda))*(T/T0)**1.5

Usage:

**bloc_sutherland** **problem_name** **mu0** **mu0_val** **t0** **t0_val** **[slambda]** **[s]** **c** **c_val**  

Parameters are:

- **problem_name**  (*type:* :ref:`pb_base`) Name of problem.

- **mu0**  (*type:* string into ["mu0"])  not_set

- **mu0_val**  (*type:* double) not_set

- **t0**  (*type:* string into ["t0"])  not_set

- **t0_val**  (*type:* double) not_set

- **[slambda]**  (*type:* string into ["slambda"])  not_set

- **[s]**  (*type:* double) not_set

- **c**  (*type:* string into ["c"])  not_set

- **c_val**  (*type:* double) not_set


----

.. _bloc_tube:

**bloc_tube**
-------------

**Inherits from:** :ref:`objet_lecture` 


Class to create a tube (3D).

Usage:

**bloc_tube** **name** **origin | origine** **name2** **direction** **name3** **ri** **name4** **re** **name5** **h**  

Parameters are:

- **name**  (*type:* string into ["origine"])  Keyword to define the center of the tube.

- **origin | origine**  (*type:* double list) Center of the tube.

- **name2**  (*type:* string into ["dir"])  Keyword to define the direction of the main axis.

- **direction**  (*type:* string into ["x", "y", "z"])  direction of the main axis X, Y or Z

- **name3**  (*type:* string into ["ri"])  Keyword to define the interior radius.

- **ri**  (*type:* double) Interior radius.

- **name4**  (*type:* string into ["re"])  Keyword to define the exterior radius.

- **re**  (*type:* double) Exterior radius.

- **name5**  (*type:* string into ["hauteur"])  Keyword to define the heigth of the tube.

- **h**  (*type:* double) Heigth of the tube.


----

.. _bord:

**bord**
--------

**Inherits from:** :ref:`objet_lecture` 


The block side is not in contact with another block and boundary conditions are applied 
to it.

Usage:

**bord** **nom** **defbord**  

Parameters are:

- **nom**  (*type:* string) Name of block side.

- **defbord**  (*type:* :ref:`defbord`) Definition of block side.


----

.. _bord_base:

**bord_base**
-------------

**Inherits from:** :ref:`objet_lecture` 


Basic class for block sides.
Block sides that are neither edges nor connectors are not specified.
The duplicate nodes of two blocks in contact are automatically recognized and deleted.

Usage:

| **bord_base** *str*
| **Read** *str* {
| }


----

.. _bords_ecrire:

**bords_ecrire**
----------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**bords_ecrire** **chaine** **bords**  

Parameters are:

- **chaine**  (*type:* string into ["bords"])  not_set

- **bords**  (*type:* string list) Keyword to post-process only on some boundaries :  bords nb_bords boundary1 ... boundaryn  where  nb_bords : number of boundaries  boundary1 ... boundaryn : name of the boundaries.


----

.. _calcul:

**calcul**
----------

**Inherits from:** :ref:`objet_lecture` 


The centre of gravity will be calculated.

Usage:

**calcul**  


----

.. _canal:

**canal**
---------

**Inherits from:** :ref:`objet_lecture` 


Keyword for statistics on a periodic plane channel.

Usage:

| **canal** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_moy_spat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_moy_temp]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[debut_stat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fin_stat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pulsation_w]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_points_par_phase]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  string
| }

Parameters are:

- **[dt_impr_moy_spat]**  (*type:* double) Period to print the spatial average (default value is 1e6).

- **[dt_impr_moy_temp]**  (*type:* double) Period to print the temporal average (default value is 1e6).

- **[debut_stat]**  (*type:* double) Time to start the temporal averaging (default value is 1e6).

- **[fin_stat]**  (*type:* double) Time to end the temporal averaging (default value is 1e6).

- **[pulsation_w]**  (*type:* double) Pulsation for phase averaging (in case of pulsating forcing term) (no  default value).

- **[nb_points_par_phase]**  (*type:* int) Number of samples to represent phase average all along a period (no default  value).

- **[reprise]**  (*type:* string) val_moy_temp_xxxxxx.sauv : Keyword to resume a calculation with previous  averaged quantities.  Note that for thermal and turbulent problems, averages on temperature and turbulent  viscosity are automatically calculated. To resume a calculation with phase averaging, val_moy_temp_xxxxxx.sauv_phase file  is required on the directory where the job is submitted (this last file will be then  automatically loaded by TRUST).


----

.. _centre_de_gravite:

**centre_de_gravite**
---------------------

**Inherits from:** :ref:`objet_lecture` 


To specify the centre of gravity.

Usage:

**centre_de_gravite** **point**  

Parameters are:

- **point**  (*type:* :ref:`un_point`) A centre of gravity.


----

.. _champ_a_post:

**champ_a_post**
----------------

**Inherits from:** :ref:`objet_lecture` 


Field to be post-processed.

Usage:

**champ_a_post** **champ** **[localisation]**  

Parameters are:

- **champ**  (*type:* string) Name of the post-processed field.

- **[localisation]**  (*type:* string into ["elem", "som", "faces"])  Localisation of post-processed field values:  The two available values are elem, som, or faces (LATA format only) used respectively  to select field values at mesh centres (CHAMPMAILLE type field in the lml file) or  at mesh nodes (CHAMPPOINT type field in the lml file). If no selection is made, localisation is set to som by default.


----

.. _champs_posts:

**champs_posts**
----------------

**Inherits from:** :ref:`objet_lecture` 


Field's write mode.

Usage:

**champs_posts** **[format]** **mot** **period** **champs | fields**  

Parameters are:

- **[format]**  (*type:* string into ["binaire", "formatte"])  Type of file.

- **mot**  (*type:* string into ["dt_post", "nb_pas_dt_post"])  Keyword to set the kind of the field's  write frequency. Either a time period or a time step period.

- **period**  (*type:* string) Value of the period which can be like (2.*t).

- **champs | fields**  (*type:* :ref:`champs_a_post`) Post-processed fields.


----

.. _chmoy_faceperio:

**chmoy_faceperio**
-------------------

**Inherits from:** :ref:`objet_lecture` 


non documente

Usage:

**chmoy_faceperio** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) not_set


----

.. _circle_3:

**circle_3**
------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define several probes located on a circle (in 3-D space).

Usage:

**circle_3** **nbr** **point_deb** **direction** **radius** **theta1** **theta2**  

Parameters are:

- **nbr**  (*type:* int) Number of probes between teta1 and teta2 (angles given in degrees).

- **point_deb**  (*type:* :ref:`un_point`) Center of the circle.

- **direction**  (*type:* int into [0,1,2])  Axis normal to the circle plane (0:x axis, 1:y axis, 2:z axis).

- **radius**  (*type:* double) Radius of the circle.

- **theta1**  (*type:* double) First angle.

- **theta2**  (*type:* double) Second angle.


----

.. _circle:

**circle**
----------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define several probes located on a circle.

Usage:

**circle** **nbr** **point_deb** **[direction]** **radius** **theta1** **theta2**  

Parameters are:

- **nbr**  (*type:* int) Number of probes between teta1 and teta2 (angles given in degrees).

- **point_deb**  (*type:* :ref:`un_point`) Center of the circle.

- **[direction]**  (*type:* int into [0,1,2])  Axis normal to the circle plane (0:x axis, 1:y axis, 2:z axis).

- **radius**  (*type:* double) Radius of the circle.

- **theta1**  (*type:* double) First angle.

- **theta2**  (*type:* double) Second angle.


----

.. _coarsen_operator_uniform:

**coarsen_operator_uniform**
----------------------------

**Inherits from:** :ref:`objet_lecture` 


Object defining the uniform coarsening process of the given grid in IJK discretization

Usage:

**coarsen_operator_uniform** **[coarsen_operator_uniform]** **aco** **[coarsen_i]** **[coarsen_i_val]** **[coarsen_j]** **[coarsen_j_val]** **[coarsen_k]** **[coarsen_k_val]** **acof**  

Parameters are:

- **[coarsen_operator_uniform]**  (*type:* string) not_set

- **aco**  (*type:* string into ["{"])  opening curly brace

- **[coarsen_i]**  (*type:* string into ["coarsen_i"])  not_set

- **[coarsen_i_val]**  (*type:* int) Integer indicating the number by which we will divide the number of elements  in the I direction (in order to obtain a coarser grid)

- **[coarsen_j]**  (*type:* string into ["coarsen_j"])  not_set

- **[coarsen_j_val]**  (*type:* int) Integer indicating the number by which we will divide the number of elements  in the J direction (in order to obtain a coarser grid)

- **[coarsen_k]**  (*type:* string into ["coarsen_k"])  not_set

- **[coarsen_k_val]**  (*type:* int) Integer indicating the number by which we will divide the number of elements  in the K direction (in order to obtain a coarser grid)

- **acof**  (*type:* string into ["}"])  closing curly brace


----

.. _condinit:

**condinit**
------------

**Inherits from:** :ref:`objet_lecture` 


Initial condition.

Usage:

**condinit** **nom** **ch**  

Parameters are:

- **nom**  (*type:* string) Name of initial condition field.

- **ch**  (*type:* :ref:`field_base`) Type field and the initial values.


----

.. _condlimlu:

**condlimlu**
-------------

**Inherits from:** :ref:`objet_lecture` 


Boundary condition specified.

Usage:

**condlimlu** **bord** **cl**  

Parameters are:

- **bord**  (*type:* string) Name of the edge where the boundary condition applies.

- **cl**  (*type:* :ref:`condlim_base`) Boundary condition at the boundary called bord (edge).


----

.. _convection_ale:

**convection_ale**
------------------

**Synonyms:** ale

**Inherits from:** :ref:`objet_lecture` 


A convective scheme for ALE (Arbitrary Lagrangian-Eulerian) framework.

Usage:

**convection_ale** **opconv**  

Parameters are:

- **opconv**  (*type:* :ref:`bloc_convection`) Choice between: amont and muscl  Example: convection { ALE { amont } }


----

.. _convection_amont:

**convection_amont**
--------------------

**Synonyms:** amont

**Inherits from:** :ref:`objet_lecture` 


Keyword for upwind scheme for VDF or VEF discretizations.
In VEF discretization equivalent to generic amont for TRUST version 1.5 or later.
The previous upwind scheme can be used with the obsolete in future amont_old keyword.

Usage:

**convection_amont**  


----

.. _convection_amont_old:

**convection_amont_old**
------------------------

**Synonyms:** amont_old

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization, obsolete keyword, see amont.

Usage:

**convection_amont_old**  


----

.. _convection_btd:

**convection_btd**
------------------

**Synonyms:** btd

**Inherits from:** :ref:`objet_lecture` 


Only for EF discretization.

Usage:

| **convection_btd** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **btd**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **facteur**  double
| }

Parameters are:

- **btd**  (*type:* double) not_set

- **facteur**  (*type:* double) not_set


----

.. _convection_centre4:

**convection_centre4**
----------------------

**Synonyms:** centre4

**Inherits from:** :ref:`objet_lecture` 


For VDF and VEF discretizations.

Usage:

**convection_centre4**  


----

.. _convection_centre:

**convection_centre**
---------------------

**Synonyms:** centre

**Inherits from:** :ref:`objet_lecture` 


For VDF and VEF discretizations.

Usage:

**convection_centre**  


----

.. _convection_centre_old:

**convection_centre_old**
-------------------------

**Synonyms:** centre_old

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization.

Usage:

**convection_centre_old**  


----

.. _convection_deriv:

**convection_deriv**
--------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**convection_deriv**  


----

.. _convection_di_l2:

**convection_di_l2**
--------------------

**Synonyms:** di_l2

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization.

Usage:

**convection_di_l2**  


----

.. _convection_ef:

**convection_ef**
-----------------

**Synonyms:** ef

**Inherits from:** :ref:`objet_lecture` 


For VEF calculations, a centred convective scheme based on Finite Elements formulation 
can be called through the following data:NL2

Convection { EF transportant_bar val transporte_bar val antisym val filtrer_resu 
val }NL2

This scheme is 2nd order accuracy (and get better the property of kinetic energy 
conservation).
Due to possible problems of instabilities phenomena, this scheme has to be coupled 
with stabilisation process (see Source_Qdm_lambdaup).These two last data are equivalent 
from a theoretical point of view in variationnal writing to : div(( u.
grad ub , vb) - (u.
grad vb, ub)), where vb corresponds to the filtered reference test functions.NL2

Remark:NL2 This class requires to define a filtering operator : see solveur_bar

Usage:

**convection_ef** **[mot1]** **[bloc_ef]**  

Parameters are:

- **[mot1]**  (*type:* string into ["defaut_bar"])  equivalent to transportant_bar 0 transporte_bar 1 filtrer_resu  1 antisym 1

- **[bloc_ef]**  (*type:* :ref:`bloc_ef`) not_set


----

.. _convection_ef_stab:

**convection_ef_stab**
----------------------

**Synonyms:** ef_stab

**Inherits from:** :ref:`objet_lecture` 


Keyword for a VEF convective scheme.

Usage:

| **convection_ef_stab** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[test]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tdivu]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[old]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[volumes_etendus]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[volumes_non_etendus]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[amont_sous_zone]**  :ref:`sous_zone`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha_sous_zone]**  :ref:`listsous_zone_valeur`
| }

Parameters are:

- **[alpha]**  (*type:* double) To weight the scheme centering with the factor double (between 0 (full  centered) and 1 (mix between upwind and centered), by default 1). For scalar equation, it is adviced to use alpha=1 and for the momentum equation,  alpha=0.2 is adviced.

- **[test]**  (*type:* int) Developer option to compare old and new version of EF_stab

- **[tdivu]**  (*type:* flag) To have the convective operator calculated as div(TU)-TdivU(=UgradT).

- **[old]**  (*type:* flag) To use old version of EF_stab scheme (default no).

- **[volumes_etendus]**  (*type:* flag) Option for the scheme to use the extended volumes (default, yes).

- **[volumes_non_etendus]**  (*type:* flag) Option for the scheme to not use the extended volumes (default, no).

- **[amont_sous_zone]**  (*type:* :ref:`sous_zone`) Option to degenerate EF_stab scheme into Amont (upwind) scheme in  the sub zone of name sz_name. The sub zone may be located arbitrarily in the domain but the more often this option  will be activated in a zone where EF_stab scheme generates instabilities as for free  outlet for example.

- **[alpha_sous_zone]**  (*type:* :ref:`listsous_zone_valeur`) Option to change locally the alpha value on N sub-zones named  sub_zone_name_I. Generally, it is used to prevent from a local divergence by increasing locally the  alpha parameter.


----

.. _convection_generic:

**convection_generic**
----------------------

**Synonyms:** generic

**Inherits from:** :ref:`objet_lecture` 


Keyword for generic calling of upwind and muscl convective scheme in VEF discretization.
For muscl scheme, limiters and order for fluxes calculations have to be specified.
The available limiters are : minmod - vanleer -vanalbada - chakravarthy - superbee, 
and the order of accuracy is 1 or 2.
Note that chakravarthy is a non-symmetric limiter and superbee may engender results 
out of physical limits.
By consequence, these two limiters are not recommended.

Examples:

convection { generic amont }NL2 convection { generic muscl minmod 1 }NL2 convection 
{ generic muscl vanleer 2 }NL2

In case of results out of physical limits with muscl scheme (due for instance to 
strong non-conformal velocity flow field), user can redefine in data file a lower 
order and a smoother limiter, as : convection { generic muscl minmod 1 }

Usage:

**convection_generic** **type** **[limiteur]** **[ordre]** **[alpha]**  

Parameters are:

- **type**  (*type:* string into ["amont", "muscl", "centre"])  type of scheme

- **[limiteur]**  (*type:* string into ["minmod", "vanleer", "vanalbada", "chakravarthy", "superbee"])  type of  limiter

- **[ordre]**  (*type:* int into [1,2,3])  order of accuracy

- **[alpha]**  (*type:* double) alpha


----

.. _convection_kquick:

**convection_kquick**
---------------------

**Synonyms:** kquick

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization.

Usage:

**convection_kquick**  


----

.. _convection_muscl3:

**convection_muscl3**
---------------------

**Synonyms:** muscl3

**Inherits from:** :ref:`objet_lecture` 


Keyword for a scheme using a ponderation between muscl and center schemes in VEF.

Usage:

| **convection_muscl3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha]**  double
| }

Parameters are:

- **[alpha]**  (*type:* double) To weight the scheme centering with the factor double (between 0 (full  centered) and 1 (muscl), by default 1).


----

.. _convection_muscl:

**convection_muscl**
--------------------

**Synonyms:** muscl

**Inherits from:** :ref:`objet_lecture` 


Keyword for muscl scheme in VEF discretization equivalent to generic muscl vanleer 
2 for the 1.5 version or later.
The previous muscl scheme can be used with the obsolete in future muscl_old keyword.

Usage:

**convection_muscl**  


----

.. _convection_muscl_new:

**convection_muscl_new**
------------------------

**Synonyms:** muscl_new

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization.

Usage:

**convection_muscl_new**  


----

.. _convection_muscl_old:

**convection_muscl_old**
------------------------

**Synonyms:** muscl_old

**Inherits from:** :ref:`objet_lecture` 


Only for VEF discretization.

Usage:

**convection_muscl_old**  


----

.. _convection_negligeable:

**convection_negligeable**
--------------------------

**Synonyms:** negligeable

**Inherits from:** :ref:`objet_lecture` 


For VDF and VEF discretizations.
Suppresses the convection operator.

Usage:

**convection_negligeable**  


----

.. _convection_quick:

**convection_quick**
--------------------

**Synonyms:** quick

**Inherits from:** :ref:`objet_lecture` 


Only for VDF discretization.

Usage:

**convection_quick**  


----

.. _convection_supg:

**convection_supg**
-------------------

**Synonyms:** supg

**Inherits from:** :ref:`objet_lecture` 


Only for EF discretization.

Usage:

| **convection_supg** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **facteur**  double
| }

Parameters are:

- **facteur**  (*type:* double) not_set


----

.. _corps_postraitement:

**corps_postraitement**
-----------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **corps_postraitement** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[format]**  string into ["lml", "lata", "single_lata", "lata_v2", "med", "med_major"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_domaine | sous_zone]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parallele]**  string into ["simple", "multiple", "mpi-io"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[definition_champs]**  :ref:`definition_champs`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[definition_champs_fichier | definition_champs_file]**  :ref:`definition_champs_fichier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes | probes]**  :ref:`sondes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes_mobiles | mobile_probes]**  :ref:`sondes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes_fichier | probes_file]**  :ref:`sondes_fichier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[deprecatedkeepduplicatedprobes]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[champs | fields]**  :ref:`champs_posts`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[statistiques]**  :ref:`stats_posts`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[statistiques_en_serie]**  :ref:`stats_serie_posts`
| }

Parameters are:

- **[fichier]**  (*type:* string) Name of file.

- **[format]**  (*type:* string into ["lml", "lata", "single_lata", "lata_v2", "med", "med_major"])  This optional  parameter specifies the format of the output file. The basename used for the output file is the basename of the data file. For the fmt parameter, choices are lml or lata. A short description of each format can be found below. The default value is lml. single_lata is not compatible with 64 bits integer version.

- **[domaine]**  (*type:* string) This optional parameter specifies the domain on which the data should be  interpolated before it is written in the output file. The default is to write the data on the domain of the current problem (no interpolation).

- **[sous_domaine | sous_zone]**  (*type:* string) This optional parameter specifies the sub_domaine on which the data should  be interpolated before it is written in the output file. It is only available for sequential computation.

- **[parallele]**  (*type:* string into ["simple", "multiple", "mpi-io"])  Select simple (single file, sequential  write), multiple (several files, parallel write), or mpi-io (single file, parallel  write) for LATA format

- **[definition_champs]**  (*type:* :ref:`definition_champs`) Keyword to create new or more complex field for advanced postprocessing.

- **[definition_champs_fichier | definition_champs_file]**  (*type:* :ref:`definition_champs_fichier`) Definition_champs read from file.

- **[sondes | probes]**  (*type:* :ref:`sondes`) Probe.

- **[sondes_mobiles | mobile_probes]**  (*type:* :ref:`sondes`) Mobile probes useful for ALE, their positions will be updated in the mesh.

- **[sondes_fichier | probes_file]**  (*type:* :ref:`sondes_fichier`) Probe read in a file.

- **[deprecatedkeepduplicatedprobes]**  (*type:* int) Flag to not remove duplicated probes in .son files (1: keep duplicate probes,  0: remove duplicate probes)

- **[champs | fields]**  (*type:* :ref:`champs_posts`) Field's write mode.

- **[statistiques]**  (*type:* :ref:`stats_posts`) Statistics between two points fixed : start of integration time and  end of integration time.

- **[statistiques_en_serie]**  (*type:* :ref:`stats_serie_posts`) Statistics between two points not fixed : on period of integration.


----

.. _defbord_3:

**defbord_3**
-------------

**Inherits from:** :ref:`objet_lecture` 


2-D edge (plane) in the 3-D space.

Usage:

**defbord_3** **dir** **eq** **pos** **pos2_min** **inf1** **dir2** **inf2** **pos2_max** **pos3_min** **inf3** **dir3** **inf4** **pos3_max**  

Parameters are:

- **dir**  (*type:* string into ["x", "y", "z"])  Edge is perpendicular to this direction.

- **eq**  (*type:* string into [" "])  Equality sign.

- **pos**  (*type:* double) Position value.

- **pos2_min**  (*type:* double) Minimal value.

- **inf1**  (*type:* string into ["< "])  Less than or equal to sign.

- **dir2**  (*type:* string into ["x", "y"])  Edge is parallel to this direction.

- **inf2**  (*type:* string into ["< "])  Less than or equal to sign.

- **pos2_max**  (*type:* double) Maximal value.

- **pos3_min**  (*type:* double) Minimal value.

- **inf3**  (*type:* string into ["< "])  Less than or equal to sign.

- **dir3**  (*type:* string into ["y", "z"])  Edge is parallel to this direction.

- **inf4**  (*type:* string into ["< "])  Less than or equal to sign.

- **pos3_max**  (*type:* double) Maximal value.


----

.. _defbord:

**defbord**
-----------

**Inherits from:** :ref:`objet_lecture` 


Class to define an edge.

Usage:

**defbord**  


----

.. _definition_champ:

**definition_champ**
--------------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to create new complex field for advanced postprocessing.

Usage:

**definition_champ** **name** **champ_generique**  

Parameters are:

- **name**  (*type:* string) The name of the new created field.

- **champ_generique**  (*type:* :ref:`champ_generique_base`) not_set


----

.. _definition_champs_fichier:

**definition_champs_fichier**
-----------------------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to read definition_champs from a file

Usage:

| **definition_champs_fichier** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fichier | file**  string
| }

Parameters are:

- **fichier | file**  (*type:* string) name of file containing the definition of advanced fields


----

.. _deuxentiers:

**deuxentiers**
---------------

**Inherits from:** :ref:`objet_lecture` 


Two integers.

Usage:

**deuxentiers** **int1** **int2**  

Parameters are:

- **int1**  (*type:* int) First integer.

- **int2**  (*type:* int) Second integer.


----

.. _deuxmots:

**deuxmots**
------------

**Inherits from:** :ref:`objet_lecture` 


Two words.

Usage:

**deuxmots** **mot_1** **mot_2**  

Parameters are:

- **mot_1**  (*type:* string) First word.

- **mot_2**  (*type:* string) Second word.


----

.. _diffusion_deriv:

**diffusion_deriv**
-------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**diffusion_deriv**  


----

.. _diffusion_negligeable:

**diffusion_negligeable**
-------------------------

**Synonyms:** negligeable

**Inherits from:** :ref:`objet_lecture` 


the diffusivity will not taken in count

Usage:

**diffusion_negligeable**  


----

.. _diffusion_option:

**diffusion_option**
--------------------

**Synonyms:** option

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**diffusion_option** **bloc_lecture**  

Parameters are:

- **bloc_lecture**  (*type:* :ref:`bloc_lecture`) not_set


----

.. _diffusion_p1ncp1b:

**diffusion_p1ncp1b**
---------------------

**Synonyms:** p1ncp1b

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **diffusion_p1ncp1b** *str*
| **Read** *str* {
| }


----

.. _diffusion_stab:

**diffusion_stab**
------------------

**Synonyms:** stab

**Inherits from:** :ref:`objet_lecture` 


keyword allowing consistent and stable calculations even in case of obtuse angle meshes.

Usage:

| **diffusion_stab** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[standard]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[info]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[new_jacobian]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nu]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nut]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nu_transp]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nut_transp]**  int
| }

Parameters are:

- **[standard]**  (*type:* int) to recover the same results as calculations made by standard laminar diffusion  operator. However, no stabilization technique is used and calculations may be unstable when  working with obtuse angle meshes (by default 0)

- **[info]**  (*type:* int) developer option to get the stabilizing ratio (by default 0)

- **[new_jacobian]**  (*type:* int) when implicit time schemes are used, this option defines a new jacobian that  may be more suitable to get stationary solutions (by default 0)

- **[nu]**  (*type:* int) (respectively nut 1) takes the molecular viscosity (resp. eddy viscosity) into account in the velocity gradient part of the diffusion expression  (by default nu=1 and nut=1)

- **[nut]**  (*type:* int) not_set

- **[nu_transp]**  (*type:* int) (respectively nut_transp 1) takes the molecular viscosity (resp. eddy viscosity) into account in the transposed velocity gradient part of the diffusion  expression (by default nu_transp=0 and nut_transp=1)

- **[nut_transp]**  (*type:* int) not_set


----

.. _diffusion_standard:

**diffusion_standard**
----------------------

**Synonyms:** standard

**Inherits from:** :ref:`objet_lecture` 


A new keyword, intended for LES calculations, has been developed to optimise and parameterise 
each term of the diffusion operator.
Remark:NL2

1.
This class requires to define a filtering operator : see solveur_barNL2 2.
The former (original) version: diffusion { } -which omitted some of the term of the 
diffusion operator- can be recovered by using the following parameters in the new 
class :NL2 diffusion { standard grad_Ubar 0 nu 1 nut 1 nu_transp 0 nut_transp 1 filtrer_resu 
0}.

Usage:

**diffusion_standard** **[mot1]** **[bloc_diffusion_standard]**  

Parameters are:

- **[mot1]**  (*type:* string into ["defaut_bar"])  equivalent to grad_Ubar 1 nu 1 nut 1 nu_transp 1 nut_transp  1 filtrer_resu 1

- **[bloc_diffusion_standard]**  (*type:* :ref:`bloc_diffusion_standard`) not_set


----

.. _diffusion_turbulente_multiphase:

**diffusion_turbulente_multiphase**
-----------------------------------

**Synonyms:** turbulente

**Inherits from:** :ref:`objet_lecture` 


Turbulent diffusion operator for multiphase problem

Usage:

**diffusion_turbulente_multiphase** **[type]**  

Parameters are:

- **[type]**  (*type:* :ref:`type_diffusion_turbulente_multiphase_deriv`) Turbulence model for multiphase problem


----

.. _difusion_p1b:

**difusion_p1b**
----------------

**Synonyms:** p1b

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**difusion_p1b**  


----

.. _domain:

**domain**
----------

**Inherits from:** :ref:`objet_lecture` 


Class to reuse a domain.

Usage:

**domain** **domain_name**  

Parameters are:

- **domain_name**  (*type:* :ref:`domaine`) Name of domain.


----

.. _dt_impr_ustar_mean_only:

**dt_impr_ustar_mean_only**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **dt_impr_ustar_mean_only** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dt_impr**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boundaries]**  string list
| }

Parameters are:

- **dt_impr**  (*type:* double) not_set

- **[boundaries]**  (*type:* string list) not_set


----

.. _ec:

**ec**
------

**Inherits from:** :ref:`objet_lecture` 


Keyword to print total kinetic energy into the referential linked to the domain (keyword 
Ec).
In the case where the domain is moving into a Galilean referential, the keyword Ec_dans_repere_fixe 
will print total kinetic energy in the Galilean referential whereas Ec will print 
the value calculated into the moving referential linked to the domain

Usage:

| **ec** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ec]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ec_dans_repere_fixe]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode]**  double
| }

Parameters are:

- **[ec]**  (*type:* flag) not_set

- **[ec_dans_repere_fixe]**  (*type:* flag) not_set

- **[periode]**  (*type:* double) periode is the keyword to set the period of printing into the file datafile_Ec.son  or datafile_Ec_dans_repere_fixe.son.


----

.. _ecrire_fichier_xyz_valeur_param:

**ecrire_fichier_xyz_valeur_param**
-----------------------------------

**Inherits from:** :ref:`objet_lecture` 


To write the values of a field for some boundaries in a text file.

The name of the files is pb_name_field_name_time.dat

Several Ecrire_fichier_xyz_valeur keywords may be written into an equation to write 
several fields.
This kind of files may be read by Champ_don_lu or Champ_front_lu for example.

Usage:

**ecrire_fichier_xyz_valeur_param** **name** **dt_ecrire_fic** **[bords]**  

Parameters are:

- **name**  (*type:* string) Name of the field to write (Champ_Inc, Champ_Fonc or a post_processed field).

- **dt_ecrire_fic**  (*type:* double) Time period for printing in the file.

- **[bords]**  (*type:* :ref:`bords_ecrire`) to post-process only on some boundaries


----

.. _entierfloat:

**entierfloat**
---------------

**Inherits from:** :ref:`objet_lecture` 


An integer and a real.

Usage:

**entierfloat** **the_int** **the_float**  

Parameters are:

- **the_int**  (*type:* int) Integer.

- **the_float**  (*type:* double) Real.


----

.. _epsilon:

**epsilon**
-----------

**Inherits from:** :ref:`objet_lecture` 


Two points will be confused if the distance between them is less than eps.
By default, eps is set to 1e-12.
The keyword Epsilon allows an alternative value to be assigned to eps.

Usage:

**epsilon** **eps**  

Parameters are:

- **eps**  (*type:* double) New value of precision.


----

.. _floatfloat:

**floatfloat**
--------------

**Inherits from:** :ref:`objet_lecture` 


Two reals.

Usage:

**floatfloat** **a** **b**  

Parameters are:

- **a**  (*type:* double) First real.

- **b**  (*type:* double) Second real.


----

.. _fonction_champ_reprise:

**fonction_champ_reprise**
--------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**fonction_champ_reprise** **mot** **fonction**  

Parameters are:

- **mot**  (*type:* string into ["fonction"])  not_set

- **fonction**  (*type:* string list) n f1(val) f2(val) ... fn(val)] time


----

.. _form_a_nb_points:

**form_a_nb_points**
--------------------

**Inherits from:** :ref:`objet_lecture` 


The structure fonction is calculated on nb points and we should add the 2 directions 
(0:OX, 1:OY, 2:OZ) constituting the homegeneity planes.
Example for channel flows, planes parallel to the walls.

Usage:

**form_a_nb_points** **nb** **dir1** **dir2**  

Parameters are:

- **nb**  (*type:* int into [4])  Number of points.

- **dir1**  (*type:* entier(max=2)  First direction.

- **dir2**  (*type:* entier(max=2)  Second direction.


----

.. _format_file:

**format_file**
---------------

**Inherits from:** :ref:`objet_lecture` 


File formatted.

Usage:

**format_file** **[format]** **name_file**  

Parameters are:

- **[format]**  (*type:* string into ["binaire", "formatte", "xyz", "single_hdf"])  Type of file (the file format).

- **name_file**  (*type:* string) Name of file.


----

.. _format_lata_to_med:

**format_lata_to_med**
----------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**format_lata_to_med** **mot** **[format]**  

Parameters are:

- **mot**  (*type:* string into ["format_post_sup"])  not_set

- **[format]**  (*type:* string into ["lml", "lata", "lata_v2", "med"])  generated file post_med.data use format  (MED or LATA or LML keyword).


----

.. _fourfloat:

**fourfloat**
-------------

**Inherits from:** :ref:`objet_lecture` 


Four reals.

Usage:

**fourfloat** **a** **b** **c** **d**  

Parameters are:

- **a**  (*type:* double) First real.

- **b**  (*type:* double) Second real.

- **c**  (*type:* double) Third real.

- **d**  (*type:* double) Fourth real.


----

.. _info_med:

**info_med**
------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**info_med** **file_med** **domaine** **pb_post**  

Parameters are:

- **file_med**  (*type:* string) Name of the MED file.

- **domaine**  (*type:* string) Name of domain.

- **pb_post**  (*type:* :ref:`pb_post`) not_set


----

.. _internes:

**internes**
------------

**Inherits from:** :ref:`objet_lecture` 


To indicate that the block has a set of internal faces (these faces will be duplicated 
automatically by the program and will be processed in a manner similar to edge faces).

Two boundaries with the same boundary conditions may have the same name (whether 
or not they belong to the same block).

The keyword Internes (Internal) must be used to execute a calculation with plates, 
followed by the equation of the surface area covered by the plates.

Usage:

**internes** **nom** **defbord**  

Parameters are:

- **nom**  (*type:* string) Name of block side.

- **defbord**  (*type:* :ref:`defbord`) Definition of block side.


----

.. _lecture_bloc_moment_base:

**lecture_bloc_moment_base**
----------------------------

**Inherits from:** :ref:`objet_lecture` 


Auxiliary class to compute and print the moments.

Usage:

**lecture_bloc_moment_base**  


----

.. _longitudinale:

**longitudinale**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Class to define the pressure loss in the direction of the tube bundle.

Usage:

**longitudinale** **dir** **dd** **ch_a** **a** **[ch_b]** **[b]**  

Parameters are:

- **dir**  (*type:* string into ["x", "y", "z"])  Direction.

- **dd**  (*type:* double) Tube bundle hydraulic diameter value. This value is expressed in m.

- **ch_a**  (*type:* string into ["a", "cf"])  Keyword to be used to set law coefficient values for the  coefficient of regular pressure losses.

- **a**  (*type:* double) Value of a law coefficient for regular pressure losses.

- **[ch_b]**  (*type:* string into ["b"])  Keyword to be used to set law coefficient values for regular  pressure losses.

- **[b]**  (*type:* double) Value of a law coefficient for regular pressure losses.


----

.. _mailler_base:

**mailler_base**
----------------

**Inherits from:** :ref:`objet_lecture` 


Basic class to mesh.

Usage:

| **mailler_base** *str*
| **Read** *str* {
| }


----

.. _methode_loi_horaire:

**methode_loi_horaire**
-----------------------

**Synonyms:** loi_horaire

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**methode_loi_horaire** **nom_loi**  

Parameters are:

- **nom_loi**  (*type:* :ref:`loi_horaire`) not_set


----

.. _methode_transport_deriv:

**methode_transport_deriv**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


Basic class for method of transport of interface.

Usage:

**methode_transport_deriv**  


----

.. _modele_turbulence_hyd_deriv:

**modele_turbulence_hyd_deriv**
-------------------------------

**Inherits from:** :ref:`objet_lecture` 


Basic class for turbulence model for Navier-Stokes equations.

Usage:

| **modele_turbulence_hyd_deriv** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_visco_turb_pour_controle_pas_de_temps]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_visco_turb_pour_controle_pas_de_temps_parametre]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[turbulence_paroi]**  :ref:`turbulence_paroi_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_ustar]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_ustar_mean_only]**  :ref:`dt_impr_ustar_mean_only`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nut_max]**  double
| }

Parameters are:

- **[correction_visco_turb_pour_controle_pas_de_temps]**  (*type:* flag) Keyword to set a limitation to low time steps due to high values of turbulent  viscosity. The limit for turbulent viscosity is calculated so that diffusive time-step is equal  or higher than convective time-step. For a stationary flow, the correction for turbulent viscosity should apply only during  the first time steps and not when permanent state is reached. To check that, we could post process the corr_visco_turb field which is the correction  of turbulent viscosity: it should be 1. on the whole domain.

- **[correction_visco_turb_pour_controle_pas_de_temps_parametre]**  (*type:* double) Keyword to set a limitation to low time steps due to high values of turbulent  viscosity. The limit for turbulent viscosity is the ratio between diffusive time-step and convective  time-step is higher or equal to the given value [0-1]

- **[turbulence_paroi]**  (*type:* :ref:`turbulence_paroi_base`) Keyword to set the wall law.

- **[dt_impr_ustar]**  (*type:* double) This keyword is used to print the values (U +, d+, u$\star$) obtained  with the wall laws into a file named datafile_ProblemName_Ustar.face and periode refers  to the printing period, this value is expressed in seconds.

- **[dt_impr_ustar_mean_only]**  (*type:* :ref:`dt_impr_ustar_mean_only`) This keyword is used to print the mean values of u* ( obtained  with the wall laws) on each boundary, into a file named datafile_ProblemName_Ustar_mean_only.out. periode refers to the printing period, this value is expressed in seconds. If you don't use the optional keyword boundaries, all the boundaries will be considered. If you use it, you must specify nb_boundaries which is the number of boundaries on  which you want to calculate the mean values of u*, then you have to specify their  names.

- **[nut_max]**  (*type:* double) Upper limitation of turbulent viscosity (default value 1.e8).


----

.. _modele_turbulence_hyd_nul:

**modele_turbulence_hyd_nul**
-----------------------------

**Synonyms:** null

**Inherits from:** :ref:`objet_lecture` 


Nul turbulence model (turbulent viscosity = 0) which can be used with a turbulent 
problem.

Usage:

| **modele_turbulence_hyd_nul** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_visco_turb_pour_controle_pas_de_temps]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correction_visco_turb_pour_controle_pas_de_temps_parametre]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[turbulence_paroi]**  :ref:`turbulence_paroi_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_ustar]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr_ustar_mean_only]**  :ref:`dt_impr_ustar_mean_only`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nut_max]**  double
| }

Parameters are:

- **[correction_visco_turb_pour_controle_pas_de_temps]**  (*type:* flag) Keyword to set a limitation to low time steps due to high values of turbulent  viscosity. The limit for turbulent viscosity is calculated so that diffusive time-step is equal  or higher than convective time-step. For a stationary flow, the correction for turbulent viscosity should apply only during  the first time steps and not when permanent state is reached. To check that, we could post process the corr_visco_turb field which is the correction  of turbulent viscosity: it should be 1. on the whole domain.

- **[correction_visco_turb_pour_controle_pas_de_temps_parametre]**  (*type:* double) Keyword to set a limitation to low time steps due to high values of turbulent  viscosity. The limit for turbulent viscosity is the ratio between diffusive time-step and convective  time-step is higher or equal to the given value [0-1]

- **[turbulence_paroi]**  (*type:* :ref:`turbulence_paroi_base`) Keyword to set the wall law.

- **[dt_impr_ustar]**  (*type:* double) This keyword is used to print the values (U +, d+, u$\star$) obtained  with the wall laws into a file named datafile_ProblemName_Ustar.face and periode refers  to the printing period, this value is expressed in seconds.

- **[dt_impr_ustar_mean_only]**  (*type:* :ref:`dt_impr_ustar_mean_only`) This keyword is used to print the mean values of u* ( obtained  with the wall laws) on each boundary, into a file named datafile_ProblemName_Ustar_mean_only.out. periode refers to the printing period, this value is expressed in seconds. If you don't use the optional keyword boundaries, all the boundaries will be considered. If you use it, you must specify nb_boundaries which is the number of boundaries on  which you want to calculate the mean values of u*, then you have to specify their  names.

- **[nut_max]**  (*type:* double) Upper limitation of turbulent viscosity (default value 1.e8).


----

.. _nom_postraitement:

**nom_postraitement**
---------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**nom_postraitement** **nom** **post**  

Parameters are:

- **nom**  (*type:* string) Name of the post-processing.

- **post**  (*type:* :ref:`postraitement_base`) the post


----

.. _numero_elem_sur_maitre:

**numero_elem_sur_maitre**
--------------------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define a probe at the special element.
Useful for min/max sonde.

Usage:

**numero_elem_sur_maitre** **numero**  

Parameters are:

- **numero**  (*type:* int) element number


----

.. _objet_lecture:

**objet_lecture**
-----------------


Auxiliary class for reading.

Usage:

| **objet_lecture** *str*
| **Read** *str* {
| }


----

.. _op_implicite:

**op_implicite**
----------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**op_implicite** **implicite** **mot** **solveur**  

Parameters are:

- **implicite**  (*type:* string into ["implicite"])  not_set

- **mot**  (*type:* string into ["solveur"])  not_set

- **solveur**  (*type:* :ref:`solveur_sys_base`) not_set


----

.. _parametre_diffusion_implicite:

**parametre_diffusion_implicite**
---------------------------------

**Inherits from:** :ref:`objet_lecture` 


To specify additional parameters for the equation when using impliciting diffusion

Usage:

| **parametre_diffusion_implicite** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[crank]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[preconditionnement_diag]**  int into [0,1]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| }

Parameters are:

- **[crank]**  (*type:* int into [0,1])  Use (1) or not (0, default) a Crank Nicholson method for the  diffusion implicitation algorithm. Setting crank to 1 increases the order of the algorithm from 1 to 2.

- **[preconditionnement_diag]**  (*type:* int into [0,1])  The CG used to solve the implicitation of the equation diffusion  operator is not preconditioned by default. If this option is set to 1, a diagonal preconditionning is used. Warning: this option is not necessarily more efficient, depending on the treated  case.

- **[niter_max_diffusion_implicite]**  (*type:* int) Change the maximum number of iterations for the CG (Conjugate Gradient) algorithm  when solving the diffusion implicitation of the equation.

- **[seuil_diffusion_implicite]**  (*type:* double) Change the threshold convergence value used by default for the CG resolution  for the diffusion implicitation of this equation.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Conjugate Gradient) to  solve the linear system.


----

.. _parametre_equation_base:

**parametre_equation_base**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


Basic class for parametre_equation

Usage:

| **parametre_equation_base** *str*
| **Read** *str* {
| }


----

.. _parametre_implicite:

**parametre_implicite**
-----------------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to change for this equation only the parameter of the implicit scheme used 
to solve the problem.

Usage:

| **parametre_implicite** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resolution_explicite]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_non_resolue]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[equation_frequence_resolue]**  string
| }

Parameters are:

- **[seuil_convergence_implicite]**  (*type:* double) Keyword to change for this equation only the value of seuil_convergence_implicite  used in the implicit scheme.

- **[seuil_convergence_solveur]**  (*type:* double) Keyword to change for this equation only the value of seuil_convergence_solveur  used in the implicit scheme

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Keyword to change for this equation only the solver used in the  implicit scheme

- **[resolution_explicite]**  (*type:* flag) To solve explicitly the equation whereas the scheme is an implicit scheme.

- **[equation_non_resolue]**  (*type:* flag) Keyword to specify that the equation is not solved.

- **[equation_frequence_resolue]**  (*type:* string) Keyword to specify that the equation is solved only every n time steps (n  is an integer or given by a time-dependent function f(t)).


----

.. _pave:

**pave**
--------

**Inherits from:** :ref:`objet_lecture` 


Class to create a pave (block) with boundaries.

Usage:

**pave** **name** **bloc** **list_bord**  

Parameters are:

- **name**  (*type:* string) Name of the pave (block).

- **bloc**  (*type:* :ref:`bloc_pave`) Definition of the pave (block).

- **list_bord**  (*type:* :ref:`list_bord`) Domain boundaries definition.


----

.. _penalisation_l2_ftd_lec:

**penalisation_l2_ftd_lec**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**penalisation_l2_ftd_lec** **[postraiter_gradient_pression_sans_masse]** **[correction_matrice_projection_initiale]** **[correction_calcul_pression_initiale]** **[correction_vitesse_projection_initiale]** **[correction_matrice_pression]** **[matrice_pression_penalisee_h1]** **[correction_vitesse_modifie]** **[correction_pression_modifie]** **[gradient_pression_qdm_modifie]** **bord** **val**  

Parameters are:

- **[postraiter_gradient_pression_sans_masse]**  (*type:* int) (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing

- **[correction_matrice_projection_initiale]**  (*type:* int) (IBM advanced) fix matrix of initial projection for PDF

- **[correction_calcul_pression_initiale]**  (*type:* int) (IBM advanced) fix initial pressure computation for PDF

- **[correction_vitesse_projection_initiale]**  (*type:* int) (IBM advanced) fix initial velocity computation for PDF

- **[correction_matrice_pression]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[matrice_pression_penalisee_h1]**  (*type:* int) (IBM advanced) fix pressure matrix for PDF

- **[correction_vitesse_modifie]**  (*type:* int) (IBM advanced) fix velocity for PDF

- **[correction_pression_modifie]**  (*type:* int) (IBM advanced) fix pressure for PDF

- **[gradient_pression_qdm_modifie]**  (*type:* int) (IBM advanced) fix pressure gradient

- **bord**  (*type:* string) not_set

- **val**  (*type:* list) not_set


----

.. _plan:

**plan**
--------

**Inherits from:** :ref:`objet_lecture` 


Keyword to set the number of probe layout points.
The file format is type .lml

Usage:

**plan** **nbr** **nbr2** **point_deb** **point_fin** **point_fin_2**  

Parameters are:

- **nbr**  (*type:* int) Number of probes in the first direction.

- **nbr2**  (*type:* int) Number of probes in the second direction.

- **point_deb**  (*type:* :ref:`un_point`) First point defining the angle. This angle should be positive.

- **point_fin**  (*type:* :ref:`un_point`) Second point defining the angle. This angle should be positive.

- **point_fin_2**  (*type:* :ref:`un_point`) Third point defining the angle. This angle should be positive.


----

.. _point:

**point**
---------

**Inherits from:** :ref:`objet_lecture` 


Point as class-daughter of Points.

Usage:

**point** **points**  

Parameters are:

- **points**  (*type:* :ref:`listpoints`) Probe points.


----

.. _points:

**points**
----------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define the number of probe points.
The file is arranged in columns.

Usage:

**points** **points**  

Parameters are:

- **points**  (*type:* :ref:`listpoints`) Probe points.


----

.. _position_like:

**position_like**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define a probe at the same position of another probe named autre_sonde.

Usage:

**position_like** **autre_sonde**  

Parameters are:

- **autre_sonde**  (*type:* string) Name of the other probe.


----

.. _postraitement:

**postraitement**
-----------------

**Synonyms:** post_processing

**Inherits from:** :ref:`objet_lecture` 


An object of post-processing (without name).

Usage:

| **postraitement** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[format]**  string into ["lml", "lata", "single_lata", "lata_v2", "med", "med_major"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaine]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_domaine | sous_zone]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[parallele]**  string into ["simple", "multiple", "mpi-io"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[definition_champs]**  :ref:`definition_champs`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[definition_champs_fichier | definition_champs_file]**  :ref:`definition_champs_fichier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes | probes]**  :ref:`sondes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes_mobiles | mobile_probes]**  :ref:`sondes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sondes_fichier | probes_file]**  :ref:`sondes_fichier`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[deprecatedkeepduplicatedprobes]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[champs | fields]**  :ref:`champs_posts`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[statistiques]**  :ref:`stats_posts`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[statistiques_en_serie]**  :ref:`stats_serie_posts`
| }

Parameters are:

- **[fichier]**  (*type:* string) Name of file.

- **[format]**  (*type:* string into ["lml", "lata", "single_lata", "lata_v2", "med", "med_major"])  This optional  parameter specifies the format of the output file. The basename used for the output file is the basename of the data file. For the fmt parameter, choices are lml or lata. A short description of each format can be found below. The default value is lml. single_lata is not compatible with 64 bits integer version.

- **[domaine]**  (*type:* string) This optional parameter specifies the domain on which the data should be  interpolated before it is written in the output file. The default is to write the data on the domain of the current problem (no interpolation).

- **[sous_domaine | sous_zone]**  (*type:* string) This optional parameter specifies the sub_domaine on which the data should  be interpolated before it is written in the output file. It is only available for sequential computation.

- **[parallele]**  (*type:* string into ["simple", "multiple", "mpi-io"])  Select simple (single file, sequential  write), multiple (several files, parallel write), or mpi-io (single file, parallel  write) for LATA format

- **[definition_champs]**  (*type:* :ref:`definition_champs`) Keyword to create new or more complex field for advanced postprocessing.

- **[definition_champs_fichier | definition_champs_file]**  (*type:* :ref:`definition_champs_fichier`) Definition_champs read from file.

- **[sondes | probes]**  (*type:* :ref:`sondes`) Probe.

- **[sondes_mobiles | mobile_probes]**  (*type:* :ref:`sondes`) Mobile probes useful for ALE, their positions will be updated in the mesh.

- **[sondes_fichier | probes_file]**  (*type:* :ref:`sondes_fichier`) Probe read in a file.

- **[deprecatedkeepduplicatedprobes]**  (*type:* int) Flag to not remove duplicated probes in .son files (1: keep duplicate probes,  0: remove duplicate probes)

- **[champs | fields]**  (*type:* :ref:`champs_posts`) Field's write mode.

- **[statistiques]**  (*type:* :ref:`stats_posts`) Statistics between two points fixed : start of integration time and  end of integration time.

- **[statistiques_en_serie]**  (*type:* :ref:`stats_serie_posts`) Statistics between two points not fixed : on period of integration.


----

.. _postraitement_base:

**postraitement_base**
----------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **postraitement_base** *str*
| **Read** *str* {
| }


----

.. _raccord:

**raccord**
-----------

**Inherits from:** :ref:`objet_lecture` 


The block side is in contact with the block of another domain (case of two coupled 
problems).

Usage:

**raccord** **type1** **type2** **nom** **defbord**  

Parameters are:

- **type1**  (*type:* string into ["local", "distant"])  Contact type.

- **type2**  (*type:* string into ["homogene"])  Contact type.

- **nom**  (*type:* string) Name of block side.

- **defbord**  (*type:* :ref:`defbord`) Definition of block side.


----

.. _radius:

**radius**
----------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**radius** **nbr** **point_deb** **radius** **teta1** **teta2**  

Parameters are:

- **nbr**  (*type:* int) Number of probe points of the segment, evenly distributed.

- **point_deb**  (*type:* :ref:`un_point`) First outer probe segment point.

- **radius**  (*type:* double) not_set

- **teta1**  (*type:* double) not_set

- **teta2**  (*type:* double) not_set


----

.. _reaction:

**reaction**
------------

**Inherits from:** :ref:`objet_lecture` 


Keyword to describe reaction:

w =K pow(T,beta) exp(-Ea/( R T)) $\Pi$ pow(Reactif_i,activitivity_i).

If K_inv >0,

w= K pow(T,beta) exp(-Ea/( R T)) ( $\Pi$ pow(Reactif_i,activitivity_i) - Kinv/exp(-c_r_Ea/(R 
T)) $\Pi$ pow(Produit_i,activitivity_i ))

Usage:

| **reaction** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **reactifs**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **produits**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constante_taux_reaction]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coefficients_activites]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **enthalpie_reaction**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **energie_activation**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **exposant_beta**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[contre_reaction]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[contre_energie_activation]**  double
| }

Parameters are:

- **reactifs**  (*type:* string) LHS of equation (ex CH4+2*O2)

- **produits**  (*type:* string) RHS of equation (ex CO2+2*H20)

- **[constante_taux_reaction]**  (*type:* double) constante of cinetic K

- **[coefficients_activites]**  (*type:* :ref:`bloc_lecture`) coefficients od ativity (exemple { CH4 1 O2 2 })

- **enthalpie_reaction**  (*type:* double) DH

- **energie_activation**  (*type:* double) Ea

- **exposant_beta**  (*type:* double) Beta

- **[contre_reaction]**  (*type:* double) K_inv

- **[contre_energie_activation]**  (*type:* double) c_r_Ea


----

.. _remove_elem_bloc:

**remove_elem_bloc**
--------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **remove_elem_bloc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fonction]**  string
| }

Parameters are:

- **[liste]**  (*type:* int list) not_set

- **[fonction]**  (*type:* string) not_set


----

.. _segment:

**segment**
-----------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define the number of probe segment points.
The file is arranged in columns.

Usage:

**segment** **nbr** **point_deb** **point_fin**  

Parameters are:

- **nbr**  (*type:* int) Number of probe points of the segment, evenly distributed.

- **point_deb**  (*type:* :ref:`un_point`) First outer probe segment point.

- **point_fin**  (*type:* :ref:`un_point`) Second outer probe segment point.


----

.. _segmentfacesx:

**segmentfacesx**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Segment probe where points are moved to the nearest x faces

Usage:

**segmentfacesx** **nbr** **point_deb** **point_fin**  

Parameters are:

- **nbr**  (*type:* int) Number of probe points of the segment, evenly distributed.

- **point_deb**  (*type:* :ref:`un_point`) First outer probe segment point.

- **point_fin**  (*type:* :ref:`un_point`) Second outer probe segment point.


----

.. _segmentfacesy:

**segmentfacesy**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Segment probe where points are moved to the nearest y faces

Usage:

**segmentfacesy** **nbr** **point_deb** **point_fin**  

Parameters are:

- **nbr**  (*type:* int) Number of probe points of the segment, evenly distributed.

- **point_deb**  (*type:* :ref:`un_point`) First outer probe segment point.

- **point_fin**  (*type:* :ref:`un_point`) Second outer probe segment point.


----

.. _segmentfacesz:

**segmentfacesz**
-----------------

**Inherits from:** :ref:`objet_lecture` 


Segment probe where points are moved to the nearest z faces

Usage:

**segmentfacesz** **nbr** **point_deb** **point_fin**  

Parameters are:

- **nbr**  (*type:* int) Number of probe points of the segment, evenly distributed.

- **point_deb**  (*type:* :ref:`un_point`) First outer probe segment point.

- **point_fin**  (*type:* :ref:`un_point`) Second outer probe segment point.


----

.. _segmentpoints:

**segmentpoints**
-----------------

**Inherits from:** :ref:`objet_lecture` 


This keyword is used to define a probe segment from specifics points.
The nom_champ field is sampled at ns specifics points.

Usage:

**segmentpoints** **points**  

Parameters are:

- **points**  (*type:* :ref:`listpoints`) Probe points.


----

.. _sonde:

**sonde**
---------

**Inherits from:** :ref:`objet_lecture` 


Keyword is used to define the probes.
Observations: the probe coordinates should be given in Cartesian coordinates (X, 
Y, Z), including axisymmetric.

Usage:

**sonde** **nom_sonde** **[special]** **nom_inco** **mperiode** **prd** **type**  

Parameters are:

- **nom_sonde**  (*type:* string) Name of the file in which the values taken over time will be saved. The complete file name is nom_sonde.son.

- **[special]**  (*type:* string into ["grav", "som", "nodes", "chsom", "gravcl"])  Option to change the positions  of the probes. Several options are available:  grav : each probe is moved to the nearest cell center of the mesh;  som : each probe is moved to the nearest vertex of the mesh  nodes : each probe is moved to the nearest face center of the mesh;  chsom : only available for P1NC sampled field. The values of the probes are calculated according to P1-Conform corresponding field.  gravcl : Extend to the domain face boundary a cell-located segment probe in order  to have the boundary condition for the field. For this type the extreme probe point has to be on the face center of gravity.

- **nom_inco**  (*type:* string) Name of the sampled field.

- **mperiode**  (*type:* string into ["periode"])  Keyword to set the sampled field measurement frequency.

- **prd**  (*type:* double) Period value. Every prd seconds, the field value calculated at the previous time step is written  to the nom_sonde.son file.

- **type**  (*type:* :ref:`sonde_base`) Type of probe.


----

.. _sonde_base:

**sonde_base**
--------------

**Inherits from:** :ref:`objet_lecture` 


Basic probe.
Probes refer to sensors that allow a value or several points of the domain to be 
monitored over time.
The probes may be a set of points defined one by one (keyword Points) or a set of 
points evenly distributed over a straight segment (keyword Segment) or arranged according 
to a layout (keyword Plan) or according to a parallelepiped (keyword Volume).
The fields allow all the values of a physical value on the domain to be known at 
several moments in time.

Usage:

**sonde_base**  


----

.. _sonde_tble:

**sonde_tble**
--------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**sonde_tble** **name** **point**  

Parameters are:

- **name**  (*type:* string) not_set

- **point**  (*type:* :ref:`un_point`) not_set


----

.. _sondes_fichier:

**sondes_fichier**
------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **sondes_fichier** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fichier | file**  string
| }

Parameters are:

- **fichier | file**  (*type:* string) name of file


----

.. _sous_zone_valeur:

**sous_zone_valeur**
--------------------

**Inherits from:** :ref:`objet_lecture` 


Two words.

Usage:

**sous_zone_valeur** **sous_zone** **valeur**  

Parameters are:

- **sous_zone**  (*type:* :ref:`sous_zone`) sous zone

- **valeur**  (*type:* double) value


----

.. _spec_pdcr_base:

**spec_pdcr_base**
------------------

**Inherits from:** :ref:`objet_lecture` 


Class to read the source term modelling the presence of a bundle of tubes in a flow.
Cf=A Re-B.

Usage:

**spec_pdcr_base** **ch_a** **a** **[ch_b]** **[b]**  

Parameters are:

- **ch_a**  (*type:* string into ["a", "cf"])  Keyword to be used to set law coefficient values for the  coefficient of regular pressure losses.

- **a**  (*type:* double) Value of a law coefficient for regular pressure losses.

- **[ch_b]**  (*type:* string into ["b"])  Keyword to be used to set law coefficient values for regular  pressure losses.

- **[b]**  (*type:* double) Value of a law coefficient for regular pressure losses.


----

.. _stat_post_correlation:

**stat_post_correlation**
-------------------------

**Synonyms:** champ_post_statistiques_correlation, correlation

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_correlation** **first_field** **second_field** **[localisation]**  

Parameters are:

- **first_field**  (*type:* string) not_set

- **second_field**  (*type:* string) not_set

- **[localisation]**  (*type:* string into ["elem", "som", "faces"])  Localisation of post-processed field value


----

.. _stat_post_deriv:

**stat_post_deriv**
-------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_deriv**  


----

.. _stat_post_ecart_type:

**stat_post_ecart_type**
------------------------

**Synonyms:** ecart_type, champ_post_statistiques_ecart_type

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_ecart_type** **field** **[localisation]**  

Parameters are:

- **field**  (*type:* string) not_set

- **[localisation]**  (*type:* string into ["elem", "som", "faces"])  Localisation of post-processed field value


----

.. _stat_post_moyenne:

**stat_post_moyenne**
---------------------

**Synonyms:** champ_post_statistiques_moyenne, moyenne

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_moyenne** **field** **[localisation]**  

Parameters are:

- **field**  (*type:* string) not_set

- **[localisation]**  (*type:* string into ["elem", "som", "faces"])  Localisation of post-processed field value


----

.. _stat_post_t_deb:

**stat_post_t_deb**
-------------------

**Synonyms:** t_deb

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_t_deb** **val**  

Parameters are:

- **val**  (*type:* double) not_set


----

.. _stat_post_t_fin:

**stat_post_t_fin**
-------------------

**Synonyms:** t_fin

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**stat_post_t_fin** **val**  

Parameters are:

- **val**  (*type:* double) not_set


----

.. _stats_posts:

**stats_posts**
---------------

**Inherits from:** :ref:`objet_lecture` 


Field's write mode.
\input{{statistiques}}

Usage:

**stats_posts** **mot** **period** **champs | fields**  

Parameters are:

- **mot**  (*type:* string into ["dt_post", "nb_pas_dt_post"])  Keyword to set the kind of the field's  write frequency. Either a time period or a time step period.

- **period**  (*type:* string) Value of the period which can be like (2.*t).

- **champs | fields**  (*type:* :ref:`list_stat_post`) Post-processed fields.


----

.. _stats_serie_posts:

**stats_serie_posts**
---------------------

**Inherits from:** :ref:`objet_lecture` 


Post-processing for statistics.
\input{{statistiquesseries}}

Usage:

**stats_serie_posts** **mot** **dt_integr** **stat**  

Parameters are:

- **mot**  (*type:* string into ["dt_integr"])  Keyword is used to set the statistics period of integration  and write period.

- **dt_integr**  (*type:* double) Average on dt_integr time interval is post-processed every dt_integr seconds.

- **stat**  (*type:* :ref:`list_stat_post`) not_set


----

.. _temperature:

**temperature**
---------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **temperature** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **bord**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  int
| }

Parameters are:

- **bord**  (*type:* string) not_set

- **direction**  (*type:* int) not_set


----

.. _thi:

**thi**
-------

**Inherits from:** :ref:`objet_lecture` 


Keyword for a THI (Homogeneous Isotropic Turbulence) calculation.

Usage:

| **thi** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **init_ec**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[val_ec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facon_init]**  int into ["0", "1"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[calc_spectre]**  int into ["0", "1"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_calc_spectre]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[spectre_3d]**  int into ["0", "1"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[spectre_1d]**  int into ["0", "1"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conservation_ec]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[longueur_boite]**  double
| }

Parameters are:

- **init_ec**  (*type:* int) Keyword to renormalize initial velocity so that kinetic energy equals to  the value given by keyword val_Ec.

- **[val_ec]**  (*type:* double) Keyword to impose a value for kinetic energy by velocity renormalizated  if init_Ec value is 1.

- **[facon_init]**  (*type:* int into ["0", "1"])  Keyword to specify how kinetic energy is computed (0 or 1).

- **[calc_spectre]**  (*type:* int into ["0", "1"])  Calculate or not the spectrum of kinetic energy.  Files called Sorties_THI are written with inside four columns :  time:t global_kinetic_energy:Ec enstrophy:D skewness:S  If calc_spectre is set to 1, a file Sorties_THI2_2 is written with three columns  :  time:t kinetic_energy_at_kc=32 enstrophy_at_kc=32  If calc_spectre is set to 1, a file spectre_xxxxx is written with two columns at  each time xxxxx :  frequency:k energy:E(k).

- **[periode_calc_spectre]**  (*type:* double) Period for calculating spectrum of kinetic energy

- **[spectre_3d]**  (*type:* int into ["0", "1"])  Calculate or not the 3D spectrum

- **[spectre_1d]**  (*type:* int into ["0", "1"])  Calculate or not the 1D spectrum

- **[conservation_ec]**  (*type:* flag) If set to 1, velocity field will be changed as to have a constant kinetic energy  (default 0)

- **[longueur_boite]**  (*type:* double) Length of the calculation domain


----

.. _traitement_particulier:

**traitement_particulier**
--------------------------

**Inherits from:** :ref:`objet_lecture` 


Auxiliary class to post-process particular values.

Usage:

**traitement_particulier** **aco** **trait_part** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **trait_part**  (*type:* :ref:`traitement_particulier_base`) Type of traitement_particulier.

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _traitement_particulier_base:

**traitement_particulier_base**
-------------------------------

**Inherits from:** :ref:`objet_lecture` 


Basic class to post-process particular values.

Usage:

| **traitement_particulier_base** *str*
| **Read** *str* {
| }


----

.. _transversale:

**transversale**
----------------

**Inherits from:** :ref:`objet_lecture` 


Class to define the pressure loss in the direction perpendicular to the tube bundle.

Usage:

**transversale** **dir** **dd** **chaine_d** **d** **ch_a** **a** **[ch_b]** **[b]**  

Parameters are:

- **dir**  (*type:* string into ["x", "y", "z"])  Direction.

- **dd**  (*type:* double) Value of the tube bundle step.

- **chaine_d**  (*type:* string into ["d"])  Keyword to be used to set the value of the tube external diameter.

- **d**  (*type:* double) Value of the tube external diameter.

- **ch_a**  (*type:* string into ["a", "cf"])  Keyword to be used to set law coefficient values for the  coefficient of regular pressure losses.

- **a**  (*type:* double) Value of a law coefficient for regular pressure losses.

- **[ch_b]**  (*type:* string into ["b"])  Keyword to be used to set law coefficient values for regular  pressure losses.

- **[b]**  (*type:* double) Value of a law coefficient for regular pressure losses.


----

.. _troisf:

**troisf**
----------

**Inherits from:** :ref:`objet_lecture` 


Auxiliary class to extrude.

Usage:

**troisf** **lx** **ly** **lz**  

Parameters are:

- **lx**  (*type:* double) X direction of the extrude operation.

- **ly**  (*type:* double) Y direction of the extrude operation.

- **lz**  (*type:* double) Z direction of the extrude operation.


----

.. _troismots:

**troismots**
-------------

**Inherits from:** :ref:`objet_lecture` 


Three words.

Usage:

**troismots** **mot_1** **mot_2** **mot_3**  

Parameters are:

- **mot_1**  (*type:* string) First word.

- **mot_2**  (*type:* string) Snd word.

- **mot_3**  (*type:* string) Third word.


----

.. _twofloat:

**twofloat**
------------

**Inherits from:** :ref:`objet_lecture` 


two reals.

Usage:

**twofloat** **a** **b**  

Parameters are:

- **a**  (*type:* double) First real.

- **b**  (*type:* double) Second real.


----

.. _type_diffusion_turbulente_multiphase_deriv:

**type_diffusion_turbulente_multiphase_deriv**
----------------------------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **type_diffusion_turbulente_multiphase_deriv** *str*
| **Read** *str* {
| }


----

.. _type_diffusion_turbulente_multiphase_l_melange:

**type_diffusion_turbulente_multiphase_l_melange**
--------------------------------------------------

**Synonyms:** l_melange

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **type_diffusion_turbulente_multiphase_l_melange** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[l_melange]**  double
| }

Parameters are:

- **[l_melange]**  (*type:* double) not_set


----

.. _type_diffusion_turbulente_multiphase_sgdh:

**type_diffusion_turbulente_multiphase_sgdh**
---------------------------------------------

**Synonyms:** sgdh

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **type_diffusion_turbulente_multiphase_sgdh** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pr_t | prandtl_turbulent]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sigma | sigma_turbulent]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_alpha]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gas_turb]**  flag
| }

Parameters are:

- **[pr_t | prandtl_turbulent]**  (*type:* double) not_set

- **[sigma | sigma_turbulent]**  (*type:* double) not_set

- **[no_alpha]**  (*type:* flag) not_set

- **[gas_turb]**  (*type:* flag) not_set


----

.. _type_perte_charge_deriv:

**type_perte_charge_deriv**
---------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **type_perte_charge_deriv** *str*
| **Read** *str* {
| }


----

.. _type_perte_charge_dp:

**type_perte_charge_dp**
------------------------

**Synonyms:** dp

**Inherits from:** :ref:`objet_lecture` 


DP field should have 3 components defining dp, dDP/dQ, Q0

Usage:

**type_perte_charge_dp** **dp_field**  

Parameters are:

- **dp_field**  (*type:* :ref:`field_base`) the parameters of the previous formula (DP = dp + dDP/dQ * (Q - Q0)):  uniform_field 3 dp dDP/dQ Q0 where Q0 is a mass flow rate (kg/s).


----

.. _type_perte_charge_dp_regul:

**type_perte_charge_dp_regul**
------------------------------

**Synonyms:** dp_regul

**Inherits from:** :ref:`objet_lecture` 


Keyword used to regulate the DP value in order to match a target flow rate.
Syntax : dp_regul { DP0 d deb d eps e }

Usage:

| **type_perte_charge_dp_regul** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dp0**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **deb**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **eps**  string
| }

Parameters are:

- **dp0**  (*type:* double) initial value of DP

- **deb**  (*type:* string) target flow rate in kg/s

- **eps**  (*type:* string) strength of the regulation (low values might be slow to find the target flow  rate, high values might oscillate around the target value)


----

.. _type_postraitement_ft_lata:

**type_postraitement_ft_lata**
------------------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**type_postraitement_ft_lata** **type** **nom** **bloc**  

Parameters are:

- **type**  (*type:* string into ["postraitement_ft_lata", "postraitement_lata"])  not_set

- **nom**  (*type:* string) Name of the post-processing.

- **bloc**  (*type:* string) not_set


----

.. _type_un_post:

**type_un_post**
----------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

**type_un_post** **type** **post**  

Parameters are:

- **type**  (*type:* string into ["postraitement", "post_processing"])  not_set

- **post**  (*type:* :ref:`un_postraitement`) not_set


----

.. _un_pb:

**un_pb**
---------

**Inherits from:** :ref:`objet_lecture` 


pour les groupes

Usage:

**un_pb** **mot**  

Parameters are:

- **mot**  (*type:* :ref:`pb_base`) the string


----

.. _un_point:

**un_point**
------------

**Inherits from:** :ref:`objet_lecture` 


A point.

Usage:

**un_point** **pos**  

Parameters are:

- **pos**  (*type:* double list) Point coordinates.


----

.. _un_postraitement:

**un_postraitement**
--------------------

**Inherits from:** :ref:`objet_lecture` 


An object of post-processing (with name).

Usage:

**un_postraitement** **nom** **post**  

Parameters are:

- **nom**  (*type:* string) Name of the post-processing.

- **post**  (*type:* :ref:`corps_postraitement`) Definition of the post-processing.


----

.. _un_postraitement_spec:

**un_postraitement_spec**
-------------------------

**Inherits from:** :ref:`objet_lecture` 


An object of post-processing (with type +name).

Usage:

**un_postraitement_spec** **[type_un_post]** **[type_postraitement_ft_lata]**  

Parameters are:

- **[type_un_post]**  (*type:* :ref:`type_un_post`) not_set

- **[type_postraitement_ft_lata]**  (*type:* :ref:`type_postraitement_ft_lata`) not_set


----

.. _verifiercoin_bloc:

**verifiercoin_bloc**
---------------------

**Inherits from:** :ref:`objet_lecture` 


not_set

Usage:

| **verifiercoin_bloc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[read_file | filename | lire_fichier]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[expert_only]**  flag
| }

Parameters are:

- **[read_file | filename | lire_fichier]**  (*type:* string) name of the *.decoupage_som file

- **[expert_only]**  (*type:* flag) to not check the mesh


----

.. _volume:

**volume**
----------

**Inherits from:** :ref:`objet_lecture` 


Keyword to define the probe volume in a parallelepiped passing through 4 points and 
the number of probes in each direction.

Usage:

**volume** **nbr** **nbr2** **nbr3** **point_deb** **point_fin** **point_fin_2** **point_fin_3**  

Parameters are:

- **nbr**  (*type:* int) Number of probes in the first direction.

- **nbr2**  (*type:* int) Number of probes in the second direction.

- **nbr3**  (*type:* int) Number of probes in the third direction.

- **point_deb**  (*type:* :ref:`un_point`) Point of origin.

- **point_fin**  (*type:* :ref:`un_point`) Point defining the first direction (from point of origin).

- **point_fin_2**  (*type:* :ref:`un_point`) Point defining the second direction (from point of origin).

- **point_fin_3**  (*type:* :ref:`un_point`) Point defining the third direction (from point of origin).


----

**Keywords derived from partitionneur_deriv**
=============================================

.. _partitionneur_deriv:

**partitionneur_deriv**
-----------------------


not_set

Usage:

| **partitionneur_deriv** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_fichier_decoupage:

**partitionneur_fichier_decoupage**
-----------------------------------

**Synonyms:** fichier_decoupage

**Inherits from:** :ref:`partitionneur_deriv` 


This algorithm reads an array of integer values on the disc, one value for each mesh 
element.
Each value is interpreted as the target part number n>=0 for this element.
The number of parts created is the highest value in the array plus one.
Empty parts can be created if some values are not present in the array.

The file format is ASCII, and contains space, tab or carriage-return separated integer 
values.
The first value is the number nb_elem of elements in the domain, followed by nb_elem 
integer values (positive or zero).

This algorithm has been designed to work together with the 'ecrire_decoupage' option.
You can generate a partition with any other algorithm, write it to disc, modify it, 
and read it again to generate the .Zone files.

Contrary to other partitioning algorithms, no correction is applied by default to 
the partition (eg.
element 0 on processor 0 and corrections for periodic boundaries).
If 'corriger_partition' is specified, these corrections are applied.

Usage:

| **partitionneur_fichier_decoupage** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fichier**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[corriger_partition]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **fichier**  (*type:* string) FILENAME

- **[corriger_partition]**  (*type:* flag) not_set

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_fichier_med:

**partitionneur_fichier_med**
-----------------------------

**Synonyms:** fichier_med

**Inherits from:** :ref:`partitionneur_deriv` 


Partitioning a domain using a MED file containing an integer field providing for each 
element the processor number on which the element should be located.

Usage:

| **partitionneur_fichier_med** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **file**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **field**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **file**  (*type:* string) file name of the MED file to load

- **field**  (*type:* string) field name of the integer field to load

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_metis:

**partitionneur_metis**
-----------------------

**Synonyms:** metis

**Inherits from:** :ref:`partitionneur_deriv` 


Metis is an external partitionning library.
It is a general algorithm that will generate a partition of the domain.

Usage:

| **partitionneur_metis** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[kmetis]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[use_weights]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **[kmetis]**  (*type:* flag) The default values are pmetis, default parameters are automatically chosen  by Metis. 'kmetis' is faster than pmetis option but the last option produces better partitioning  quality. In both cases, the partitioning quality may be slightly improved by increasing the  nb_essais option (by default N=1). It will compute N partitions and will keep the best one (smallest edge cut number). But this option is CPU expensive, taking N=10 will multiply the CPU cost of partitioning  by 10.  Experiments show that only marginal improvements can be obtained with non default  parameters.

- **[use_weights]**  (*type:* flag) If use_weights is specified, weighting of the element-element links in the  graph is used to force metis to keep opposite periodic elements on the same processor. This option can slightly improve the partitionning quality but it consumes more memory  and takes more time. It is not mandatory since a correction algorithm is always applied afterwards to  ensure a correct partitionning for periodic boundaries.

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_partition:

**partitionneur_partition**
---------------------------

**Synonyms:** decouper, partition

**Inherits from:** :ref:`partitionneur_deriv` 


This algorithm re-use the partition of the domain named DOMAINE_NAME.
It is useful to partition for example a post processing domain.
The partition should match with the calculation domain.

Usage:

| **partitionneur_partition** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **domaine**  :ref:`domaine`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **domaine**  (*type:* :ref:`domaine`) domain name

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_sous_dom:

**partitionneur_sous_dom**
--------------------------

**Synonyms:** sous_dom

**Inherits from:** :ref:`partitionneur_deriv` 


Given a global partition of a global domain, 'sous-domaine' allows to produce a conform 
partition of a sub-domain generated from the bigger one using the keyword create_domain_from_sous_domaine.
The sub-domain will be partitionned in a conform fashion with the global domain.

Usage:

| **partitionneur_sous_dom** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fichier**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fichier_ssz**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **fichier**  (*type:* string) fichier

- **fichier_ssz**  (*type:* string) fichier sous zonne

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_sous_domaines:

**partitionneur_sous_domaines**
-------------------------------

**Synonyms:** partitionneur_sous_zones

**Inherits from:** :ref:`partitionneur_deriv` 


This algorithm will create one part for each specified subdomaine/domain.
All elements contained in the first subdomaine/domain are put in the first part, 
all remaining elements contained in the second subdomaine/domain in the second part, 
etc...

If all elements of the current domain are contained in the specified subdomaines/domain, 
then N parts are created, otherwise, a supplemental part is created with the remaining 
elements.

If no subdomaine is specified, all subdomaines defined in the domain are used to 
split the mesh.

Usage:

| **partitionneur_sous_domaines** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zones]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domaines]**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **[sous_zones]**  (*type:* string list) N SUBZONE_NAME_1 SUBZONE_NAME_2 ...

- **[domaines]**  (*type:* string list) N DOMAIN_NAME_1 DOMAIN_NAME_2 ...

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_sous_zones:

**partitionneur_sous_zones**
----------------------------

**Synonyms:** sous_zones, partitionneur_sous_domaines

**Inherits from:** :ref:`partitionneur_deriv` 


This algorithm will create one part for each specified subzone.
All elements contained in the first subzone are put in the first part, all remaining 
elements contained in the second subzone in the second part, etc...

If all elements of the domain are contained in the specified subzones, then N parts 
are created, otherwise, a supplemental part is created with the remaining elements.

If no subzone is specified, all subzones defined in the domain are used to split 
the mesh.

Usage:

| **partitionneur_sous_zones** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **sous_zones**  string list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **sous_zones**  (*type:* string list) N SUBZONE_NAME_1 SUBZONE_NAME_2 ...

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_tranche:

**partitionneur_tranche**
-------------------------

**Synonyms:** tranche

**Inherits from:** :ref:`partitionneur_deriv` 


This algorithm will create a geometrical partitionning by slicing the mesh in the 
two or three axis directions, based on the geometric center of each mesh element.
nz must be given if dimension=3.
Each slice contains the same number of elements (slices don't have the same geometrical 
width, and for VDF meshes, slice boundaries are generally not flat except if the number 
of mesh elements in each direction is an exact multiple of the number of slices).
First, nx slices in the X direction are created, then each slice is split in ny slices 
in the Y direction, and finally, each part is split in nz slices in the Z direction.
The resulting number of parts is nx*ny*nz.
If one particular direction has been declared periodic, the default slicing (0, 1, 
2, ..., n-1)is replaced by (0, 1, 2, ...
n-1, 0), each of the two '0' slices having twice less elements than the other slices.

Usage:

| **partitionneur_tranche** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tranches]**  int list - size is dimension
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_parts]**  int
| }

Parameters are:

- **[tranches]**  (*type:* int list - size is dimension) Partitioned by nx in the X direction, ny in the Y direction, nz in the  Z direction. Works only for structured meshes. No warranty for unstructured meshes.

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

.. _partitionneur_union:

**partitionneur_union**
-----------------------

**Synonyms:** union

**Inherits from:** :ref:`partitionneur_deriv` 


Let several local domains be generated from a bigger one using the keyword create_domain_from_sous_domaine, 
and let their partitions be generated in the usual way.
Provided the list of partition files for each small domain, the keyword 'union' will 
partition the global domain in a conform fashion with the smaller domains.

Usage:

**partitionneur_union** **liste** **[nb_parts]**  

Parameters are:

- **liste**  (*type:* :ref:`bloc_lecture`) List of the partition files with the following syntaxe: {sous_domaine1  decoupage1 ... sous_domaineim decoupageim } where sous_domaine1 ... sous_zomeim are small domains names and decoupage1 ... decoupageim are partition files.

- **[nb_parts]**  (*type:* int) The number of non empty parts that must be generated (generally equal to  the number of processors in the parallel run).


----

**Keywords derived from pb_gen_base**
=====================================

.. _coupled_problem:

**coupled_problem**
-------------------

**Synonyms:** probleme_couple

**Inherits from:** :ref:`pb_gen_base` 


This instruction causes a probleme_couple type object to be created.
This type of object has an associated problem list, that is, the coupling of n problems 
among them may be processed.
Coupling between these problems is carried out explicitly via conditions at particular 
contact limits.
Each problem may be associated either with the Associate keyword or with the Read/groupes 
keywords.
The difference is that in the first case, the four problems exchange values then 
calculate their timestep, rather in the second case, the same strategy is used for 
all the problems listed inside one group, but the second group of problem exchange 
values with the first group of problems after the first group did its timestep.
So, the first case may then also be written like this:

Probleme_Couple pbc

Read pbc { groupes { { pb1 , pb2 , pb3 , pb4 } } }

There is a physical environment per problem (however, the same physical environment 
could be common to several problems).

Each problem is resolved in a domain.

Warning : Presently, coupling requires coincident meshes.
In case of non-coincident meshes, boundary condition 'paroi_contact' in VEF returns 
error message (see paroi_contact for correcting procedure).

Usage:

| **coupled_problem** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[groupes]**  :ref:`list_list_nom`
| }

Parameters are:

- **[groupes]**  (*type:* :ref:`list_list_nom`) { groupes { { pb1 , pb2 } , { pb3 , pb4 } } }


----

.. _pb_avec_passif:

**pb_avec_passif**
------------------

**Inherits from:** :ref:`pb_gen_base` 


Class to create a classical problem with a scalar transport equation (e.g: temperature 
or concentration) and an additional set of passive scalars (e.g: temperature or concentration) 
equations.

Usage:

| **pb_avec_passif** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_base:

**pb_base**
-----------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of equations on a domain.
A problem is defined by creating an object and assigning the problem type that the 
user wishes to resolve.
To enter values for the problem objects created, the Lire (Read) interpretor is used 
with a data block.

Usage:

| **pb_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_conduction:

**pb_conduction**
-----------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of the heat equation.

Usage:

| **pb_conduction** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solide]**  :ref:`solide`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[conduction]**  :ref:`conduction`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[solide]**  (*type:* :ref:`solide`) The medium associated with the problem.

- **[conduction]**  (*type:* :ref:`conduction`) Heat equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_gen_base:

**pb_gen_base**
---------------


Basic class for problems.

Usage:

| **pb_gen_base** *str*
| **Read** *str* {
| }


----

.. _pb_hydraulique:

**pb_hydraulique**
------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of the Navier-Stokes equations.

Usage:

| **pb_hydraulique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_standard**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **navier_stokes_standard**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_concentration:

**pb_hydraulique_concentration**
--------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/multiple constituent transport equations.

Usage:

| **pb_hydraulique_concentration** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration]**  :ref:`convection_diffusion_concentration`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_concentration]**  (*type:* :ref:`convection_diffusion_concentration`) Constituent transport vectorial equation (concentration  diffusion convection).

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_concentration_scalaires_passifs:

**pb_hydraulique_concentration_scalaires_passifs**
--------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/multiple constituent transport equations with the additional 
passive scalar equations.

Usage:

| **pb_hydraulique_concentration_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration]**  :ref:`convection_diffusion_concentration`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_concentration]**  (*type:* :ref:`convection_diffusion_concentration`) Constituent transport equations (concentration  diffusion convection).

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_concentration_turbulent:

**pb_hydraulique_concentration_turbulent**
------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence 
modelling.

Usage:

| **pb_hydraulique_concentration_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_turbulent]**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration_turbulent]**  :ref:`convection_diffusion_concentration_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_turbulent]**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[convection_diffusion_concentration_turbulent]**  (*type:* :ref:`convection_diffusion_concentration_turbulent`) Constituent transport equations (concentration  diffusion convection) as well as the associated turbulence model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_concentration_turbulent_scalaires_passifs:

**pb_hydraulique_concentration_turbulent_scalaires_passifs**
------------------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence 
modelling and with the additional passive scalar equations.

Usage:

| **pb_hydraulique_concentration_turbulent_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_turbulent]**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration_turbulent]**  :ref:`convection_diffusion_concentration_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_turbulent]**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[convection_diffusion_concentration_turbulent]**  (*type:* :ref:`convection_diffusion_concentration_turbulent`) Constituent transport equations (concentration  diffusion convection) as well as the associated turbulence model equations.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_melange_binaire_qc:

**pb_hydraulique_melange_binaire_qc**
-------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of a binary mixture problem for a quasi-compressible fluid with an iso-thermal 
condition.

Keywords for the unknowns other than pressure, velocity, fraction_massique are :

masse_volumique : density

pression : reduced pressure

pression_tot : total pressure.

Usage:

| **pb_hydraulique_melange_binaire_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_qc**  :ref:`navier_stokes_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_espece_binaire_qc**  :ref:`convection_diffusion_espece_binaire_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) The various constituants associated to the problem.

- **navier_stokes_qc**  (*type:* :ref:`navier_stokes_qc`) Navier-Stokes equation for a quasi-compressible fluid.

- **convection_diffusion_espece_binaire_qc**  (*type:* :ref:`convection_diffusion_espece_binaire_qc`) Species conservation equation for a binary  quasi-compressible fluid.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_melange_binaire_turbulent_qc:

**pb_hydraulique_melange_binaire_turbulent_qc**
-----------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of a turbulent binary mixture problem for a quasi-compressible fluid with 
an iso-thermal condition.

Usage:

| **pb_hydraulique_melange_binaire_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_turbulent_qc**  :ref:`navier_stokes_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_espece_binaire_turbulent_qc**  :ref:`convection_diffusion_espece_binaire_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **navier_stokes_turbulent_qc**  (*type:* :ref:`navier_stokes_turbulent_qc`) Navier-Stokes equation for a quasi-compressible fluid  as well as the associated turbulence model equations.

- **convection_diffusion_espece_binaire_turbulent_qc**  (*type:* :ref:`convection_diffusion_espece_binaire_turbulent_qc`) Species conservation equation for  a quasi-compressible fluid as well as the associated turbulence model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_melange_binaire_wc:

**pb_hydraulique_melange_binaire_wc**
-------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of a binary mixture problem for a weakly-compressible fluid with an iso-thermal 
condition.

Keywords for the unknowns other than pressure, velocity, fraction_massique are :

masse_volumique : density

pression : reduced pressure

pression_tot : total pressure

pression_hydro : hydro-static pressure

pression_eos : pressure used in state equation.

Usage:

| **pb_hydraulique_melange_binaire_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_weakly_compressible**  :ref:`fluide_weakly_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_wc**  :ref:`navier_stokes_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_espece_binaire_wc**  :ref:`convection_diffusion_espece_binaire_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_weakly_compressible**  (*type:* :ref:`fluide_weakly_compressible`) The fluid medium associated with the problem.

- **navier_stokes_wc**  (*type:* :ref:`navier_stokes_wc`) Navier-Stokes equation for a weakly-compressible fluid.

- **convection_diffusion_espece_binaire_wc**  (*type:* :ref:`convection_diffusion_espece_binaire_wc`) Species conservation equation for a binary  weakly-compressible fluid.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_hydraulique_turbulent:

**pb_hydraulique_turbulent**
----------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes equations with turbulence modelling.

Usage:

| **pb_hydraulique_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_turbulent**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **navier_stokes_turbulent**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_multiphase:

**pb_multiphase**
-----------------

**Inherits from:** :ref:`pb_gen_base` 


A problem that allows the resolution of N-phases with 3*N equations

Usage:

| **pb_multiphase** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu_composite]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu_musig]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correlations]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **qdm_multiphase**  :ref:`qdm_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **masse_multiphase**  :ref:`masse_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **energie_multiphase**  :ref:`energie_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[energie_cinetique_turbulente]**  :ref:`energie_cinetique_turbulente`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[echelle_temporelle_turbulente]**  :ref:`echelle_temporelle_turbulente`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[energie_cinetique_turbulente_wit]**  :ref:`energie_cinetique_turbulente_wit`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[taux_dissipation_turbulent]**  :ref:`taux_dissipation_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[milieu_composite]**  (*type:* :ref:`bloc_lecture`) The composite medium associated with the problem.

- **[milieu_musig]**  (*type:* :ref:`bloc_lecture`) The composite medium associated with the problem.

- **[correlations]**  (*type:* :ref:`bloc_lecture`) List of correlations used in specific source terms (i.e. interfacial flux, interfacial friction, ...)

- **qdm_multiphase**  (*type:* :ref:`qdm_multiphase`) Momentum conservation equation for a multi-phase problem where the  unknown is the velocity

- **masse_multiphase**  (*type:* :ref:`masse_multiphase`) Mass consevation equation for a multi-phase problem where the unknown  is the alpha (void fraction)

- **energie_multiphase**  (*type:* :ref:`energie_multiphase`) Internal energy conservation equation for a multi-phase problem  where the unknown is the temperature

- **[energie_cinetique_turbulente]**  (*type:* :ref:`energie_cinetique_turbulente`) Turbulent kinetic Energy conservation equation for  a turbulent mono/multi-phase problem (available in TrioCFD)

- **[echelle_temporelle_turbulente]**  (*type:* :ref:`echelle_temporelle_turbulente`) Turbulent Dissipation time scale equation for a turbulent  mono/multi-phase problem (available in TrioCFD)

- **[energie_cinetique_turbulente_wit]**  (*type:* :ref:`energie_cinetique_turbulente_wit`) Bubble Induced Turbulent kinetic Energy equation  for a turbulent multi-phase problem (available in TrioCFD)

- **[taux_dissipation_turbulent]**  (*type:* :ref:`taux_dissipation_turbulent`) Turbulent Dissipation frequency equation for a turbulent  mono/multi-phase problem (available in TrioCFD)

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_multiphase_hem:

**pb_multiphase_hem**
---------------------

**Synonyms:** pb_hem

**Inherits from:** :ref:`pb_gen_base` 


A problem that allows the resolution of 2-phases mechanicaly and thermally coupled 
with 3 equations

Usage:

| **pb_multiphase_hem** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu_composite]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu_musig]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[correlations]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **qdm_multiphase**  :ref:`qdm_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **masse_multiphase**  :ref:`masse_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **energie_multiphase**  :ref:`energie_multiphase`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[energie_cinetique_turbulente]**  :ref:`energie_cinetique_turbulente`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[echelle_temporelle_turbulente]**  :ref:`echelle_temporelle_turbulente`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[energie_cinetique_turbulente_wit]**  :ref:`energie_cinetique_turbulente_wit`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[taux_dissipation_turbulent]**  :ref:`taux_dissipation_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[milieu_composite]**  (*type:* :ref:`bloc_lecture`) The composite medium associated with the problem.

- **[milieu_musig]**  (*type:* :ref:`bloc_lecture`) The composite medium associated with the problem.

- **[correlations]**  (*type:* :ref:`bloc_lecture`) List of correlations used in specific source terms (i.e. interfacial flux, interfacial friction, ...)

- **qdm_multiphase**  (*type:* :ref:`qdm_multiphase`) Momentum conservation equation for a multi-phase problem where the  unknown is the velocity

- **masse_multiphase**  (*type:* :ref:`masse_multiphase`) Mass consevation equation for a multi-phase problem where the unknown  is the alpha (void fraction)

- **energie_multiphase**  (*type:* :ref:`energie_multiphase`) Internal energy conservation equation for a multi-phase problem  where the unknown is the temperature

- **[energie_cinetique_turbulente]**  (*type:* :ref:`energie_cinetique_turbulente`) Turbulent kinetic Energy conservation equation for  a turbulent mono/multi-phase problem (available in TrioCFD)

- **[echelle_temporelle_turbulente]**  (*type:* :ref:`echelle_temporelle_turbulente`) Turbulent Dissipation time scale equation for a turbulent  mono/multi-phase problem (available in TrioCFD)

- **[energie_cinetique_turbulente_wit]**  (*type:* :ref:`energie_cinetique_turbulente_wit`) Bubble Induced Turbulent kinetic Energy equation  for a turbulent multi-phase problem (available in TrioCFD)

- **[taux_dissipation_turbulent]**  (*type:* :ref:`taux_dissipation_turbulent`) Turbulent Dissipation frequency equation for a turbulent  mono/multi-phase problem (available in TrioCFD)

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_post:

**pb_post**
-----------

**Inherits from:** :ref:`pb_gen_base` 


not_set

Usage:

| **pb_post** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique:

**pb_thermohydraulique**
------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermohydraulic problem.

Usage:

| **pb_thermohydraulique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fluide_incompressible]**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fluide_ostwald]**  :ref:`fluide_ostwald`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fluide_sodium_liquide]**  :ref:`fluide_sodium_liquide`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fluide_sodium_gaz]**  :ref:`fluide_sodium_gaz`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature]**  :ref:`convection_diffusion_temperature`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[fluide_incompressible]**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem (only one possibility).

- **[fluide_ostwald]**  (*type:* :ref:`fluide_ostwald`) The fluid medium associated with the problem (only one possibility).

- **[fluide_sodium_liquide]**  (*type:* :ref:`fluide_sodium_liquide`) The fluid medium associated with the problem (only one possibility).

- **[fluide_sodium_gaz]**  (*type:* :ref:`fluide_sodium_gaz`) The fluid medium associated with the problem (only one possibility).

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_temperature]**  (*type:* :ref:`convection_diffusion_temperature`) Energy equation (temperature diffusion convection).

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_concentration:

**pb_thermohydraulique_concentration**
--------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/energy/multiple constituent transport equations.

Usage:

| **pb_thermohydraulique_concentration** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration]**  :ref:`convection_diffusion_concentration`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature]**  :ref:`convection_diffusion_temperature`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_concentration]**  (*type:* :ref:`convection_diffusion_concentration`) Constituent transport equations (concentration  diffusion convection).

- **[convection_diffusion_temperature]**  (*type:* :ref:`convection_diffusion_temperature`) Energy equation (temperature diffusion convection).

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_concentration_scalaires_passifs:

**pb_thermohydraulique_concentration_scalaires_passifs**
--------------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
the additional passive scalar equations.

Usage:

| **pb_thermohydraulique_concentration_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration]**  :ref:`convection_diffusion_concentration`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature]**  :ref:`convection_diffusion_temperature`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_concentration]**  (*type:* :ref:`convection_diffusion_concentration`) Constituent transport equations (concentration  diffusion convection).

- **[convection_diffusion_temperature]**  (*type:* :ref:`convection_diffusion_temperature`) Energy equations (temperature diffusion convection).

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_concentration_turbulent:

**pb_thermohydraulique_concentration_turbulent**
------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
turbulence modelling.

Usage:

| **pb_thermohydraulique_concentration_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_turbulent]**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration_turbulent]**  :ref:`convection_diffusion_concentration_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature_turbulent]**  :ref:`convection_diffusion_temperature_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_turbulent]**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[convection_diffusion_concentration_turbulent]**  (*type:* :ref:`convection_diffusion_concentration_turbulent`) Constituent transport equations (concentration  diffusion convection) as well as the associated turbulence model equations.

- **[convection_diffusion_temperature_turbulent]**  (*type:* :ref:`convection_diffusion_temperature_turbulent`) Energy equation (temperature diffusion  convection) as well as the associated turbulence model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_concentration_turbulent_scalaires_passifs:

**pb_thermohydraulique_concentration_turbulent_scalaires_passifs**
------------------------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
turbulence modelling and with the additional passive scalar equations.

Usage:

| **pb_thermohydraulique_concentration_turbulent_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_turbulent]**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_concentration_turbulent]**  :ref:`convection_diffusion_concentration_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature_turbulent]**  :ref:`convection_diffusion_temperature_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_turbulent]**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[convection_diffusion_concentration_turbulent]**  (*type:* :ref:`convection_diffusion_concentration_turbulent`) Constituent transport equations (concentration  diffusion convection) as well as the associated turbulence model equations.

- **[convection_diffusion_temperature_turbulent]**  (*type:* :ref:`convection_diffusion_temperature_turbulent`) Energy equations (temperature diffusion  convection) as well as the associated turbulence model equations.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_especes_qc:

**pb_thermohydraulique_especes_qc**
-----------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermo-hydraulic problem for a multi-species quasi-compressible fluid.

Usage:

| **pb_thermohydraulique_especes_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_qc**  :ref:`navier_stokes_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_qc**  :ref:`convection_diffusion_chaleur_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **navier_stokes_qc**  (*type:* :ref:`navier_stokes_qc`) Navier-Stokes equation for a quasi-compressible fluid.

- **convection_diffusion_chaleur_qc**  (*type:* :ref:`convection_diffusion_chaleur_qc`) Temperature equation for a quasi-compressible fluid.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_especes_turbulent_qc:

**pb_thermohydraulique_especes_turbulent_qc**
---------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of turbulent thermohydraulic problem under low Mach number with passive 
scalar equations.

Usage:

| **pb_thermohydraulique_especes_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_turbulent_qc**  :ref:`navier_stokes_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_turbulent_qc**  :ref:`convection_diffusion_chaleur_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **navier_stokes_turbulent_qc**  (*type:* :ref:`navier_stokes_turbulent_qc`) Navier-Stokes equations under low Mach number as well  as the associated turbulence model equations.

- **convection_diffusion_chaleur_turbulent_qc**  (*type:* :ref:`convection_diffusion_chaleur_turbulent_qc`) Energy equation under low Mach number  as well as the associated turbulence model equations.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_especes_wc:

**pb_thermohydraulique_especes_wc**
-----------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermo-hydraulic problem for a multi-species weakly-compressible fluid.

Usage:

| **pb_thermohydraulique_especes_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_weakly_compressible**  :ref:`fluide_weakly_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_wc**  :ref:`navier_stokes_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_wc**  :ref:`convection_diffusion_chaleur_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_weakly_compressible**  (*type:* :ref:`fluide_weakly_compressible`) The fluid medium associated with the problem.

- **navier_stokes_wc**  (*type:* :ref:`navier_stokes_wc`) Navier-Stokes equation for a weakly-compressible fluid.

- **convection_diffusion_chaleur_wc**  (*type:* :ref:`convection_diffusion_chaleur_wc`) Temperature equation for a weakly-compressible fluid.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_qc:

**pb_thermohydraulique_qc**
---------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermo-hydraulic problem for a quasi-compressible fluid.

Keywords for the unknowns other than pressure, velocity, temperature are :

masse_volumique : density

enthalpie : enthalpy

pression : reduced pressure

pression_tot : total pressure.

Usage:

| **pb_thermohydraulique_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_qc**  :ref:`navier_stokes_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_qc**  :ref:`convection_diffusion_chaleur_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **navier_stokes_qc**  (*type:* :ref:`navier_stokes_qc`) Navier-Stokes equation for a quasi-compressible fluid.

- **convection_diffusion_chaleur_qc**  (*type:* :ref:`convection_diffusion_chaleur_qc`) Temperature equation for a quasi-compressible fluid.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_scalaires_passifs:

**pb_thermohydraulique_scalaires_passifs**
------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermohydraulic problem, with the additional passive scalar equations.

Usage:

| **pb_thermohydraulique_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_standard]**  :ref:`navier_stokes_standard`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature]**  :ref:`convection_diffusion_temperature`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_standard]**  (*type:* :ref:`navier_stokes_standard`) Navier-Stokes equations.

- **[convection_diffusion_temperature]**  (*type:* :ref:`convection_diffusion_temperature`) Energy equations (temperature diffusion convection).

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_turbulent:

**pb_thermohydraulique_turbulent**
----------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermohydraulic problem, with turbulence modelling.

Usage:

| **pb_thermohydraulique_turbulent** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_turbulent**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_temperature_turbulent**  :ref:`convection_diffusion_temperature_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **navier_stokes_turbulent**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **convection_diffusion_temperature_turbulent**  (*type:* :ref:`convection_diffusion_temperature_turbulent`) Energy equation (temperature diffusion  convection) as well as the associated turbulence model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_turbulent_qc:

**pb_thermohydraulique_turbulent_qc**
-------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of turbulent thermohydraulic problem under low Mach number.

Warning : Available for VDF and VEF P0/P1NC discretization only.

Usage:

| **pb_thermohydraulique_turbulent_qc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_quasi_compressible**  :ref:`fluide_quasi_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_turbulent_qc**  :ref:`navier_stokes_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_turbulent_qc**  :ref:`convection_diffusion_chaleur_turbulent_qc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_quasi_compressible**  (*type:* :ref:`fluide_quasi_compressible`) The fluid medium associated with the problem.

- **navier_stokes_turbulent_qc**  (*type:* :ref:`navier_stokes_turbulent_qc`) Navier-Stokes equations under low Mach number as well  as the associated turbulence model equations.

- **convection_diffusion_chaleur_turbulent_qc**  (*type:* :ref:`convection_diffusion_chaleur_turbulent_qc`) Energy equation under low Mach number  as well as the associated turbulence model equations.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_turbulent_scalaires_passifs:

**pb_thermohydraulique_turbulent_scalaires_passifs**
----------------------------------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermohydraulic problem, with turbulence modelling and with the additional 
passive scalar equations.

Usage:

| **pb_thermohydraulique_turbulent_scalaires_passifs** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_incompressible**  :ref:`fluide_incompressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[navier_stokes_turbulent]**  :ref:`navier_stokes_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[convection_diffusion_temperature_turbulent]**  :ref:`convection_diffusion_temperature_turbulent`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **equations_scalaires_passifs**  :ref:`listeqn`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_incompressible**  (*type:* :ref:`fluide_incompressible`) The fluid medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituents.

- **[navier_stokes_turbulent]**  (*type:* :ref:`navier_stokes_turbulent`) Navier-Stokes equations as well as the associated turbulence  model equations.

- **[convection_diffusion_temperature_turbulent]**  (*type:* :ref:`convection_diffusion_temperature_turbulent`) Energy equations (temperature diffusion  convection) as well as the associated turbulence model equations.

- **equations_scalaires_passifs**  (*type:* :ref:`listeqn`) Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN  or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only  one calculation) different schemes or different boundary conditions for the scalar  transport equation.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pb_thermohydraulique_wc:

**pb_thermohydraulique_wc**
---------------------------

**Inherits from:** :ref:`pb_gen_base` 


Resolution of thermo-hydraulic problem for a weakly-compressible fluid.

Keywords for the unknowns other than pressure, velocity, temperature are :

masse_volumique : density

pression : reduced pressure

pression_tot : total pressure

pression_hydro : hydro-static pressure

pression_eos : pressure used in state equation.

Usage:

| **pb_thermohydraulique_wc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **fluide_weakly_compressible**  :ref:`fluide_weakly_compressible`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **navier_stokes_wc**  :ref:`navier_stokes_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **convection_diffusion_chaleur_wc**  :ref:`convection_diffusion_chaleur_wc`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **fluide_weakly_compressible**  (*type:* :ref:`fluide_weakly_compressible`) The fluid medium associated with the problem.

- **navier_stokes_wc**  (*type:* :ref:`navier_stokes_wc`) Navier-Stokes equation for a weakly-compressible fluid.

- **convection_diffusion_chaleur_wc**  (*type:* :ref:`convection_diffusion_chaleur_wc`) Temperature equation for a weakly-compressible fluid.

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

.. _pbc_med:

**pbc_med**
-----------

**Inherits from:** :ref:`pb_gen_base` 


Allows to read med files and post-process them.

Usage:

**pbc_med** **list_info_med**  

Parameters are:

- **list_info_med**  (*type:* :ref:`list_info_med`) not_set


----

.. _problem_read_generic:

**problem_read_generic**
------------------------

**Inherits from:** :ref:`pb_gen_base` 


The probleme_read_generic differs rom the rest of the TRUST code : The problem does 
not state the number of equations that are enclosed in the problem.
As the list of equations to be solved in the generic read problem is declared in 
the data file and not pre-defined in the structure of the problem, each equation has 
to be distinctively associated with the problem with the Associate keyword.

Usage:

| **problem_read_generic** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[milieu]**  :ref:`milieu_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[constituant]**  :ref:`constituant`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitement | post_processing]**  :ref:`corps_postraitement`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[postraitements | post_processings]**  :ref:`postraitements`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_de_postraitements]**  :ref:`liste_post_ok`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste_postraitements]**  :ref:`liste_post`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sauvegarde_simple]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reprise]**  :ref:`format_file`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resume_last_time]**  :ref:`format_file`
| }

Parameters are:

- **[milieu]**  (*type:* :ref:`milieu_base`) The medium associated with the problem.

- **[constituant]**  (*type:* :ref:`constituant`) Constituent.

- **[postraitement | post_processing]**  (*type:* :ref:`corps_postraitement`) One post-processing (without name).

- **[postraitements | post_processings]**  (*type:* :ref:`postraitements`) List of Postraitement objects (with name).

- **[liste_de_postraitements]**  (*type:* :ref:`liste_post_ok`) This

- **[liste_postraitements]**  (*type:* :ref:`liste_post`) This block defines the output files to be written during the computation. The output format is lata in order to use OpenDX to draw the results. This block can be divided in one or several sub-blocks that can be written at different  frequencies and in different directories. Attention. The directory lata used in this example should be created before running the computation  or the lata files will be lost.

- **[sauvegarde]**  (*type:* :ref:`format_file`) Keyword used when calculation results are to be backed up. When a coupling is performed, the backup-recovery file name must be well specified  for each problem. In this case, you must save to different files and correctly specify these files  when resuming the calculation.

- **[sauvegarde_simple]**  (*type:* :ref:`format_file`) The same keyword than Sauvegarde except, the last time step only is  saved.

- **[reprise]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file (see the  class format_file). If format_reprise is xyz, the name_file file should be the .xyz file created by the  previous calculation. With this file, it is possible to resume a parallel calculation on P processors,  whereas the previous calculation has been run on N (N<>P) processors. Should the calculation be resumed, values for the tinit (see schema_temps_base) time  fields are taken from the name_file file. If there is no backup corresponding to this time in the name_file, TRUST exits in  error.

- **[resume_last_time]**  (*type:* :ref:`format_file`) Keyword to resume a calculation based on the name_file file, resume  the calculation at the last time found in the file (tinit is set to last time of saved  files).


----

**Keywords derived from penalisation_l2_ftd**
=============================================

.. _penalisation_l2_ftd:

**penalisation_l2_ftd**
-----------------------

**Synonyms:** pp


not_set


----

**Keywords derived from porosites**
===================================

.. _porosites:

**porosites**
-------------


To define the volume porosity and surface porosity that are uniform in every direction 
in space on a sub-area.

Porosity was only usable in VDF discretization, and now available for VEF P1NC/P0.

Observations :

- Surface porosity values must be given in every direction in space (set this value 
to 1 if there is no porosity),

- Prior to defining porosity, the problem must have been discretized.NL2 Can 't be 
used in VEF discretization, use Porosites_champ instead.

Usage:

**porosites** **aco** **sous_zone | sous_zone1** **bloc** **[sous_zone2]** **[bloc2]** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **sous_zone | sous_zone1**  (*type:* string) Name of the sub-area to which porosity are allocated.

- **bloc**  (*type:* :ref:`bloc_lecture_poro`) Surface and volume porosity values.

- **[sous_zone2]**  (*type:* string) Name of the 2nd sub-area to which porosity are allocated.

- **[bloc2]**  (*type:* :ref:`bloc_lecture_poro`) Surface and volume porosity values.

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

**Keywords derived from postraitements**
========================================

.. _postraitements:

**postraitements**
------------------

**Synonyms:** post_processings


Keyword to use several results files.
List of objects of post-processing (with name).


----

**Keywords derived from precond_base**
======================================

.. _ilu:

**ilu**
-------

**Inherits from:** :ref:`precond_base` 


This preconditionner can be only used with the generic GEN solver.

Usage:

| **ilu** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[type]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[filling]**  int
| }

Parameters are:

- **[type]**  (*type:* int) values can be 0|1|2|3 for null|left|right|left-and-right preconditionning  (default value = 2)

- **[filling]**  (*type:* int) default value = 1.


----

.. _precond_base:

**precond_base**
----------------


Basic class for preconditioning.

Usage:

| **precond_base** *str*
| **Read** *str* {
| }


----

.. _precondsolv:

**precondsolv**
---------------

**Inherits from:** :ref:`precond_base` 


not_set

Usage:

**precondsolv** **solveur**  

Parameters are:

- **solveur**  (*type:* :ref:`solveur_sys_base`) Solver type.


----

.. _ssor:

**ssor**
--------

**Inherits from:** :ref:`precond_base` 


Symmetric successive over-relaxation algorithm.

Usage:

| **ssor** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[omega]**  double
| }

Parameters are:

- **[omega]**  (*type:* double) Over-relaxation facteur (between 1 and 2, default value 1.6).


----

.. _ssor_bloc:

**ssor_bloc**
-------------

**Inherits from:** :ref:`precond_base` 


not_set

Usage:

| **ssor_bloc** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha_0]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond0]**  :ref:`precond_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha_1]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precond1]**  :ref:`precond_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[alpha_a]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[preconda]**  :ref:`precond_base`
| }

Parameters are:

- **[alpha_0]**  (*type:* double) not_set

- **[precond0]**  (*type:* :ref:`precond_base`) not_set

- **[alpha_1]**  (*type:* double) not_set

- **[precond1]**  (*type:* :ref:`precond_base`) not_set

- **[alpha_a]**  (*type:* double) not_set

- **[preconda]**  (*type:* :ref:`precond_base`) not_set


----

**Keywords derived from reactions**
===================================

.. _reactions:

**reactions**
-------------


list of reactions


----

**Keywords derived from saturation_base**
=========================================

.. _saturation_base:

**saturation_base**
-------------------


Basic class for a liquid-gas interface (used in pb_multiphase)

Usage:

| **saturation_base** *str*
| **Read** *str* {
| }


----

.. _saturation_constant:

**saturation_constant**
-----------------------

**Inherits from:** :ref:`saturation_base` 


Class for saturation constant

Usage:

| **saturation_constant** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p_sat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[t_sat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[lvap]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[hlsat]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[hvsat]**  double
| }

Parameters are:

- **[p_sat]**  (*type:* double) Define the saturation pressure value (this is a required parameter)

- **[t_sat]**  (*type:* double) Define the saturation temperature value (this is a required parameter)

- **[lvap]**  (*type:* double) Latent heat of vaporization

- **[hlsat]**  (*type:* double) Liquid saturation enthalpy

- **[hvsat]**  (*type:* double) Vapor saturation enthalpy


----

.. _saturation_sodium:

**saturation_sodium**
---------------------

**Inherits from:** :ref:`saturation_base` 


Class for saturation sodium

Usage:

| **saturation_sodium** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[p_ref]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[t_ref]**  double
| }

Parameters are:

- **[p_ref]**  (*type:* double) Use to fix the pressure value in the closure law. If not specified, the value of the pressure unknown will be used

- **[t_ref]**  (*type:* double) Use to fix the temperature value in the closure law. If not specified, the value of the temperature unknown will be used


----

**Keywords derived from schema_temps_base**
===========================================

.. _euler_scheme:

**euler_scheme**
----------------

**Synonyms:** schema_euler_explicite, scheme_euler_explicit

**Inherits from:** :ref:`schema_temps_base` 


This is the Euler explicit scheme.

Usage:

| **euler_scheme** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _leap_frog:

**leap_frog**
-------------

**Inherits from:** :ref:`schema_temps_base` 


This is the leap-frog scheme.

Usage:

| **leap_frog** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_2:

**runge_kutta_ordre_2**
-----------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a low-storage Runge-Kutta scheme of second order that uses 2 integration points.
The method is presented by Williamson (case 1) in https://www.sciencedirect.com/science/article/pii/0021999180900339

Usage:

| **runge_kutta_ordre_2** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_2_classique:

**runge_kutta_ordre_2_classique**
---------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a classical Runge-Kutta scheme of second order that uses 2 integration points.

Usage:

| **runge_kutta_ordre_2_classique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_3:

**runge_kutta_ordre_3**
-----------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a low-storage Runge-Kutta scheme of third order that uses 3 integration points.
The method is presented by Williamson (case 7) in https://www.sciencedirect.com/science/article/pii/0021999180900339

Usage:

| **runge_kutta_ordre_3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_3_classique:

**runge_kutta_ordre_3_classique**
---------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a classical Runge-Kutta scheme of third order that uses 3 integration points.

Usage:

| **runge_kutta_ordre_3_classique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_4:

**runge_kutta_ordre_4**
-----------------------

**Synonyms:** runge_kutta_ordre_4_d3p

**Inherits from:** :ref:`schema_temps_base` 


This is a low-storage Runge-Kutta scheme of fourth order that uses 3 integration points.
The method is presented by Williamson (case 17) in https://www.sciencedirect.com/science/article/pii/0021999180900339

Usage:

| **runge_kutta_ordre_4** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_4_classique_3_8:

**runge_kutta_ordre_4_classique_3_8**
-------------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points 
and the 3/8 rule.

Usage:

| **runge_kutta_ordre_4_classique_3_8** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_ordre_4_classique:

**runge_kutta_ordre_4_classique**
---------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points.

Usage:

| **runge_kutta_ordre_4_classique** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _runge_kutta_rationnel_ordre_2:

**runge_kutta_rationnel_ordre_2**
---------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is the Runge-Kutta rational scheme of second order.
The method is described in the note: Wambeck - Rational Runge-Kutta methods for solving 
systems of ordinary differential equations, at the link: https://link.springer.com/article/10.1007/BF02252381.
Although rational methods require more computational work than linear ones, they 
can have some other properties, such as a stable behaviour with explicitness, which 
make them preferable.
The CFD application of this RRK2 scheme is described in the note: https://link.springer.com/content/pdf/10.1007\%2F3-540-13917-6_112.pdf.

Usage:

| **runge_kutta_rationnel_ordre_2** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _sch_cn_ex_iteratif:

**sch_cn_ex_iteratif**
----------------------

**Inherits from:** :ref:`schema_temps_base` 


This keyword also describes a Crank-Nicholson method of second order accuracy but 
here, for scalars, because of instablities encountered when dt>dt_CFL, the Crank Nicholson 
scheme is not applied to scalar quantities.
Scalars are treated according to Euler-Explicite scheme at the end of the CN treatment 
for velocity flow fields (by doing p Euler explicite under-iterations at dt<=dt_CFL).
Parameters are the sames (but default values may change) compare to the Sch_CN_iterative 
scheme plus a relaxation keyword: niter_min (2 by default), niter_max (6 by default), 
niter_avg (3 by default), facsec_max (20 by default), seuil (0.05 by default)

Usage:

| **sch_cn_ex_iteratif** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[omega]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_min]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_avg]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[omega]**  (*type:* double) relaxation factor (0.1 by default)

- **[niter_min]**  (*type:* int) minimal number of p-iterations to satisfy convergence criteria (2 by default)

- **[niter_max]**  (*type:* int) number of maximum p-iterations allowed to satisfy convergence criteria (6  by default)

- **[niter_avg]**  (*type:* int) threshold of p-iterations (3 by default). If the number of p-iterations is greater than niter_avg, facsec is reduced, if lesser  than niter_avg, facsec is increased (but limited by the facsec_max value).

- **[facsec_max]**  (*type:* double) maximum ratio allowed between dynamical time step returned by iterative  process and stability time returned by CFL condition (2 by default).

- **[seuil]**  (*type:* double) criteria for ending iterative process (Max( || u(p) - u(p-1)||/Max ||  u(p) ||) < seuil) (0.001 by default)

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _sch_cn_iteratif:

**sch_cn_iteratif**
-------------------

**Inherits from:** :ref:`schema_temps_base` 


The Crank-Nicholson method of second order accuracy.
A mid-point rule formulation is used (Euler-centered scheme).
The basic scheme is: $$u(t+1) = u(t) + du/dt(t+1/2)*dt$$ The estimation of the time 
derivative du/dt at the level (t+1/2) is obtained either by iterative process.
The time derivative du/dt at the level (t+1/2) is calculated iteratively with a simple 
under-relaxations method.
Since the method is implicit, neither the cfl nor the fourier stability criteria 
must be respected.
The time step is calculated in a way that the iterative procedure converges with 
the less iterations as possible.

Remark : for stationary or RANS calculations, no limitation can be given for time 
step through high value of facsec_max parameter (for instance : facsec_max 1000).
In counterpart, for LES calculations, high values of facsec_max may engender numerical 
instabilities.

Usage:

| **sch_cn_iteratif** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_min]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_avg]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[niter_min]**  (*type:* int) minimal number of p-iterations to satisfy convergence criteria (2 by default)

- **[niter_max]**  (*type:* int) number of maximum p-iterations allowed to satisfy convergence criteria (6  by default)

- **[niter_avg]**  (*type:* int) threshold of p-iterations (3 by default). If the number of p-iterations is greater than niter_avg, facsec is reduced, if lesser  than niter_avg, facsec is increased (but limited by the facsec_max value).

- **[facsec_max]**  (*type:* double) maximum ratio allowed between dynamical time step returned by iterative  process and stability time returned by CFL condition (2 by default).

- **[seuil]**  (*type:* double) criteria for ending iterative process (Max( || u(p) - u(p-1)||/Max ||  u(p) ||) < seuil) (0.001 by default)

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_adams_bashforth_order_2:

**schema_adams_bashforth_order_2**
----------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_adams_bashforth_order_2** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_adams_bashforth_order_3:

**schema_adams_bashforth_order_3**
----------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_adams_bashforth_order_3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_adams_moulton_order_2:

**schema_adams_moulton_order_2**
--------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_adams_moulton_order_2** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[facsec_max]**  (*type:* double) Maximum ratio allowed between time step and stability time returned by  CFL condition. The initial ratio given by facsec keyword is changed during the calculation with  the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some  implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3  needs facsec=facsec_max=1.  Advice:NL2 The calculation may start with a facsec specified by the user and increased  by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set  to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic  only or thermal hydraulic with forced convection and low coupling between velocity  and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic  with forced convection and strong coupling between velocity and temperature (Boussinesq  value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection,  facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8)  as if the scheme was unconditionally stableNL2These values can also be used as rule  of thumb for initial facsec with a facsec_max limit higher.

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_adams_moulton_order_3:

**schema_adams_moulton_order_3**
--------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_adams_moulton_order_3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[facsec_max]**  (*type:* double) Maximum ratio allowed between time step and stability time returned by  CFL condition. The initial ratio given by facsec keyword is changed during the calculation with  the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some  implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3  needs facsec=facsec_max=1.  Advice:NL2 The calculation may start with a facsec specified by the user and increased  by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set  to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic  only or thermal hydraulic with forced convection and low coupling between velocity  and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic  with forced convection and strong coupling between velocity and temperature (Boussinesq  value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection,  facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8)  as if the scheme was unconditionally stableNL2These values can also be used as rule  of thumb for initial facsec with a facsec_max limit higher.

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_backward_differentiation_order_2:

**schema_backward_differentiation_order_2**
-------------------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_backward_differentiation_order_2** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[facsec_max]**  (*type:* double) Maximum ratio allowed between time step and stability time returned by  CFL condition. The initial ratio given by facsec keyword is changed during the calculation with  the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some  implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3  needs facsec=facsec_max=1.  Advice:NL2 The calculation may start with a facsec specified by the user and increased  by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set  to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic  only or thermal hydraulic with forced convection and low coupling between velocity  and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic  with forced convection and strong coupling between velocity and temperature (Boussinesq  value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection,  facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8)  as if the scheme was unconditionally stableNL2These values can also be used as rule  of thumb for initial facsec with a facsec_max limit higher.

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_backward_differentiation_order_3:

**schema_backward_differentiation_order_3**
-------------------------------------------

**Inherits from:** :ref:`schema_temps_base` 


not_set

Usage:

| **schema_backward_differentiation_order_3** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[facsec_max]**  (*type:* double) Maximum ratio allowed between time step and stability time returned by  CFL condition. The initial ratio given by facsec keyword is changed during the calculation with  the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some  implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3  needs facsec=facsec_max=1.  Advice:NL2 The calculation may start with a facsec specified by the user and increased  by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set  to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic  only or thermal hydraulic with forced convection and low coupling between velocity  and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic  with forced convection and strong coupling between velocity and temperature (Boussinesq  value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection,  facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8)  as if the scheme was unconditionally stableNL2These values can also be used as rule  of thumb for initial facsec with a facsec_max limit higher.

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_euler_implicite:

**schema_euler_implicite**
--------------------------

**Synonyms:** scheme_euler_implicit

**Inherits from:** :ref:`schema_temps_base` 


This is the Euler implicit scheme.

Usage:

| **schema_euler_implicite** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[resolution_monolithique]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[facsec_max]**  (*type:* double) 1 Maximum ratio allowed between time step and stability time returned  by CFL condition. The initial ratio given by facsec keyword is changed during the calculation with  the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some  implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3  needs facsec=facsec_max=1.  Advice:NL2 The calculation may start with a facsec specified by the user and increased  by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set  to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic  only or thermal hydraulic with forced convection and low coupling between velocity  and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic  with forced convection and strong coupling between velocity and temperature (Boussinesq  value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection,  facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8)  as if the scheme was unconditionally stableNL2These values can also be used as rule  of thumb for initial facsec with a facsec_max limit higher.

- **[resolution_monolithique]**  (*type:* :ref:`bloc_lecture`) Activate monolithic resolution for coupled problems. Solves together the equations corresponding to the application domains in the given  order. All aplication domains of the coupled equations must be given to determine the order  of resolution. If the monolithic solving is not wanted for a specific application domain, an underscore  can be added as prefix. For example, resolution_monolithique { dom1 { dom2 dom3 } _dom4 } will solve in a  single matrix the equations having dom1 as application domain, then the equations  having dom2 or dom3 as application domain in a single matrix, then the equations having  dom4 as application domain in a sequential way (not in a single matrix).

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_implicite_base:

**schema_implicite_base**
-------------------------

**Inherits from:** :ref:`schema_temps_base` 


Basic class for implicite time scheme.

Usage:

| **schema_implicite_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[max_iter_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **solveur**  :ref:`solveur_implicite_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[max_iter_implicite]**  (*type:* int) Maximum number of iterations allowed for the solver (by default 200).

- **solveur**  (*type:* :ref:`solveur_implicite_base`) This keyword is used to designate the solver selected in  the situation where the time scheme is an implicit scheme. solver is the name of the solver that allows equation diffusion and convection operators  to be set as implicit terms. Keywords corresponding to this functionality are Simple (SIMPLE type algorithm),  Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit  with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified  solver, it will use fewer timesteps, and ICE (for PB_multiphase). But it may run faster because the pressure matrix is not re-assembled and thus provides  CPU gains.  Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple,  then Piso, and at least Simpler. Because the two first give a fastest convergence (several times) than Piso and the  Simpler has not been validated. It seems also than Implicite and Piso schemes give better results than the Simple  scheme when the flow is not fully stationary. Thus, if the solution obtained with Simple is not stationary, it is recommended to  switch to Piso or Implicite scheme.

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_predictor_corrector:

**schema_predictor_corrector**
------------------------------

**Inherits from:** :ref:`schema_temps_base` 


This is the predictor-corrector scheme (second order).
It is more accurate and economic than MacCormack scheme.
It gives best results with a second ordre convective scheme like quick, centre (VDF).

Usage:

| **schema_predictor_corrector** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

.. _schema_temps_base:

**schema_temps_base**
---------------------


Basic class for time schemes.
This scheme will be associated with a problem and the equations of this problem.

Usage:

| **schema_temps_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tinit]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tmax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tcpumax]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_max]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_sauv]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_impr]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_statio]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[residuals]**  :ref:`residuals`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_diffusion_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[impr_extremums]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_error_if_not_converged_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_conv_subiteration_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dt_start]**  :ref:`dt_start`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_pas_dt_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[niter_max_diffusion_implicite]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[precision_impr]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[periode_sauvegarde_securite_en_heures]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_check_disk_space]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_progress]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[disable_dt_ev]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[gnuplot_header]**  int
| }

Parameters are:

- **[tinit]**  (*type:* double) Value of initial calculation time (0 by default).

- **[tmax]**  (*type:* double) Time during which the calculation will be stopped (1e30s by default).

- **[tcpumax]**  (*type:* double) CPU time limit (must be specified in hours) for which the calculation  is stopped (1e30s by default).

- **[dt_min]**  (*type:* double) Minimum calculation time step (1e-16s by default).

- **[dt_max]**  (*type:* string) Maximum calculation time step as function of time (1e30s by default).

- **[dt_sauv]**  (*type:* double) Save time step value (1e30s by default). Every dt_sauv, fields are saved in the .sauv file. The file contains all the information saved over time. If this instruction is not entered, results are saved only upon calculation completion. To disable the writing of the .sauv files, you must specify 0. Note that dt_sauv is in terms of physical time (not cpu time).

- **[dt_impr]**  (*type:* double) Scheme parameter printing time step in time (1e30s by default). The time steps and the flux balances are printed (incorporated onto every side of  processed domains) into the .out file.

- **[facsec]**  (*type:* double) Value assigned to the safety factor for the time step (1. by default). The time step calculated is multiplied by the safety factor. The first thing to try when a calculation does not converge with an explicit time  scheme is to reduce the facsec to 0.5.  Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example  Schema_Adams_Bashforth_order_3.

- **[seuil_statio]**  (*type:* double) Value of the convergence threshold (1e-12 by default). Problems using this type of time scheme converge when the derivatives dGi/dt NL1  of all the unknown transported values Gi have a combined absolute value less than  this value. This is the keyword used to set the permanent rating threshold.

- **[residuals]**  (*type:* :ref:`residuals`) To specify how the residuals will be computed (default max norm, possible  to choose L2-norm instead).

- **[diffusion_implicite]**  (*type:* int) Keyword to make the diffusive term in the Navier-Stokes equations implicit  (in this case, it should be set to 1). The stability time step is then only based on the convection time step (dt=facsec*dt_convection). Thus, in some circumstances, an important gain is achieved with respect to the time  step (large diffusion with respect to convection on tightened meshes). Caution: It is however recommended that the user avoids exceeding the convection  time step by selecting a too large facsec value. Start with a facsec value of 1 and then increase it gradually if you wish to accelerate  calculation. In addition, for a natural convection calculation with a zero initial velocity, in  the first time step, the convection time is infinite and therefore dt=facsec*dt_max.

- **[seuil_diffusion_implicite]**  (*type:* double) This keyword changes the default value (1e-6) of convergency criteria  for the resolution by conjugate gradient used for implicit diffusion.

- **[impr_diffusion_implicite]**  (*type:* int) Unactivate (default) or not the printing of the convergence during the resolution  of the conjugate gradient.

- **[impr_extremums]**  (*type:* int) Print unknowns extremas

- **[no_error_if_not_converged_diffusion_implicite]**  (*type:* int) not_set

- **[no_conv_subiteration_diffusion_implicite]**  (*type:* int) not_set

- **[dt_start]**  (*type:* :ref:`dt_start`) dt_start dt_min : the first iteration is based on dt_min.  dt_start dt_calc : the time step at first iteration is calculated in agreement with  CFL condition.  dt_start dt_fixe value : the first time step is fixed by the user (recommended when  resuming calculation with Crank Nicholson temporal scheme to ensure continuity).  By default, the first iteration is based on dt_calc.

- **[nb_pas_dt_max]**  (*type:* int) Maximum number of calculation time steps (1e9 by default).

- **[niter_max_diffusion_implicite]**  (*type:* int) This keyword changes the default value (number of unknowns) of the maximal  iterations number in the conjugate gradient method used for implicit diffusion.

- **[precision_impr]**  (*type:* int) Optional keyword to define the digit number for flux values printed into  .out files (by default 3).

- **[periode_sauvegarde_securite_en_heures]**  (*type:* double) To change the default period (23 hours) between the save of the fields  in .sauv file.

- **[no_check_disk_space]**  (*type:* flag) To disable the check of the available amount of disk space during the calculation.

- **[disable_progress]**  (*type:* flag) To disable the writing of the .progress file.

- **[disable_dt_ev]**  (*type:* flag) To disable the writing of the .dt_ev file.

- **[gnuplot_header]**  (*type:* int) Optional keyword to modify the header of the .out files. Allows to use the column title instead of columns number.


----

**Keywords derived from solveur_implicite_base**
================================================

.. _ice:

**ice**
-------

**Inherits from:** :ref:`solveur_implicite_base` 


Implicit Continuous-fluid Eulerian solver which is useful for a multiphase problem.
Robust pressure reduction resolution.

Usage:

| **ice** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pression_degeneree]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[reduction_pression | pressure_reduction]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[criteres_convergence]**  :ref:`bloc_criteres_convergence`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[iter_min]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_diffusion_for_sets]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[pression_degeneree]**  (*type:* int) Set to 1 if the pressure field is degenerate (ex. : incompressible fluid with no imposed-pressure BCs). Default: autodetected

- **[reduction_pression | pressure_reduction]**  (*type:* int) Set to 1 if the user wants a resolution with a pressure reduction. Otherwise, the flag is to be set to 0 so that the complete matrix is considered. The default value of this flag is 1.

- **[criteres_convergence]**  (*type:* :ref:`bloc_criteres_convergence`) Set the convergence thresholds for each unknown (i.e:  alpha, temperature, velocity and pressure). The default values are respectively 0.01, 0.1, 0.01 and 100

- **[iter_min]**  (*type:* int) Number of minimum iterations

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[facsec_diffusion_for_sets]**  (*type:* double) facsec to impose on the diffusion time step in sets while the total time  step stays smaller than the convection time step.

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _implicite:

**implicite**
-------------

**Inherits from:** :ref:`solveur_implicite_base` 


similar to PISO, but as it looks like a simplified solver, it will use fewer timesteps.
But it may run faster because the pressure matrix is not re-assembled and thus provides 
CPU gains.

Usage:

| **implicite** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _piso:

**piso**
--------

**Inherits from:** :ref:`solveur_implicite_base` 


Piso (Pressure Implicit with Split Operator) - method to solve N_S.

Usage:

| **piso** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _sets:

**sets**
--------

**Inherits from:** :ref:`solveur_implicite_base` 


Stability-Enhancing Two-Step solver which is useful for a multiphase problem.
Ref : J.
H.
MAHAFFY, A stability-enhancing two-step method for fluid flow calculations, Journal 
of Computational Physics, 46, 3, 329 (1982).

Usage:

| **sets** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[criteres_convergence]**  :ref:`bloc_criteres_convergence`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[iter_min]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[facsec_diffusion_for_sets]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[criteres_convergence]**  (*type:* :ref:`bloc_criteres_convergence`) Set the convergence thresholds for each unknown (i.e:  alpha, temperature, velocity and pressure). The default values are respectively 0.01, 0.1, 0.01 and 100

- **[iter_min]**  (*type:* int) Number of minimum iterations

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[facsec_diffusion_for_sets]**  (*type:* double) facsec to impose on the diffusion time step in sets while the total time  step stays smaller than the convection time step.

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _simple:

**simple**
----------

**Inherits from:** :ref:`solveur_implicite_base` 


SIMPLE type algorithm

Usage:

| **simple** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[relax_pression]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[relax_pression]**  (*type:* double) Value between 0 and 1 (by default 1), this keyword is used only by the  SIMPLE algorithm for relaxing the increment of pressure.

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _simpler:

**simpler**
-----------

**Inherits from:** :ref:`solveur_implicite_base` 


Simpler method for incompressible systems.

Usage:

| **simpler** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **seuil_convergence_implicite**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **seuil_convergence_implicite**  (*type:* double) Keyword to set the value of the convergence criteria for the resolution  of the implicit system build to solve either the Navier_Stokes equation (only for  Simple and Simpler algorithms) or a scalar equation. It is adviced to use the default value (1e6) to solve the implicit system only once  by time step. This value must be decreased when a coupling between problems is considered.

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

.. _solveur_implicite_base:

**solveur_implicite_base**
--------------------------


Class for solver in the situation where the time scheme is the implicit scheme.
Solver allows equation diffusion and convection operators to be set as implicit terms.

Usage:

| **solveur_implicite_base** *str*
| **Read** *str* {
| }


----

.. _solveur_lineaire_std:

**solveur_lineaire_std**
------------------------

**Inherits from:** :ref:`solveur_implicite_base` 


not_set

Usage:

| **solveur_lineaire_std** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| }

Parameters are:

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) not_set


----

.. _solveur_u_p:

**solveur_u_p**
---------------

**Inherits from:** :ref:`solveur_implicite_base` 


similar to simple.

Usage:

| **solveur_u_p** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[relax_pression]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_implicite]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_corrections_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_convergence_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_generation_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_verification_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[seuil_test_preliminaire_solveur]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[solveur]**  :ref:`solveur_sys_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[no_qdm]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[nb_it_max]**  int
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[controle_residu]**  flag
| }

Parameters are:

- **[relax_pression]**  (*type:* double) Value between 0 and 1 (by default 1), this keyword is used only by the  SIMPLE algorithm for relaxing the increment of pressure.

- **[seuil_convergence_implicite]**  (*type:* double) Convergence criteria.

- **[nb_corrections_max]**  (*type:* int) Maximum number of corrections performed by the PISO algorithm to achieve  the projection of the velocity field. The algorithm may perform less corrections then nb_corrections_max if the accuracy  of the projection is sufficient. (By default nb_corrections_max is set to 21).

- **[seuil_convergence_solveur]**  (*type:* double) value of the convergence criteria for the resolution of the implicit system  build by solving several times per time step the Navier_Stokes equation and the scalar  equations if any. This value MUST be used when a coupling between problems is considered (should be  set to a value typically of 0.1 or 0.01).

- **[seuil_generation_solveur]**  (*type:* double) Option to create a GMRES solver and use vrel as the convergence threshold  (implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than  vrel).

- **[seuil_verification_solveur]**  (*type:* double) Option to check if residual error ||Ax-B|| is lesser than vrel after the  implicit linear system Ax=B has been solved.

- **[seuil_test_preliminaire_solveur]**  (*type:* double) Option to decide if the implicit linear system Ax=B should be solved by  checking if the residual error ||Ax-B|| is bigger than vrel.

- **[solveur]**  (*type:* :ref:`solveur_sys_base`) Method (different from the default one, Gmres with diagonal preconditioning)  to solve the linear system.

- **[no_qdm]**  (*type:* flag) Keyword to not solve qdm equation (and turbulence models of these equation).

- **[nb_it_max]**  (*type:* int) Keyword to set the maximum iterations number for the Gmres.

- **[controle_residu]**  (*type:* flag) Keyword of Boolean type (by default 0). If set to 1, the convergence occurs if the residu suddenly increases.


----

**Keywords derived from sondes**
================================

.. _sondes:

**sondes**
----------


List of probes.


----

**Keywords derived from source_base**
=====================================

.. _acceleration:

**acceleration**
----------------

**Inherits from:** :ref:`source_base` 


Momentum source term to take in account the forces due to rotation or translation 
of a non Galilean referential R' (centre 0') into the Galilean referential R (centre 
0).

Usage:

| **acceleration** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[vitesse]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[acceleration]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[omega]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[domegadt]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[centre_rotation]**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[option]**  string into ["terme_complet", "coriolis_seul", "entrainement_seul"]
| }

Parameters are:

- **[vitesse]**  (*type:* :ref:`field_base`) Keyword for the velocity of the referential R' into the R referential  (dOO'/dt term [m.s-1]). The velocity is mandatory when you want to print the total cinetic energy into the  non-mobile Galilean referential R (see Ec_dans_repere_fixe keyword).

- **[acceleration]**  (*type:* :ref:`field_base`) Keyword for the acceleration of the referential R' into the R referential  (d2OO'/dt2 term [m.s-2]). field_base is a time dependant field (eg: Champ_Fonc_t).

- **[omega]**  (*type:* :ref:`field_base`) Keyword for a rotation of the referential R' into the R referential [rad.s-1]. field_base is a 3D time dependant field specified for example by a Champ_Fonc_t keyword. The time_field field should have 3 components even in 2D (In 2D: 0 0 omega).

- **[domegadt]**  (*type:* :ref:`field_base`) Keyword to define the time derivative of the previous rotation [rad.s-2]. Should be zero if the rotation is constant. The time_field field should have 3 components even in 2D (In 2D: 0 0 domegadt).

- **[centre_rotation]**  (*type:* :ref:`field_base`) Keyword to specify the centre of rotation (expressed in R' coordinates)  of R' into R (if the domain rotates with the R' referential, the centre of rotation  is 0'=(0,0,0)). The time_field should have 2 or 3 components according the dimension 2 or 3.

- **[option]**  (*type:* string into ["terme_complet", "coriolis_seul", "entrainement_seul"])  Keyword to specify  the kind of calculation: terme_complet (default option) will calculate both the Coriolis  and centrifugal forces, coriolis_seul will calculate the first one only, entrainement_seul  will calculate the second one only.


----

.. _boussinesq_concentration:

**boussinesq_concentration**
----------------------------

**Inherits from:** :ref:`source_base` 


Class to describe a source term that couples the movement quantity equation and constituent 
transport equation with the Boussinesq hypothesis.

Usage:

| **boussinesq_concentration** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **c0**  list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[verif_boussinesq]**  int
| }

Parameters are:

- **c0**  (*type:* list) Reference concentration field type. The only field type currently available is Champ_Uniforme (Uniform field).

- **[verif_boussinesq]**  (*type:* int) Keyword to check (1) or not (0) the reference concentration in comparison  with the mean concentration value in the domain. It is set to 1 by default.


----

.. _boussinesq_temperature:

**boussinesq_temperature**
--------------------------

**Inherits from:** :ref:`source_base` 


Class to describe a source term that couples the movement quantity equation and energy 
equation with the Boussinesq hypothesis.

Usage:

| **boussinesq_temperature** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **t0**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[verif_boussinesq]**  int
| }

Parameters are:

- **t0**  (*type:* string) Reference temperature value (oC or K). It can also be a time dependant function since the 1.6.6 version.

- **[verif_boussinesq]**  (*type:* int) Keyword to check (1) or not (0) the reference temperature in comparison with  the mean temperature value in the domain. It is set to 1 by default.


----

.. _canal_perio:

**canal_perio**
---------------

**Inherits from:** :ref:`source_base` 


Momentum source term to maintain flow rate.
The expression of the source term is:

S(t) = (2*(Q(0) - Q(t))-(Q(0)-Q(t-dt))/(coeff*dt*area)

NL2 Where:

coeff=damping coefficient

area=area of the periodic boundary

Q(t)=flow rate at time t

dt=time step

NL2 Three files will be created during calculation on a datafile named DataFile.data.
The first file contains the flow rate evolution.
The second file is useful for resuming a calculation with the flow rate of the previous 
stopped calculation, and the last one contains the pressure gradient evolution:

-DataFile_Channel_Flow_Rate_ProblemName_BoundaryName

-DataFile_Channel_Flow_Rate_repr_ProblemName_BoundaryName

-DataFile_Pressure_Gradient_ProblemName_BoundaryName

Usage:

| **canal_perio** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **bord**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[h]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coeff]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[debit_impose]**  double
| }

Parameters are:

- **bord**  (*type:* string) The name of the (periodic) boundary normal to the flow direction.

- **[h]**  (*type:* double) Half heigth of the channel.

- **[coeff]**  (*type:* double) Damping coefficient (optional, default value is 10).

- **[debit_impose]**  (*type:* double) Optional option to specify the aimed flow rate Q(0). If not used, Q(0) is computed by the code after the projection phase, where velocity  initial conditions are slighlty changed to verify incompressibility.


----

.. _coriolis:

**coriolis**
------------

**Inherits from:** :ref:`source_base` 


Keyword for a Coriolis term in hydraulic equation.
Warning: Only available in VDF.

Usage:

**coriolis** **omega**  

Parameters are:

- **omega**  (*type:* string) Value of omega.


----

.. _correction_antal:

**correction_antal**
--------------------

**Inherits from:** :ref:`source_base` 


Antal correction source term for multiphase problem

Usage:

| **correction_antal** *str*
| **Read** *str* {
| }


----

.. _darcy:

**darcy**
---------

**Inherits from:** :ref:`source_base` 


Class for calculation in a porous media with source term of Darcy -nu/K*V.
This keyword must be used with a permeability model.
For the moment there are two models : permeability constant or Ergun's law.
Darcy source term is available for quasi compressible calculation.
A new keyword is aded for porosity (porosite).

Usage:

**darcy** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) Description.


----

.. _dirac:

**dirac**
---------

**Inherits from:** :ref:`source_base` 


Class to define a source term corresponding to a volume power release in the energy 
equation.

Usage:

**dirac** **position** **ch**  

Parameters are:

- **position**  (*type:* list) not_set

- **ch**  (*type:* :ref:`field_base`) Thermal power field type. To impose a volume power on a domain sub-area, the Champ_Uniforme_Morceaux (partly_uniform_field)  type must be used.  Warning : The volume thermal power is expressed in W.m-3.


----

.. _dispersion_bulles:

**dispersion_bulles**
---------------------

**Inherits from:** :ref:`source_base` 


Base class for source terms of bubble dispersion in momentum equation.

Usage:

| **dispersion_bulles** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta]**  double
| }

Parameters are:

- **[beta]**  (*type:* double) Mutliplying factor for the output of the bubble dispersion source term.


----

.. _dp_impose:

**dp_impose**
-------------

**Inherits from:** :ref:`source_base` 


Source term to impose a pressure difference according to the formula : DP = dp + dDP/dQ 
* (Q - Q0)

Usage:

**dp_impose** **aco** **dp_type** **surface** **bloc_surface** **acof**  

Parameters are:

- **aco**  (*type:* string into ["{"])  Opening curly bracket.

- **dp_type**  (*type:* :ref:`type_perte_charge_deriv`) mass flow rate (kg/s).

- **surface**  (*type:* string into ["surface"])  not_set

- **bloc_surface**  (*type:* :ref:`bloc_lecture`) Three syntaxes are possible for the surface definition block:  For VDF and VEF: { X|Y|Z = location subzone_name }  Only for VEF: { Surface surface_name }.  For polymac { Surface surface_name Orientation champ_uniforme }.

- **acof**  (*type:* string into ["}"])  Closing curly bracket.


----

.. _flux_interfacial:

**flux_interfacial**
--------------------

**Inherits from:** :ref:`source_base` 


Source term of mass transfer between phases connected by the saturation object defined 
in saturation_xxxx

Usage:

**flux_interfacial**  


----

.. _forchheimer:

**forchheimer**
---------------

**Inherits from:** :ref:`source_base` 


Class to add the source term of Forchheimer -Cf/sqrt(K)*V2 in the Navier-Stokes equations.
We must precise a permeability model : constant or Ergun's law.
Moreover we can give the constant Cf : by default its value is 1.
Forchheimer source term is available also for quasi compressible calculation.
A new keyword is aded for porosity (porosite).

Usage:

**forchheimer** **bloc**  

Parameters are:

- **bloc**  (*type:* :ref:`bloc_lecture`) Description.


----

.. _frottement_interfacial:

**frottement_interfacial**
--------------------------

**Inherits from:** :ref:`source_base` 


Source term which corresponds to the phases friction at the interface

Usage:

| **frottement_interfacial** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[a_res]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[dv_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[exp_res]**  int
| }

Parameters are:

- **[a_res]**  (*type:* double) void fraction at which the gas velocity is forced to approach liquid velocity  (default alpha_evanescence*100)

- **[dv_min]**  (*type:* double) minimal relative velocity used to linearize interfacial friction at low  velocities

- **[exp_res]**  (*type:* int) exponent that callibrates intensity of velocity convergence (default 2)


----

.. _perte_charge_anisotrope:

**perte_charge_anisotrope**
---------------------------

**Inherits from:** :ref:`source_base` 


Anisotropic pressure loss.

Usage:

| **perte_charge_anisotrope** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **lambda_ortho**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diam_hydr**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  string
| }

Parameters are:

- **Lambda | lambda_u**  (*type:* string) Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).

- **lambda_ortho**  (*type:* string) Function for loss coefficient in transverse direction which may be Reynolds  dependant (Ex: 64/Re).

- **diam_hydr**  (*type:* :ref:`champ_don_base`) Hydraulic diameter value.

- **direction**  (*type:* :ref:`champ_don_base`) Field which indicates the direction of the pressure loss.

- **[sous_zone]**  (*type:* string) Optional sub-area where pressure loss applies.


----

.. _perte_charge_circulaire:

**perte_charge_circulaire**
---------------------------

**Inherits from:** :ref:`source_base` 


New pressure loss.

Usage:

| **perte_charge_circulaire** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **lambda_ortho**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diam_hydr**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diam_hydr_ortho**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  string
| }

Parameters are:

- **Lambda | lambda_u**  (*type:* string) Function f(Re_tot, Re_long, t, x, y, z) for loss coefficient in the longitudinal  direction

- **lambda_ortho**  (*type:* string) function: Function f(Re_tot, Re_ortho, t, x, y, z) for loss coefficient in  transverse direction

- **diam_hydr**  (*type:* :ref:`champ_don_base`) Hydraulic diameter value.

- **diam_hydr_ortho**  (*type:* :ref:`champ_don_base`) Transverse hydraulic diameter value.

- **direction**  (*type:* :ref:`champ_don_base`) Field which indicates the direction of the pressure loss.

- **[sous_zone]**  (*type:* string) Optional sub-area where pressure loss applies.


----

.. _perte_charge_directionnelle:

**perte_charge_directionnelle**
-------------------------------

**Inherits from:** :ref:`source_base` 


Directional pressure loss.

Usage:

| **perte_charge_directionnelle** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diam_hydr**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **direction**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  string
| }

Parameters are:

- **Lambda | lambda_u**  (*type:* string) Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).

- **diam_hydr**  (*type:* :ref:`champ_don_base`) Hydraulic diameter value.

- **direction**  (*type:* :ref:`champ_don_base`) Field which indicates the direction of the pressure loss.

- **[sous_zone]**  (*type:* string) Optional sub-area where pressure loss applies.


----

.. _perte_charge_isotrope:

**perte_charge_isotrope**
-------------------------

**Inherits from:** :ref:`source_base` 


Isotropic pressure loss.

Usage:

| **perte_charge_isotrope** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **diam_hydr**  :ref:`champ_don_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[sous_zone]**  string
| }

Parameters are:

- **Lambda | lambda_u**  (*type:* string) Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).

- **diam_hydr**  (*type:* :ref:`champ_don_base`) Hydraulic diameter value.

- **[sous_zone]**  (*type:* string) Optional sub-area where pressure loss applies.


----

.. _perte_charge_reguliere:

**perte_charge_reguliere**
--------------------------

**Inherits from:** :ref:`source_base` 


Source term modelling the presence of a bundle of tubes in a flow.

Usage:

**perte_charge_reguliere** **spec** **zone_name | name_of_zone**  

Parameters are:

- **spec**  (*type:* :ref:`spec_pdcr_base`) Description of longitudinale or transversale type.

- **zone_name | name_of_zone**  (*type:* string) Name of the sub-area occupied by the tube bundle. A Sous_Zone (Sub-area) type object called zone_name should have been previously created.


----

.. _perte_charge_singuliere:

**perte_charge_singuliere**
---------------------------

**Inherits from:** :ref:`source_base` 


Source term that is used to model a pressure loss over a surface area (transition 
through a grid, sudden enlargement) defined by the faces of elements located on the 
intersection of a subzone named subzone_name and a X,Y, or Z plane located at X,Y 
or Z = location.

Usage:

| **perte_charge_singuliere** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **dir**  string into ["kx", "ky", "kz", "k"]
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[coeff]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[regul]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **surface**  :ref:`bloc_lecture`
| }

Parameters are:

- **dir**  (*type:* string into ["kx", "ky", "kz", "k"])  KX, KY or KZ designate directional pressure loss  coefficients for respectively X, Y or Z direction. Or in the case where you chose a target flow rate with regul. Use K for isotropic pressure loss coefficient

- **[coeff]**  (*type:* double) Value (float) of friction coefficient (KX, KY, KZ).

- **[regul]**  (*type:* :ref:`bloc_lecture`) option to have adjustable K with flowrate target  { K0 valeur_initiale_de_k deb debit_cible eps intervalle_variation_mutiplicatif}.

- **surface**  (*type:* :ref:`bloc_lecture`) Three syntaxes are possible for the surface definition block:  For VDF and VEF: { X|Y|Z = location subzone_name }  Only for VEF: { Surface surface_name }.  For polymac { Surface surface_name Orientation champ_uniforme }


----

.. _portance_interfaciale:

**portance_interfaciale**
-------------------------

**Inherits from:** :ref:`source_base` 


Base class for source term of lift force in momentum equation.

Usage:

| **portance_interfaciale** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[beta]**  double
| }

Parameters are:

- **[beta]**  (*type:* double) Multiplying factor for the bubble lift force source term.


----

.. _puissance_thermique:

**puissance_thermique**
-----------------------

**Inherits from:** :ref:`source_base` 


Class to define a source term corresponding to a volume power release in the energy 
equation.

Usage:

**puissance_thermique** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`field_base`) Thermal power field type. To impose a volume power on a domain sub-area, the Champ_Uniforme_Morceaux (partly_uniform_field)  type must be used.  Warning : The volume thermal power is expressed in W.m-3 in 3D (in W.m-2 in 2D). It is a power per volume unit (in a porous media, it is a power per fluid volume  unit).


----

.. _radioactive_decay:

**radioactive_decay**
---------------------

**Inherits from:** :ref:`source_base` 


Radioactive decay source term of the form $-\lambda_i c_i$, where $0 \leq i \leq N$, 
N is the number of component of the constituent, $c_i$ and $\lambda_i$ are the concentration 
and the decay constant of the i-th component of the constituant.

Usage:

**radioactive_decay** **val**  

Parameters are:

- **val**  (*type:* list) n is the number of decay constants to read (int), and val1, val2... are the decay constants (double)


----

.. _source_base:

**source_base**
---------------


Basic class of source terms introduced in the equation.

Usage:

| **source_base** *str*
| **Read** *str* {
| }


----

.. _source_constituant:

**source_constituant**
----------------------

**Inherits from:** :ref:`source_base` 


Keyword to specify source rates, in [[C]/s], for each one of the nb constituents.
[C] is the concentration unit.

Usage:

**source_constituant** **ch**  

Parameters are:

- **ch**  (*type:* :ref:`field_base`) Field type.


----

.. _source_generique:

**source_generique**
--------------------

**Inherits from:** :ref:`source_base` 


to define a source term depending on some discrete fields of the problem and (or) 
analytic expression.
It is expressed by the way of a generic field usually used for post-processing.

Usage:

**source_generique** **champ**  

Parameters are:

- **champ**  (*type:* :ref:`champ_generique_base`) the source field


----

.. _source_pdf:

**source_pdf**
--------------

**Inherits from:** :ref:`source_base` 


Source term for Penalised Direct Forcing (PDF) method.

Usage:

| **source_pdf** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **aire**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **rotation**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[transpose_rotation]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **modele**  :ref:`bloc_pdf_model`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[interpolation]**  :ref:`interpolation_ibm_base`
| }

Parameters are:

- **aire**  (*type:* :ref:`field_base`) volumic field: a boolean for the cell (0 or 1) indicating if the obstacle  is in the cell

- **rotation**  (*type:* :ref:`field_base`) volumic field with 9 components representing the change of basis on cells  (local to global). Used for rotating cases for example.

- **[transpose_rotation]**  (*type:* flag) whether to transpose the basis change matrix.

- **modele**  (*type:* :ref:`bloc_pdf_model`) model used for the Penalized Direct Forcing

- **[interpolation]**  (*type:* :ref:`interpolation_ibm_base`) interpolation method


----

.. _source_pdf_base:

**source_pdf_base**
-------------------

**Inherits from:** :ref:`source_base` 


Base class of the source term for the Immersed Boundary Penalized Direct Forcing method 
(PDF)

Usage:

| **source_pdf_base** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **aire**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **rotation**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[transpose_rotation]**  flag
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **modele**  :ref:`bloc_pdf_model`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[interpolation]**  :ref:`interpolation_ibm_base`
| }

Parameters are:

- **aire**  (*type:* :ref:`field_base`) volumic field: a boolean for the cell (0 or 1) indicating if the obstacle  is in the cell

- **rotation**  (*type:* :ref:`field_base`) volumic field with 9 components representing the change of basis on cells  (local to global). Used for rotating cases for example.

- **[transpose_rotation]**  (*type:* flag) whether to transpose the basis change matrix.

- **modele**  (*type:* :ref:`bloc_pdf_model`) model used for the Penalized Direct Forcing

- **[interpolation]**  (*type:* :ref:`interpolation_ibm_base`) interpolation method


----

.. _source_qdm:

**source_qdm**
--------------

**Inherits from:** :ref:`source_base` 


Momentum source term in the Navier-Stokes equations.

Usage:

**source_qdm** **ch | champ**  

Parameters are:

- **ch | champ**  (*type:* :ref:`field_base`) Field type.


----

.. _source_qdm_lambdaup:

**source_qdm_lambdaup**
-----------------------

**Inherits from:** :ref:`source_base` 


This source term is a dissipative term which is intended to minimise the energy associated 
to non-conformscales u' (responsible for spurious oscillations in some cases).
The equation for these scales can be seen as: du'/dt= -lambda.
u' + grad P' where -lambda.
u' represents the dissipative term, with lambda = a/Delta t For Crank-Nicholson temporal 
scheme, recommended value for a is 2.

Remark : This method requires to define a filtering operator.

Usage:

| **source_qdm_lambdaup** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **Lambda | lambda_u**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[lambda_min]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[lambda_max]**  double
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[ubar_umprim_cible]**  double
| }

Parameters are:

- **Lambda | lambda_u**  (*type:* double) value of lambda

- **[lambda_min]**  (*type:* double) value of lambda_min

- **[lambda_max]**  (*type:* double) value of lambda_max

- **[ubar_umprim_cible]**  (*type:* double) value of ubar_umprim_cible


----

.. _source_robin:

**source_robin**
----------------

**Inherits from:** :ref:`source_base` 


This source term should be used when a Paroi_decalee_Robin boundary condition is set 
in a hydraulic equation.
The source term will be applied on the N specified boundaries.
To post-process the values of tauw, u_tau and Reynolds_tau into the files tauw_robin.dat, 
reynolds_tau_robin.dat and u_tau_robin.dat, you must add a block Traitement_particulier 
{ canal { } }

Usage:

**source_robin** **bords**  

Parameters are:

- **bords**  (*type:* :ref:`vect_nom`) not_set


----

.. _source_robin_scalaire:

**source_robin_scalaire**
-------------------------

**Inherits from:** :ref:`source_base` 


This source term should be used when a Paroi_decalee_Robin boundary condition is set 
in a an energy equation.
The source term will be applied on the N specified boundaries.
The values temp_wall_valueI are the temperature specified on the Ith boundary.
The last value dt_impr is a printing period which is mandatory to specify in the 
data file but has no effect yet.

Usage:

**source_robin_scalaire** **bords**  

Parameters are:

- **bords**  (*type:* :ref:`listdeuxmots_sacc`) not_set


----

.. _source_th_tdivu:

**source_th_tdivu**
-------------------

**Inherits from:** :ref:`source_base` 


This term source is dedicated for any scalar (called T) transport.
Coupled with upwind (amont) or muscl scheme, this term gives for final expression 
of convection : div(U.T)-T.div (U)=U.grad(T) This ensures, in incompressible flow 
when divergence free is badly resolved, to stay in a better way in the physical boundaries.

Warning: Only available in VEF discretization.

Usage:

**source_th_tdivu**  


----

.. _source_travail_pression_elem_base:

**source_travail_pression_elem_base**
-------------------------------------

**Inherits from:** :ref:`source_base` 


Source term which corresponds to the additional pressure work term that appears when 
dealing with compressible multiphase fluids

Usage:

**source_travail_pression_elem_base**  


----

.. _terme_puissance_thermique_echange_impose:

**terme_puissance_thermique_echange_impose**
--------------------------------------------

**Inherits from:** :ref:`source_base` 


Source term to impose thermal power according to formula : P = himp * (T - Text).
Where T is the Trust temperature, Text is the outside temperature with which energy 
is exchanged via an exchange coefficient himp

Usage:

| **terme_puissance_thermique_echange_impose** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **himp**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **text**  :ref:`field_base`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[pid_controler_on_targer_power]**  :ref:`bloc_lecture`
| }

Parameters are:

- **himp**  (*type:* :ref:`field_base`) the exchange coefficient

- **text**  (*type:* :ref:`field_base`) the outside temperature

- **[pid_controler_on_targer_power]**  (*type:* :ref:`bloc_lecture`) PID_controler_on_targer_power bloc with parameters target_power (required),  Kp, Ki and Kd (at least one of them should be provided)


----

.. _travail_pression:

**travail_pression**
--------------------

**Inherits from:** :ref:`source_base` 


Source term which corresponds to the additional pressure work term that appears when 
dealing with compressible multiphase fluids

Usage:

**travail_pression**  


----

.. _vitesse_derive_base:

**vitesse_derive_base**
-----------------------

**Inherits from:** :ref:`source_base` 


Source term which corresponds to the drift-velocity between a liquid and a gas phase

Usage:

**vitesse_derive_base**  


----

.. _vitesse_relative_base:

**vitesse_relative_base**
-------------------------

**Inherits from:** :ref:`source_base` 


Basic class for drift-velocity source term between a liquid and a gas phase

Usage:

**vitesse_relative_base**  


----

**Keywords derived from sources**
=================================

.. _sources:

**sources**
-----------


The sources.


----

**Keywords derived from sous_zone**
===================================

.. _sous_zone:

**sous_zone**
-------------

**Synonyms:** sous_domaine


It is an object type describing a domain sub-set.

A Sous_Zone (Sub-area) type object must be associated with a Domaine type object.
The Read (Lire) interpretor is used to define the items comprising the sub-area.

Caution: The Domain type object nom_domaine must have been meshed (and triangulated 
or tetrahedralised in VEF) prior to carrying out the Associate (Associer) nom_sous_zone 
nom_domaine instruction; this instruction must always be preceded by the read instruction.

Usage:

| **sous_zone** *str*
| **Read** *str* {
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[restriction]**  :ref:`sous_zone`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[rectangle]**  :ref:`bloc_origine_cotes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[segment]**  :ref:`bloc_origine_cotes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[boite | box]**  :ref:`bloc_origine_cotes`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[liste]**  int list
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fichier | filename]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[intervalle]**  :ref:`deuxentiers`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[polynomes]**  :ref:`bloc_lecture`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[couronne]**  :ref:`bloc_couronne`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[tube]**  :ref:`bloc_tube`
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[fonction_sous_zone | fonction_sous_domaine]**  string
| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| |nbsp| **[union | union_with]**  :ref:`sous_zone`
| }

Parameters are:

- **[restriction]**  (*type:* :ref:`sous_zone`) The elements of the sub-area nom_sous_zone must be included into the  other sub-area named nom_sous_zone2. This keyword should be used first in the Read keyword.

- **[rectangle]**  (*type:* :ref:`bloc_origine_cotes`) The sub-area will include all the domain elements whose centre  of gravity is within the Rectangle (in dimension 2).

- **[segment]**  (*type:* :ref:`bloc_origine_cotes`) not_set

- **[boite | box]**  (*type:* :ref:`bloc_origine_cotes`) The sub-area will include all the domain elements whose centre  of gravity is within the Box (in dimension 3).

- **[liste]**  (*type:* int list) The sub-area will include n domain items, numbers No. 1 No. i No. n.

- **[fichier | filename]**  (*type:* string) The sub-area is read into the file filename.

- **[intervalle]**  (*type:* :ref:`deuxentiers`) The sub-area will include domain items whose number is between n1 and  n2 (where n1<=n2).

- **[polynomes]**  (*type:* :ref:`bloc_lecture`) A REPRENDRE

- **[couronne]**  (*type:* :ref:`bloc_couronne`) In 2D case, to create a couronne.

- **[tube]**  (*type:* :ref:`bloc_tube`) In 3D case, to create a tube.

- **[fonction_sous_zone | fonction_sous_domaine]**  (*type:* string) Keyword to build a sub-area with the the elements included into the area  defined by fonction>0.

- **[union | union_with]**  (*type:* :ref:`sous_zone`) The elements of the sub-area nom_sous_zone3 will be added to the sub-area  nom_sous_zone. This keyword should be used last in the Read keyword.


----

**Keywords derived from turbulence_paroi_base**
===============================================

.. _turbulence_paroi_base:

**turbulence_paroi_base**
-------------------------


Basic class for wall laws for Navier-Stokes equations.

Usage:

| **turbulence_paroi_base** *str*
| **Read** *str* {
| }


----

**Keywords derived from turbulence_paroi_scalaire_base**
========================================================

.. _turbulence_paroi_scalaire_base:

**turbulence_paroi_scalaire_base**
----------------------------------


Basic class for wall laws for energy equation.

Usage:

| **turbulence_paroi_scalaire_base** *str*
| **Read** *str* {
| }


----

**Keywords derived from vect_nom**
==================================

.. _vect_nom:

**vect_nom**
------------


Vect of name.

