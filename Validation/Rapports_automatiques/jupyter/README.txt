Validation forms using Jupyter notebooks
========================================

This folder contains validation forms written as Jupyter notebooks.

To run those validation forms, you need a specific environment:
  - first, jupyterlab and some extra modules should be installed:
        conda install -c conda-forge jupyterlab pandas ipywidgets ca-certificates=2020.10.14 conda=4.9.2 certifi=2020.6.20

  - the relevant Python environment should be loaded
        source $TRUST_ROOT/env_for_python.sh

  - finally, you should launch a Jupyter server (typically in this directory)
        cd $TRUST_ROOT/Validation/Rapports_automatiques/jupyter
        jupyter-notebook


As a first example, you should take a look at the form located in
    SampleForm/SampleForm.ipynb


