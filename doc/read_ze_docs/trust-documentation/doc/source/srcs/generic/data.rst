Data setting
============

We will now explain how to fill a data file. First you must specify some basic information like the dimension of your domain, its name, the problem type... 

To define the problem dimension, we use the following keyword:

.. code-block:: bash

   Dimension 2

or

.. code-block:: bash

   Dimension 3

Problems
--------

You have to define the problem type that you wish to solve.

.. code-block:: bash
   
   Pb_type my_problem

You can find all the available **TRUST** problems via `this link <https://cea-trust-platform.github.io/classes/problems>`__.

.. https://cea-trust-platform.github.io/classes/problems

Here are some of the available **TRUST** problem types.

-  for Incompressible flow: **Pb\_[Thermo]Hydraulic[_Concentration]**

-  for Quasi-Compressible flow: **Pb_Thermohydraulique_QC**

-  for Weakly-Compressible flow: **Pb_Thermohydraulique_WC**

-  for Multi-Phase flow: **Pb_Multiphase**

-  for solid: **Pb_Conduction**

where:

-  **hydraulique**: means that we will solve Navier-Stokes equations without energy equation.

-  **Thermo**: means that we will solve Navier-Stokes equations with energy equation.

-  **Concentration**: that we will solve multiple constituent transportation equations.

-  **Conduction**: resolution of the heat equation.

-  **QC**: Navier-Stokes equations with energy equation for quasi-compressible fluid under low Mach approach.

-  **WC**: Navier-Stokes equations with energy equation for weakly-compressible fluid under low Mach approach.


Domain Definition
-----------------

To define the domain, you must name it. This is done thanks to the following block:

.. code-block:: bash
   
   Domaine my_domain

Then you must add your mesh to your simulation.

Mesh
----

Notice the presence of the tags

.. code-block:: bash

   # BEGIN MESH #
   ...
   # END MESH #


in the data file of section :ref:`dataset`. This is useful for parallel calculation if well placed in datafile (see section :ref:`Parallel calculation`).

Allowed meshes
~~~~~~~~~~~~~~

**TRUST** allows all types of meshes if the appropriate spatial discretization is used. See the `Discretizations <https://cea-trust-platform.github.io/classes/discretizations>`__ section on the TRUST's website.

.. https://cea-trust-platform.github.io/classes/discretizations

Import a mesh file
~~~~~~~~~~~~~~~~~~

If your mesh was generated with an external tool like `SALOME <http://www.salome-platform.org>`__ (open source software), `ICEM <http://resource.ansys.com/Products/Other+Products/ANSYS+ICEM+CFD>`__ (commercial software), `Gmsh <http://gmsh.info/>`__ (open source software, included in **TRUST** package) or `Cast3M <http://www-cast3m.cea.fr/>`__ (CEA software), then you must use one of the following keywords into your data file:

-  **Read_MED** for a MED file from SALOME or Gmsh.

-  **Read_File** for a binary mesh file from ICEM. 

-  for another format, see the `TRUST Reference Manual <https://github.com/cea-trust-platform/trust-code/blob/master/doc/TRUST/TRUST_Reference_Manual.pdf>`__.

If you want to learn how to build a mesh with SALOME or Gmsh and read it with **TRUST**, you can look at the exercises of the TRUST Tutorial; `Exo Salome <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_tutorial.pdf#salome>`__ and `Exo Gmsh <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_tutorial.pdf#gmsh>`__.

You can have a look too at the `Pre-Processing <https://cea-trust-platform.github.io/classes/pre-processing>`__ section of the TRUST's website.

Quickly create a mesh
~~~~~~~~~~~~~~~~~~~~~

Here is an example of a simple geometry (of non complex channel type) using the internal tool of **TRUST**:

