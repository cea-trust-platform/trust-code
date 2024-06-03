Problem definition
==================

Set of Equations
----------------

Depending on your choosed problem type, you will have a different set of equations.

Here is a summary of some selected problems. For documentation and for complete problem sets, see the `TRUST Reference Manual <https://github.com/cea-trust-platform/trust-code/blob/master/doc/TRUST/TRUST_Reference_Manual.pdf>`__.

Incompressible problems
~~~~~~~~~~~~~~~~~~~~~~~

**TRUST** solves Navier-Stokes equations with/without the heat equation for an incompressible fluid:

.. math::

   \left\{
   \begin{array}{c}
   \nabla \cdot \vec u =0 \\
   \displaystyle{\frac{\partial \vec u }{\partial t} + \textcolor{red}{\nabla \cdot (\vec u \otimes \vec u)} = \textcolor{blue}{\nabla \cdot (\nu \nabla \vec u)} - \nabla P^* } \\
   \displaystyle{\frac{\partial T}{\partial t} + \textcolor{red}{\vec u \nabla T} = \textcolor{blue}{\nabla \cdot (\alpha \nabla T)} + \frac{Q}{\rho C_p}}
   \end{array}
   \right.

where: :math:`\displaystyle{P^*=\frac{P}{\rho} + g z}`, :math:`Q` is the
heat source term, and:

-  :math:`\rho`: density,

-  :math:`\mu`: dynamic viscosity,

-  :math:`\displaystyle{\nu=\frac{\mu}{\rho}}`: kinematic viscosity,

-  :math:`\vec g=g z`: gravity vector in cartesian coordinates,

-  :math:`\displaystyle{\alpha=\frac{\lambda}{\rho C_p}}`: thermal
   diffusivity.

-  :math:`C_p`: specific heat capacity at constant pressure,

-  :math:`\lambda`: thermal conductivity,

.. note::

   Red terms are convective terms and blue terms are diffusive terms.

In your data file, you will have:

.. code-block:: bash

   Pb_Thermohydraulique_Concentration my_problem
   ...
   Read my_problem
   {
      # Define medium and its properties + gravity if any #
      Fluide_incompressible { ... }

      # Navier Stokes equations #
      Navier_Stokes_Standard
      {
         Solveur_Pression my_solver { ... }
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }

      # Energy equation #
      Convection_Diffusion_Temperature
      {
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }

      # Constituent transportation equations #
      Convection_Diffusion_Concentration
      {
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }
   }

Quasi-Compressible problem
~~~~~~~~~~~~~~~~~~~~~~~~~~

**TRUST** solves Navier-Stokes equations with/without heat equation for quasi-compressible fluid:

.. math::

   \left\{
   \begin{array}{c}
   \displaystyle{\frac{\partial \rho }{\partial t} + \nabla \cdot (\rho \vec u) =0 }\\
   \displaystyle{ \frac{\partial \rho \vec u}{\partial t} + \textcolor{red}{\nabla \cdot (\rho \vec u \vec u)} =  \textcolor{blue}{\nabla \cdot \left(\mu \nabla \vec u \right)} - \nabla P -\rho \vec g }\\
   \displaystyle{ \rho C_p \left( \frac{\partial T}{\partial t} + \textcolor{red}{\vec u \nabla T} \right) = \textcolor{blue}{\nabla \cdot \left(\lambda \nabla T\right)} + \frac{dP_0}{dt} + Q }
   \end{array}
   \right.

where: :math:`P_0=\rho R T`, :math:`Q` is a heat source term, and:

-  :math:`\rho`: density,

-  :math:`\mu`: dynamic viscosity,

-  :math:`\vec g=g z`: gravity vector in cartesian coordinates,

-  :math:`C_p`: specific heat capacity at constant pressure,

-  :math:`\lambda`: thermal conductivity.

.. note::

   Red terms are convective terms and blue terms are diffusive terms.

In your data file, you will have:

.. code-block:: bash

   Pb_Thermohydraulique_QC my_problem
   ...
   Read my_problem
   {
      # Define medium and its properties + gravity if any #
      Fluide_Quasi_compressible { ... }

      # Navier Stokes equations for quasi-compressible fluid under low Mach numbers #
      Navier_Stokes_Turbulent_QC
      {
         Solveur_Pression my_solver { ... }
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }

      # Energy equation for quasi-compressible fluid under low Mach numbers #
      Convection_Diffusion_Chaleur_QC
      {
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }
   }

Weakly-Compressible problem
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**TRUST** solves Navier-Stokes equations with/without heat equation for weakly-compressible fluid:

.. math::

   \left\{
   \begin{array}{c}
   \displaystyle{\frac{\partial \rho }{\partial t} + \nabla \cdot (\rho \vec u) =0 }\\
   \displaystyle{ \frac{\partial \rho \vec u}{\partial t} + \textcolor{red}{\nabla \cdot (\rho \vec u \vec u)} =  \textcolor{blue}{\nabla \cdot \left(\mu \nabla \vec u \right)} - \nabla P -\rho \vec g }\\
   \displaystyle{ \rho C_p \left( \frac{\partial T}{\partial t} + \textcolor{red}{\vec u \nabla T} \right) = \textcolor{blue}{\nabla \cdot \left(\lambda \nabla T\right)} + \frac{dP_{tot}}{dt} + Q }
   \end{array}
   \right.

where: :math:`P_{tot}=\rho R T`, :math:`Q` is a heat source term, and:

-  :math:`\rho`: density,

-  :math:`\mu`: dynamic viscosity,

-  :math:`\vec g=g z`: gravity vector in cartesian coordinates,

-  :math:`C_p`: specific heat capacity at constant pressure,

-  :math:`\lambda`: thermal conductivity.

.. note::

   Red terms are convective terms and blue terms are diffusive terms.

In your data file, you will have:

.. code-block:: bash

   Pb_Thermohydraulique_WC my_problem
   ...
   Read my_problem
   {
      # Define medium and its properties + gravity if any #
      Fluide_Weakly_compressible { ... }

      # Navier Stokes equations for weakly-compressible fluid under low Mach numbers #
      Navier_Stokes_Turbulent_WC
      {
         Solveur_Pression my_solver { ... }
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }

      # Energy equation for weakly-compressible fluid under low Mach numbers #
      Convection_Diffusion_Chaleur_WC
      {
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }
   }

Conduction problem
~~~~~~~~~~~~~~~~~~

For this kind of problems, **TRUST** solves the heat equation:

.. math:: \rho C_p \frac{\partial T}{\partial t} = \textcolor{blue}{\nabla \cdot \left(\lambda \nabla T\right)} + Q

where:

-  :math:`\rho`: density,

-  :math:`C_p`: specific heat capacity at constant pressure,

-  :math:`\lambda`: thermal conductivity,

-  :math:`Q` is a heat source term.

.. note::
   The term in blue is the diffusive term.

In your data file, you will have:

.. code-block:: bash

   Pb_Conduction my_problem
   ...
   Read my_problem
   {
      # Define medium and its properties #
      Solide { ... }

      # Resolution of the heat equation #
      Conduction
      {
         Diffusion { ... }
         Convection { ... }
         Initial_conditions { ... }
         Boundary_conditions { ... }
         Sources { ... }
         ...
      }
   }

Coupled problems
~~~~~~~~~~~~~~~~

With **TRUST**, we can couple problems. We will explain here the method for two problems but you can couple as many problems as you want.

To couple two problems, we define two problems *my_problem_1* and *my_problem_2* each one associated to a separate domain *my_domain_1* and *my_domain_2*, and to a separate medium *my_medium_1* and *my_medium_2* (associated or not to the gravity).

.. code-block:: bash

   Dimension 2
   Pb_ThermoHydraulique my_problem_1
   Pb_ThermoHydraulique my_problem_2

   Domaine my_domain_1
   Read_file my_mesh_1.geo ;

   Domaine my_domain_2
   Read_file my_mesh_2.geo ;

   Associate my_problem_1 my_domain_1
   Associate my_problem_2 my_domain_2

Then we define a coupled problem associated to a single time scheme like for example:

.. code-block:: bash

   Probleme_Couple my_coupled_problem

   VEFPreP1B my_discretization

   Scheme_euler_explicit my_scheme
   Read my_scheme { ... }

   Associate my_coupled_problem my_problem_1
   Associate my_coupled_problem my_problem_2
   Associate my_coupled_problem my_scheme

Then we discretize and solve everything:

.. code-block:: bash

   Discretize my_coupled_problem my_discretization

   Read my_problem_1
   {
      Fluide_Incompressible { ... }
      ...
   }

   Read my_problem_2
   {
      Fluide_Incompressible { ... }
      ...
   }

   Solve my_coupled_problem
   End

You can see the documentation of this kind of problem in the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#coupledproblem>`__.

Pressure Solvers
----------------

Then you may indicate the choice of pressure solver using the following syntax (see the `Solvers <https://cea-trust-platform.github.io/classes/solvers>`__ section on the TRUST's website)

.. code-block:: bash
   
   Solveur_pression my_solver { ... }

The *my_solver* may be:

-  **GCP** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#solvgcp>`__).

-  **PETSc PETSc_solver_name** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#petsc>`__).

-  **Cholesky** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#cholesky>`__).

-  **Gmres** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#gmres>`__).

-  **Gen** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#gen>`__).

Reminder: in CFD, a separate solver is used to solve the pressure. For more details, you can have a look at the section **Time and space schemes** of the **TRUST**\ & **TrioCFD** user slides.

Convection
----------

There is no default convective scheme so you must choose a specific scheme and specify as follows:

.. code-block:: bash

   convection { convective_scheme }

Have a look at the `Spatial Schemes <https://cea-trust-platform.github.io/classes/spatial-schemes>`__ section for a list of schemes available in the TRUST platform.

In short, you can use the following convective scheme, following the recommendations of the user training session (see section **Time and space schemes** of the **TRUST**\ & **TrioCFD** user slides and the section **Recommendations for schemes**) following your discretization type:

-  **Amont** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#convectionamont>`__).

-  **Muscl** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#convectionmuscl>`__).

-  **EF_stab** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#convectionefstab>`__).

.. note::

   There is no default convective scheme and if you don’t want convection in your problem, you may use:

   .. code-block:: bash

      convection { negligeable }

Diffusion
---------

The diffusion term is more or less a Laplacien operator and is thus always discretized by a second order centered difference scheme. So you just need to do this:

.. code-block:: bash

   diffusion { }

.. note:: 

   If you don’t want diffusion in your problem, you may use:

   .. code-block:: bash

      diffusion { negligeable }

Initial Conditions
------------------

For each equation, you **must** set initial conditions:

.. code-block:: bash

   initial_conditions { ... }

See the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#condinits>`__ to see the syntax of each available initial condition. Here are the most used initial conditions:

-  **Vitesse** field_type *bloc_lecture_champ*

-  **Temperature** field_type *bloc_lecture_champ*

We list here some "field_type":

-  **Uniform_Field** for a uniform field (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#uniformfield>`__).

-  **Champ_Fonc_Med** to read a data field in a MED-format file .med at a specified time (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#champfoncmed>`__).

-  **Champ_Fonc_txyz** for a field which depends on time and space (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#fieldfunctxyz>`__).

-  **Champ_Fonc_Fonction_txyz** for a field which is a function of another field and time and/or space coordinates (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#champfoncfonctiontxyz>`__).

-  **Champ_Fonc_Reprise** to read a data field in a saved file (.xyz or .sauv) at a specified time (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#champfoncreprise>`__).

Boundary Conditions
-------------------

Then you may specify your boundary conditions like:

.. code-block:: bash

   boundary_conditions { ... }

It is important to specify here that **TRUST will not accept any boundary conditions by default.**

You can find help for boundary conditions in the `Boundary Conditions <https://cea-trust-platform.github.io/classes/boundary-conditions>`__ section on the TRUST's website. 

Here is a list of the most used boundary conditions:

-  Bord **Frontiere_ouverte_vitesse_imposee** boundary_field_type *bloc_lecture_champ* (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#frontiereouvertevitesseimposee>`__).

-  Bord **Frontiere_ouverte_pression_imposee** boundary_field_type *bloc_lecture_champ* (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#frontiereouvertepressionimposee>`__).

-  Bord **Paroi_fixe** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#paroifixe>`__).

-  Bord **Symetrie** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#symetrie>`__).

-  Bord **Periodique** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#periodic>`__).

-  Bord **Frontiere_ouverte_temperature_imposee** boundary_field_type *bloc_lecture_champ* (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#frontiereouvertetemperatureimposee>`__).

-  Bord **Frontiere_ouverte T_ext** boundary_field_type *bloc_lecture_champ* (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#frontiereouverte>`__).

-  Bord **Paroi_adiabatique** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#paroiadiabatique>`__).

-  Bord **Paroi_flux_impose** boundary_field_type *bloc_lecture_champ* (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#paroifluximpose>`__).

To choose your *boundary_field_type* parameters, refer to the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#condlimbase>`__.

Source Terms
------------

To introduce a source term into an equation, add the following line into the block defining the equation. The list of source keyword is described below.

.. code-block:: bash

   Sources { source_keyword }

To introduce several source terms into the same equation, the blocks corresponding to the various terms need to be separated by a comma:

.. code-block:: bash

   Sources { source_keyword1 , source_keyword2 , ... }

Here are some available source terms. For a complete list, refer to the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#sourcebase>`__.

-  **Perte_Charge_Reguliere** type_perte_charge bloc_definition_pertes_charges (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#pertechargereguliere>`__).

-  **Perte_Charge_Singuliere** **KX \| KY \| KZ** coefficient_value { ... } (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#pertechargesinguliere>`__).

-  **Canal_perio** { ... } (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#canalperio>`__).

-  **Boussinesq_temperature** { ... } (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#boussinesqtemperature>`__).

   .. note::

      Defined as :math:`\rho(T)=\rho(T_0)(1-\beta_{th}(T-T_0))`

-  **Boussinesq_concentration** { ... } (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#boussinesqconcentration>`__).

-  **Puissance_thermique** field_type bloc_lecture_champ (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#puissancethermique>`__).

Post-Processings
----------------

Before post-processing fields, during a run, **TRUST** creates several files which contain information about the calculation, the convergence, fluxes, balances... See section :ref:`Output files` for more information.

Several keywords can be used to create a post-processing block, into a problem. First, you can create a single post-processing task (**Post_processing** keyword). Generally, in this block, results will be printed with a specified format at a specified time period.

.. code-block:: bash

   Post_processing
   {
      Postraitement_definition
      ...
   }

But you can also create a list of post-processings with **Post_processings** keyword (named with Post_name1, Post_name2, etc...), in order to print results into several formats or with different time periods, or into different results files:

.. code-block:: bash

   Post_processings
   {
      Post_name1 { Postraitement_definition }
      Post_name2 { Postraitement_definition }
      ...
   }

Have a look at the `Post-Processing <https://cea-trust-platform.github.io/classes/post-processing>`__ section on the TRUST's website.

Field names
~~~~~~~~~~~

-  **Existing & predefined fields**

   You can post-process predefined fields and already existing fields. Here is a list of post-processable fields, but it is not the only ones.

   +--------------------------------------------+----------------------------------+----------------+
   |                                            |                                  |                |
   +============================================+==================================+================+
   | **Physical values**                        | **Keyword for field_name**       | **Unit**       |
   +--------------------------------------------+----------------------------------+----------------+
   | Velocity                                   | Vitesse or Velocity              | m.s−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Velocity residual                          | Vitesse_residu                   | m.s−2          |
   +--------------------------------------------+----------------------------------+----------------+
   | Kinetic energy per elements                | Energie\_cinetique\_elem         | kg.m−1.s−2     |
   +--------------------------------------------+----------------------------------+----------------+
   | Total kinetic energy                       | Energie\_cinetique\_totale       | kg.m−1.s−2     |
   +--------------------------------------------+----------------------------------+----------------+
   | Vorticity                                  | Vorticite                        | s−1            |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure in incompressible flow (P/ρ + gz) | Pression                         | Pa.m3.kg−1     |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure in incompressible flow (P+ρgz)    | Pression_pa or Pressure          | Pa             |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure in compressible flow              | Pression                         | Pa             |
   +--------------------------------------------+----------------------------------+----------------+
   | Hydrostatic pressure (ρgz)                 | Pression\_hydrostatique          | Pa             |
   +--------------------------------------------+----------------------------------+----------------+
   | Totale pressure                            | Pression_tot                     | Pa             |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure gradient                          | Gradient_pression                | m.s−2          |
   +--------------------------------------------+----------------------------------+----------------+
   | Velocity gradient                          | gradient_vitesse                 | s−1            |
   +--------------------------------------------+----------------------------------+----------------+
   | Temperature                                | Temperature                      | C or K         |
   +--------------------------------------------+----------------------------------+----------------+
   | Temperature residual                       | Temperature_residu               | C.s−1 or K.s−1 |
   +--------------------------------------------+----------------------------------+----------------+
   | Temperature variance                       | Variance_Temperature             | K2             |
   +--------------------------------------------+----------------------------------+----------------+
   | Temperature dissipation rate               | Taux\_Dissipation\_Temperature   | K2.s−1         |
   +--------------------------------------------+----------------------------------+----------------+
   | Temperature gradient                       | Gradient_temperature             | K.m−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Heat exchange coefficient                  | H\_echange\_Tref                 | W.m−2.K−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Turbulent viscosity                        | Viscosite_turbulente             | m2.s−1         |
   +--------------------------------------------+----------------------------------+----------------+
   | Turbulent dynamic viscosity                | Viscosite\_dynamique\_turbulente | kg.m.s−1       |
   +--------------------------------------------+----------------------------------+----------------+
   | Turbulent kinetic                          | Energy                           | K m2.s−2       |
   +--------------------------------------------+----------------------------------+----------------+
   | Turbulent dissipation rate                 | Eps                              | m3.s−1         |
   +--------------------------------------------+----------------------------------+----------------+
   | Constituent concentration                  | Concentration                    | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Constituent concentration residual         | Concentration_residu             | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Component velocity along X                 | VitesseX                         | m.s−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Component velocity along Y                 | VitesseY                         | m.s−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Component velocity along Z                 | VitesseZ                         | m.s−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Mass balance on each cell                  | Divergence_U                     | m3.s−1         |
   +--------------------------------------------+----------------------------------+----------------+
   | Irradiancy                                 | Irradiance                       | W.m−2          |
   +--------------------------------------------+----------------------------------+----------------+
   | Q-criteria                                 | Critere_Q                        | s−1            |
   +--------------------------------------------+----------------------------------+----------------+
   | Distance to the wall Y +                   | Y_plus                           | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Friction velocity                          | U_star                           | m.s−1          |
   +--------------------------------------------+----------------------------------+----------------+
   | Void fraction                              | Alpha                            | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Cell volumes                               | Volume_maille                    | m3             |
   +--------------------------------------------+----------------------------------+----------------+
   | Source term in non Galinean referential    | Acceleration\_terme\_source      | m.s−2          |
   +--------------------------------------------+----------------------------------+----------------+
   | Stability time steps                       | Pas\_de\_temps                   | s              |
   +--------------------------------------------+----------------------------------+----------------+
   | Volumetric porosity                        | Porosite_volumique               | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Distance to the wall                       | Distance_Paroi                   | m              |
   +--------------------------------------------+----------------------------------+----------------+
   | Volumic thermal power                      | Puissance_volumique              | W.m−3          |
   +--------------------------------------------+----------------------------------+----------------+
   | Local shear strain rate                    | Taux_cisaillement                | s−1            |
   +--------------------------------------------+----------------------------------+----------------+
   | Cell Courant number (VDF only)             | Courant_maille                   | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Cell Reynolds number (VDF only)            | Reynolds_maille                  | -              |
   +--------------------------------------------+----------------------------------+----------------+
   | Viscous force                              | Viscous_force                    | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure force                             | Pressure_force                   | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Total force                                | Total_force                      | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Viscous force along X                      | Viscous\_force\_x                | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Viscous force along Y                      | Viscous\_force\_y                | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Viscous force along Z                      | Viscous\_force\_z                | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure force along X                     | Pressure\_force\_x               | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure force along Y                     | Pressure\_force\_y               | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Pressure force along Z                     | Pressure\_force\_z               | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Total force along X                        | Total\_force\_x                  | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Total force along Y                        | Total\_force\_y                  | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+
   | Total force along Z                        | Total\_force\_z                  | kg.m2.s−1      |
   +--------------------------------------------+----------------------------------+----------------+

   .. note::

       Physical properties (conductivity, diffusivity,...) can also be post-processed.

   .. note::

      The name of the fields and components available for post-processing is displayed in the error file after the following message: "Reading of fields to be postprocessed". Of course, this list depends of the problem being solved.

-  **Creating new fields**

   The **Definition_champs** keyword is used to create new or more complex fields for advanced post-processing.

   .. code-block:: bash

      Definition_champs { field_name_post field_type { ... } }

   *field_name_post* is the name of the new created field and **field_type** is one of the following possible type:

   -  **refChamp** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#refchamp>`__).

   -  **Reduction_0D** using for example the **min**, **max** or **somme** methods (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#reduction0d>`__).

   -  **Transformation** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#transformation>`__).

  Refer to the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#definitionchamps>`__ for more information.

   .. note::

      You can combine several **field_type** keywords to create your field and then use your new fields to create other ones.
   
   Here is an example of new field named *max_temperature*:

   .. code-block:: bash

      Read my_problem 
      {
         ...
         Postraitement 
         {
            Definition_champs 
            {
               # Creation of a 0D field: maximal temperature of the domain #
               max_temperature Reduction_0D 
               {
                  methode max
                  source refChamp { Pb_champ my_problem temperature }
               }
            }

            Probes 
            {
               # Print max(temperature) into the datafile_TMAX.son file #
               tmax max_temperature periode 0.01 point 1 0. 0.
            }

            Champs dt_post 1.0 { ... }
         }
      }


You can find other examples in the **TRUST**\ & **TrioCFD** user slides in the section "Post processing description".

Post-processing blocks
~~~~~~~~~~~~~~~~~~~~~~

There are three methods to post-process in **TRUST**: using probes, fields or making statistics.

-  **Probes**
   
   Probes refer to sensors that allow a value or several points of the domain to be monitored over time. The probes are a set of points defined:

   -  one by one: **Points** keyword
   
      or

   -  by a set of points evenly 

      - distributed over a straight segment: **Segment** keyword 

         or

      -  arranged according to a layout: **Plan** keyword 

         or

      -  arranged according to a parallelepiped **Volume** keyword.

   Here is an example of 2D **Probes** block:

   .. code-block:: bash

      Probes 
      {
         pressure_probe [loc] pressure Periode 0.5 Points 3 1. 0. 1. 1. 1. 2.
         velocity_probe [loc] velocity Periode 0.5 Segment 10 1. 0. 1. 4.
      }

   where the use of *loc* option allow to specify the wanted location of the probes. The available values are **grav** for gravity center of the element, **nodes** for faces and **som** for vertices. There is not default location. If the point does not coincide with a alculation node, the value is extrapolated linearly according to neighbouring node values.

   For complete syntax, see the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#corpspostraitement>`__.

-  **Fields**

   This keyword allows to post-process fields on the whole domain, specifying the name of the backup file, its format, the post-processing time step and the name (and location) of the post-processed fields.

   Here is an example of **Fields** block:

   .. code-block:: bash

      Fichier results
      Format lata
      Fields dt_post 1. 
      {
         velocity [faces] [som] [elem]
         pressure [elem] [som]
         temperature [elem] [som]
      }

   where **faces** , **elem** and **som** are keywords allowed to specify the location of the field.

   .. note::

       When you don’t specify the location of the field, the default value is **som** for values at the vertices. So fields are post-processed at the vertices of the mesh.

   To visualize your post-processed fields, you can use open source softwares like:

     `VisIt <https://wci.llnl.gov/simulation/computer-codes/visit>`__ (included in **TRUST** package) or `SALOME <http://www.salome-platform.org>`__.

  For complete syntax, see the `TRUST Reference Manual <https://github.com/cea-trust-platform/trust-code/blob/master/doc/TRUST/TRUST_Reference_Manual.pdf>`__.

-  **Statistics**

   Using this keyword, you will compute statistics on your unknows. You must specify the begining and ending time for the statistics, the post-processing time step, the statistic method, the name (and ocation) of your post-processed field.

   Here is an example of **Statistiques** block:

   .. code-block:: bash

      Statistiques dt_post 0.1 
      {
         t_deb 1. t_fin 5.
         moyenne velocity [faces] [elem] [som]
         ecart_type pressure [elem] [som]
         correlation pressure velocity [elem] [som]
      }

   This block will write at every **dt_post** the average of the velocity :math:`\overline{V(t)}`:

   .. math::

      \overline{V(t)}=\left\{ \begin{array}{ll}
      0 & ,\mbox{ for }t\leq t_{deb}\\
      \frac{1}{t-t_{deb}}{\displaystyle \int_{t_{deb}}^{t}V(t)dt} & ,\mbox{ for }t_{deb}<t\leq t_{fin}\\
      \frac{1}{t_{fin}-t_{deb}}{\displaystyle \int_{t_{deb}}^{t_{fin}}V(t)dt} & ,\mbox{ for }t>t_{fin}
      \end{array}\right.

   the standard deviation of the pressure :math:`\left\langle P(t)\right\rangle`:

   .. math::

      \left\langle P(t)\right\rangle=\left\{ \begin{array}{ll}
      0 & ,\mbox{ for }t\leq t_{deb}\\
      \frac{1}{t-t_{deb}}{\displaystyle \sqrt{\int_{t_{deb}}^{t}\left[P(t)-\overline{P(t)}\right]^{2}dt}} & ,\mbox{ for }t_{deb}<t\leq t_{fin}\\
      \frac{1}{t_{fin}-t_{deb}}{\displaystyle \sqrt{\int_{t_{deb}}^{t_{fin}}\left[P(t)-\overline{P(t)}\right]^{2}dt}} & ,\mbox{ for }t>t_{fin}
      \end{array}\right.

   and correlation between the pressure and the velocity :math:`\left\langle P(t).V(t)\right\rangle` like:

   .. math::

      \left\langle P(t).V(t)\right\rangle=\left\{ \begin{array}{ll}
      0 & ,\mbox{ for }t\leq t_{deb}\\
      \frac{1}{t-t_{deb}}{\displaystyle \int_{t_{deb}}^{t}\left[P(t)-\overline{P(t)}\right]\cdot\left[V(t)-\overline{V(t)}\right]dt} & ,\mbox{ for }t_{deb}<t\leq t_{fin}\\
      \frac{1}{t_{fin}-t_{deb}}{\displaystyle \int_{t_{deb}}^{t_{fin}}\left[P(t)-\overline{P(t)}\right]\cdot\left[V(t)-\overline{V(t)}\right]dt} & ,\mbox{ for }t>t_{fin}
      \end{array}\right.

   **Remark:** Statistical fields can be plotted with probes with the keyword "operator_field_name" like for example: Moyenne_Vitesse or Ecart_Type_Pression or Correlation_Vitesse_Vitesse. For that, it is mandatory to have the statistical calculation of this fields defined with the keyword **Statistiques**.

   For complete syntax, see the `TRUST Reference Manual <https://github.com/cea-trust-platform/trust-code/blob/master/doc/TRUST/TRUST_Reference_Manual.pdf>`__.

Post-process location
~~~~~~~~~~~~~~~~~~~~~

You can use location keywords to specify where you want to post-process your fields in order to avoid interpolations on your post-processed fields.

For that, recall the variables localisation from the `Discretizations <https://cea-trust-platform.github.io/classes/discretizations>`__ section available the TRUST's website. 

.. note::

   If you are in P0+P1 discretization (default option) and you post-process the pressure field at the element (or at the vertices), you will have an **interpolation** because the field is computed at the element **and** at the vertices.

.. note::

   Non-main variables (like the viscosity, conductivity, cp, density, y+, ... ) are always located at the element gravity center.
