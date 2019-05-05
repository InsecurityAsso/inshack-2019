from pathlib import Path
from setuptools import setup, find_packages
from passthru import __version__

HERE = Path(__file__).absolute().parent

def requirements():
    with HERE.joinpath('requirements.txt').open() as reqs:
        return list([req.strip() for req in reqs if req.strip()])

setup(
    # main information
    name='passthru',
    version=__version__,
    description='',
    author='author',
    author_email='author@example.com',
    url='https://github.com/InsecurityAsso/inshack-2019',
    # package files
    packages=find_packages(str(HERE)),
    install_requires=requirements(),
    # configuration files
    entry_points={
        'console_scripts': [
            'passthru = passthru.passthru:app',
            'passthru-noise = passthru.noise:app',
        ]
    }
)
