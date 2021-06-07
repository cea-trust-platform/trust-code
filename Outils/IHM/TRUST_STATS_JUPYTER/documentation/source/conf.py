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
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Reference manual'
author = 'Morad BEN TAYEB'


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
    'sphinx.ext.napoleon'
]

htmlhelp_basename = 'Reference Manual'
latex_documents = [
    ('index', 'Reference_manual.tex', u'Reference Manual',
     u'Morad BEN TAYEB', 'manual'),
]
man_pages = [
    ('index', 'Reference_manual', u'Reference Manual',
     [u'Morad BEN TAYEB'], 1)
]

latex_show_urls = 'footnote'

# To show or not private members 
autodoc_default_options = {     "members": True,     "undoc-members": True,     "private-members": True  }


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

# html_theme = 'press'
# html_theme = 'alabaster'
# html_theme = 'sphinx_rtd_theme'
html_theme = 'default'
# html_theme = 'pydata_sphinx_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
