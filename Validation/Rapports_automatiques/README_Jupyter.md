# Validation forms using Jupyter notebooks

The validation forms of TRUST can now be either written in the (legacy) PRM format
or using the Jupyter notebook functionality. This new format will be pushed forward
in the coming versions of TRUST.

A first example (and good starting point) can be found in 
```sh
        SampleForm/SampleForm.ipynb
```

To run a Jupyter validation forms, you need a specific environment:
  - first, jupyterlab and some extra modules should be installed:
```sh
        conda install -c conda-forge jupyterlab ipywidgets ca-certificates=2020.10.14 conda=4.9.2 certifi=2020.6.20
```

  - the relevant Python environment should be loaded
```sh
        source $TRUST_ROOT/env_for_python.sh
```

  - finally, you should launch a Jupyter server (typically in this directory)
```sh
        cd $TRUST_ROOT/Validation/Rapports_automatiques
        jupyter-notebook
```

  - or alternatively you can directly go inside a form folder and execute :
```sh
        cd Verification/SampleFormJupyter
        Run_fiche
```

