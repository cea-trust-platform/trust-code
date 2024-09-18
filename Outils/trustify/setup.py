""" pip setup file """
from setuptools import setup, find_packages

setup(
    name='trustify',
    version='0.1',
    description='Python Parser for TRUST datasets',
    author_email='trust@cea.fr',
    package_dir={'': 'src'},
    packages=find_packages(where="src", include=["trustify", "trustify.*"]),
    install_requires=['pydantic'],
    extras_require={'dev': ['pytest']}
)