.. code-block:: bash

   Mailler my_domain
   {
      # Define the domain with one cavity #
      # cavity 1m*2m with 5*22 cells #
      Pave box
      {
         Origine 0. 0.
         Longueurs 1 2

         # Cartesian grid #
         Nombre_de_Noeuds 6 23

         # Uniform mesh #
         Facteurs 1. 1.
      }
      {
         # Definition and names of boundary conditions #
         bord Inlet  X = 0.   0. <= Y <= 2.
         bord Outlet X = 1.   0. <= Y <= 2.
         bord Upper  Y = 2.   0. <= X <= 1.
         bord Lower  Y = 0.   0. <= X <= 1.
      }
   }

To use this mesh in your data file, you just have to add the previous block in your data file or save it in a file named for example ``my_mesh.geo`` and add the line:

.. code-block:: bash

   Read_file my_mesh.geo ;

.. note::

   **Do not forget the semicolon at the end of the line!**

Transform mesh within the data file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also make transformations on your mesh after the **"Mailler"** or **"Read_"** command, using the following keywords:

-  **Trianguler** to triangulate your 2D cells and create an unstructured mesh (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#triangulate>`__).

-  **Tetraedriser** to tetrahedralise 3D cells and create an unstructured mesh (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#tetraedriser>`__).

-  **Raffiner_anisotrope** or **Raffiner_isotrope** to triangulate/tetrahedralise elements of an untructured mesh (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#raffineranisotrope>`__).

-  **ExtrudeBord** to generate an extruded mesh from a boundary of a tetrahedral or an hexahedral mesh (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#extrudebord>`__).

   .. note::

      ExtrudeBord in VEF generates 3 or 14 tetrahedra from extruded prisms.

-  **RegroupeBord** to build a new boundary with several boundaries of the domain (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#regroupebord>`__).

-  **Transformer** to transform the coordinates of the geometry (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#transformer>`__).

For other commands, see the section ``interprete`` of the TRUST Reference Manual `available here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#interprete>`__.

.. note::

   All theses keywords work on all mesh file formats (i.e. also for **\*.geo** or **\*.bin** or **\*.med** files).

Test your mesh
~~~~~~~~~~~~~~

The keyword **Discretiser_domaine** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#discretiserdomaine>`__) is useful to discretize the domain (faces will be created) without defining a problem. Indeed, you can create a minimal data file, post-process your mesh in lata format (for example) and visualize it with VisIt.

.. note::

   You must name all the boundaries to descretize!

Here is an example of this kind of data file (say ``my_data_file.data`` for example):

.. code-block:: bash

   dimension 3
   Domaine my_domain

   Mailler my_domain
   {
      Pave box
      {
         Origine 0. 0. 0.
         Longueurs 1 2 1
         Nombre_de_Noeuds 6 23 6
         Facteurs 1. 1. 1.
      }
      {
         bord Inlet X = 0. 0. <= Y <= 2. 0. <= Z <= 1.
         bord Outlet X = 1. 0. <= Y <= 2. 0. <= Z <= 1.
         bord Upper Y = 2. 0. <= X <= 1. 0. <= Z <= 1.
         bord Lower Y = 0. 0. <= X <= 1. 0. <= Z <= 1.
         bord Front Z = 0. 0. <= X <= 1. 0. <= Y <= 2.
         bord Back Z = 1. 0. <= X <= 1. 0. <= Y <= 2.
      }
   }

   discretiser_domaine my_domain
   postraiter_domaine { domaine my_domain fichier file format lata }
   End

To use it, launch in a bash terminal:

::

   # Initialize TRUST env if not already done
   > source $my_path_to_TRUST_installation/env_TRUST.sh

   # Run you data file
   > trust my_data_file
   > visit -o file.lata &

To see how to use VisIt, look at the first `TRUST Tutorial <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_tutorial.pdf#exo1>`__ exercise; Flow around an Obstacle.

Spatial Discretization
----------------------

You have to specify a discretization type to run a simulation. See the `Discretizations <https://cea-trust-platform.github.io/classes/discretizations>`__ section on the TRUST's website.

.. https://cea-trust-platform.github.io/classes/discretizations

Time Schemes
------------

Now you can choose your time scheme to solve your problem. For this you must specify the time scheme type wanted and give it a name. then you have to specify its parameters by filling the associated **Read** block.

.. code-block:: bash

   Scheme_type my_time_scheme
   Read my_time_scheme { ... }

Some available time schemes
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The time schemes available in the TRUST platform are summarized on the TRUST's website in the `Temporal schemes <https://cea-trust-platform.github.io/classes/temporal-schemes>`__ section.

.. https://cea-trust-platform.github.io/classes/temporal-schemes

Here are some available types of explicit schemes:

-  **Scheme_Euler_explicit** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#eulerscheme>`__).

-  **Schema_Adams_Bashforth_order_2** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#schemaadamsbashforthorder2>`__).

-  **Runge_Kutta_ordre_3** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#rungekuttaordre3>`__).

And also some available types of implicit schemes:

-  **Scheme_Euler_implicit** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#schemaeulerimplicite>`__).

-  **Schema_Adams_Moulton_order_3** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#schemaadamsmoultonorder3>`__).

For other schemes, see `doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#schematempsbase>`__ of the Reference Manual.

.. note::

   You can treat implicitly the diffusion/viscous operators in a TRUST calculation. For that, you should activate the **diffusion_implicite** keyword in your explicit time scheme.

Calculation stopping condition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You must specify at least one stopping condition for you simulation. It can be:

-  the final time: **tmax**

-  the maximal allowed cpu time: **tcpumax**

-  the number of time step: **nb_pas_dt_max**

-  the convergency treshold: **seuil_statio**

.. note::

   If the time step reaches the minimal time step **dt_min**, **TRUST** will stop the calculation.

If you want to stop properly your running calculation (i.e. with all saves), you may use the ``my_data_file.stop`` file. 

When the simulation is running, you can see the **0** value in that file.

To stop it, put a **1** instead of the **0**, save the file and at the next iteration the calculation will stop properly.

When you don’t change anything in that file, at the end of the calculation, you can see that it is writen **Finished correctly**.

Medium/Type of Fluide
---------------------

To specify the medium or fluid, you must add the following block.

.. code-block:: bash

   Fluid_type { ... }

**Fluid_type** can be one of the following:

-  **Fluide_incompressible** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#fluideincompressible>`__).

-  **Fluide_Quasi_compressible** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#fluidequasicompressible>`__).

-  **Fluide_Weakly_Compressible** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#fluideweaklycompressible>`__).

-  **Solide** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#solide>`__).

- **Constituant** (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#constituant>`__).

- **Milieu_Composite** (for Multi-Phase problems)


For other types and more information see the `TRUST Reference Manual <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#milieubase>`__.

.. note::

   Since TRUST v1.9.1, the medium should be read in the begining of the problem definition (before equations). 

   If you want to solve a coupled problem, each medium should be read in the corresponding problem.

Add Gravity
-----------

If needed, you can add a gravity term to your simulation. This is done by adding a uniform field, in the medium block since V1.9.1. 

For example in 2D:

.. code-block:: bash

   Gravity Uniform_field 2 0 -9.81

Objects association and discretization
--------------------------------------

Association
~~~~~~~~~~~

Until now, we have created some objects, now we must associate them together. For this, we must use the **Associate** interpretor (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#associate>`__):

.. code-block:: bash

   # Association between the different objects #
   Associate my_problem my_domain
   Associate my_problem my_time_scheme

.. _disc:

Discretization
~~~~~~~~~~~~~~

Then you must discretize your domain using the **Discretize** interpretor (`doc here <https://raw.githubusercontent.com/cea-trust-platform/trust-code/next/doc/TRUST/TRUST_Reference_Manual.pdf#discretize>`__):

.. code-block:: bash
   
   Discretize my_problem my_discretization

The problem *my_problem* is discretized according to the *my_discretization* discretization.

**IMPORTANT:** 

   A number of objects must be already associated (a domain, time scheme, ...) prior to invoking the **Discretize** keyword.

.. note::

   When the discretization step succeeds, the mesh is validated by the code.
