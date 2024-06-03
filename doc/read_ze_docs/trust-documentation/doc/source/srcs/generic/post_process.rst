Post-processing
===============

Output Files
------------

After running, you will find different files in your directory. Here is a short explaination of what you will find in each type of file depending on its extension.

Even if you don’t post-process anything, you will have output files which are listed here:

+----------------------------------+----------------------------------+
| **File**                         | **Contents**                     |
+==================================+==================================+
| *my_data_file*\ **.dt_ev**       | Time steps, facsec, equation     |
|                                  | residuals                        |
+----------------------------------+----------------------------------+
| *my_data_file*\ **.stop**        | Stop file (’0’, ’1’ or ’Finished |
|                                  | correctly’)                      |
+----------------------------------+----------------------------------+
| *my_data_file*\ **.log**         | Journal logging                  |
+----------------------------------+----------------------------------+
| *my_data_file*\ **.TU**          | CPU performances                 |
+----------------------------------+----------------------------------+
| *my_data_file*\ **\_detail.TU**  | Statistics of execution          |
+----------------------------------+----------------------------------+
| *my_da                           | Saving 2D/3D results for resume  |
| ta_file_problem_name*\ **.sauv** |                                  |
| or **.xyz**                      |                                  |
+----------------------------------+----------------------------------+
| or *specified_name*\ **.sauv**   | (binary files)                   |
| or **.xyz**                      |                                  |
+----------------------------------+----------------------------------+

and the listing of boundary fluxes where:

-  *my_data_file*\ **\_Contrainte_visqueuse.out** correspond to the friction drag exerted by the fluid.

-  *my_data_file*\ **\_Convection_qdm.out** contains the momentum flow rate.

-  *my_data_file*\ **\_Debit.out** is the volumetric flow rate.

-  *my_data_file*\ **\_Force_pression.out** correspond to the pressure drag exerted by the fluid.

If you add post-processings in your data files, you will find:

+--------------------------------------------+------------------------+
| **File**                                   | **Contents**           |
+============================================+========================+
| *my_data_file*\ **.sons**                  | 1D probes list         |
+--------------------------------------------+------------------------+
| *my_data_file_probe_name*\ **.son**        | 1D results with probes |
+--------------------------------------------+------------------------+
| *my_data_file_probe_name*\ **.plan**       | 3D results with probes |
+--------------------------------------------+------------------------+
| *my_data_file*\ **.lml** *(default         |                        |
| format)*                                   |                        |
+--------------------------------------------+------------------------+
| *my_data_file*\ **.lata** *(with all       |                        |
| \*.lata.\* files)*                         |                        |
+--------------------------------------------+------------------------+
| *my_data_file*\ **.med**                   | 2D/3D results          |
+--------------------------------------------+------------------------+
| or *specified_name*\ **.lml** or **.lata** |                        |
| or **.med**                                |                        |
+--------------------------------------------+------------------------+

The sceen outputs are automatically redirected in *my_data_file*\ **.out** and *my_data_file*\ **.err** files if you run a parallel calculation or if you use the "**-evol**" option of the "trust" script.

Else you can redirect them with the following command:

::

   # Source TRUST env if not already done
   > source $my_path_to_TRUST_installation/env_TRUST.sh

   # then
   > trust my_data_file 1>file_out.out 2>file_err.err

In the .out file, you will find the listing of physical infos with mass balance and in the .err file, the listing of warnings, errors and domain infos.

Tools
-----

To open your 3D results in **lata** format, you can use `VisIt <https://wci.llnl.gov/simulation/computer-codes/visit>`__ which is an open source software included in **TRUST** package. For that you may "source" **TRUST** environment and launch VisIt:

::

   # Source TRUST env if not already done
   > source $my_path_to_TRUST_installation/env_TRUST.sh

   # then
   > visit -o my_data_file.lata &

To learn how to use it, you can do the first exercise of the `TRUST Tutorial <https://github.com/cea-trust-platform/trust-code/blob/master/doc/TRUST/TRUST_tutorial.pdf>`__; Flow around an Obstacle.

To open your 3D results in **med** format, you can also use `VisIt <https://wci.llnl.gov/simulation/computer-codes/visit>`__, `SALOME <http://www.salome-platform.org>`__ or `Paraview <http://www.paraview.org>`__.

Here are some actions that you can perform when your simulation is finished:

-  To visualize the positions of your probes in function of the 2D/3D mesh, you can open your .son files at the same time of the .lata file in VisIt.

-  If you need more probes, you can create them with VisIt (if you have post-processed the good fields) or with MEDCoupling.

-  You can use the option **-evol** of the trust script, like:

   ::

      trust -evol my_data_file

   and access to the probes or open VisIt for 2D/3D visualizations via this tool.
