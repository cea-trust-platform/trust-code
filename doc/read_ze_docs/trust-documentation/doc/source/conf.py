import sys
import os
sys.path.insert(0, os.path.abspath('..'))
# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#

# for trust utils python docstrings
sys.path.insert(0, os.path.abspath('./srcs/trustutils/python_cloned_srcs/'))



# -- Project information -----------------------------------------------------

project = 'TRUST Documentation'
author = 'TRUST team'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
pygments_style = 'sphinx'
extensions = [
    'sphinx.ext.autodoc',
    # 'numpydoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.coverage',
    'sphinx.ext.doctest',
    'sphinx.ext.autosummary',
    'sphinx.ext.graphviz',
    'sphinx.ext.ifconfig',
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.napoleon'
]

htmlhelp_basename = 'TRUST Documentation'

# To show or not private members
autodoc_default_options = {     "members": True,     "undoc-members": True,     "private-members": False  }


# generate autosummary even if no references
numpydoc_show_class_members = False
autosummary_generate = False
autosummary_imported_members = False

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# master_doc = 'index'
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#

#html_theme = 'press'
# html_theme = 'alabaster'
#html_theme = 'sphinx_rtd_theme'
# html_theme = 'default'
html_theme = 'sphinx_material'
#html_theme = 'pydata_sphinx_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Nice little icon in tab header
html_favicon = 'favicon.ico'

# Generating doxygen from here - this will run first, before RST processing from sphinx:
import subprocess as sp

if 0:
  print("@@@ About to generate doxygen!!")
  sp.call("cd srcs/doxy; doxygen", shell=True)
  rtd = os.environ.get("READTHEDOCS_OUTPUT", "build")
  # Output directory must be created since this will run before Sphinx ...
  print(f"@@@ Creating output directory: {rtd}/html ...")
  sp.call(f"mkdir -p {rtd}/html", shell=True)
  print(f"@@@ Copying doxygen result to proper directory ...")
  sp.call(f"cp -a srcs/doxy/html {rtd}/html/doxy", shell=True)
  sp.call(f"cp -a srcs/doxy/favicon.ico {rtd}/html/doxy", shell=True)
  print("@@@ Done generating doxygen!!")

