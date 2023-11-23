# Validation forms using Jupyter notebooks

The validation forms of TRUST are now written using the Jupyter notebook functionality.
The old (legacy) PRM format is still used by applications based on TRUST (BALTIKS).

A first example (and good starting point) can be run with
```sh
        Run_fiche -doc
```

To run a Jupyter validation forms, you need a specific environment:
  - first, jupyter and some extra modules should be installed. This is normally done automatically when installing TRUST. Make sure you use the TRUST's version of juptyer
```sh
        which jupyter-notebook
```

  - if you intend to use MEDCoupling in the notebook, this can be directly done in the notebook using
```sh
        run.useMEDCoupling()
```
    alternatively, the relevant Python environment can be loaded with
```sh
        source $TRUST_ROOT/env_for_python.sh
```

  - finally, you should launch a Jupyter server (typically in the following directory)
```sh
        cd $TRUST_ROOT/Validation/Rapports_automatiques
        jupyter-notebook
```

  - or alternatively you can directly go inside a form folder and execute:
```sh
        cd Verification/SampleFormJupyter
        Run_fiche
```
    This will open a browser showing you the form and run/add cells.
    
  - **WARNING** : DO NOT EDIT *.ipynb notebook using text editor

  - to directly export the Jupyter notebook into a PDF file, you can do:
```sh
        Run_fiche -export_pdf [-not_run]
```

  - You can export the Jupyter notebook into a TEX file using:
```sh
        Run_fiche -export_tex
```

