'''
file: setup.py
date: 2018-11-20
purpose:

'''
# =============================================================================
# IMPORTS
# =============================================================================
from pathlib import Path
from setuptools import setup, find_packages
from simdrone import __version_str__
# =============================================================================
# GLOBALS
# =============================================================================
HERE = Path(__file__).absolute().parent
# =============================================================================
# FUNCTIONS
# =============================================================================
def requirements():
    '''Parses requirements.txt
    '''
    with HERE.joinpath('requirements.txt').open() as reqs:
        return list([req.strip() for req in reqs if req.strip()])
# =============================================================================
# SCRIPT
# =============================================================================
setup(
    # main information
    name='simdrone',
    version=__version_str__,
    description='Drone simulation (really simplified)',
    author='author',
    author_email='author@example.com',
    url='http://example.com',
    # package files
    packages=find_packages(str(HERE)),
    install_requires=requirements(),
    # configuration files
    entry_points={
        'console_scripts': [
            'dms = simdrone.dms:app',
            'trackgen = simdrone.trackgen:app'
        ]
    }
)
