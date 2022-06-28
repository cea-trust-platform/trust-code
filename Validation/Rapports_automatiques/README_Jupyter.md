# Validation forms using Jupyter notebooks

The validation forms of TRUST can now be either written in the (legacy) PRM format
or using the Jupyter notebook functionality. This new format will be pushed forward
in the coming versions of TRUST.

A first example (and good starting point) can be found in 
```sh
        $TRUST_ROOT/Validation/Rapports_automatiques/Verification/SampleFormJupyter/SampleFormJupyter.ipynb
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

  - finally, you should launch a Jupyter server (typically in this directory)
```sh
        cd $TRUST_ROOT/Validation/Rapports_automatiques
        jupyter-notebook
```

  - or alternatively you can directly go inside a form folder and execute:
```sh
        cd Verification/SampleFormJupyter
        Run_fiche
```
    This will open a browser showing you the form.
    
  - or you can run:
```sh
        Run_fiche -doc
```
    This will open a browser showing you the form.

  - to directly export the Jupyter notebook into a PDF file, you can do:
```sh
        Run_fiche -export_pdf [-not_run]
```

