#!/usr/bin/env python
from debian.changelog import Changelog
from os import listdir
from os.path import abspath, join, dirname, isfile, splitext
from setuptools import setup, find_packages

here = abspath(dirname(__file__))
requirements = open(join(here, 'requirements.txt')).readlines()
changelog = open(join(here, 'debian/changelog'))
modules = [
    splitext(filename)[0]
    for filename in listdir(join(here, "src"))
    if isfile(join(here, "src", filename)) and splitext(filename)[1] == '.py'
]

setup(
    name='hlc.web',
    version=str(Changelog(changelog).get_version()),
    packages=find_packages('src'),
    py_modules=modules,
    package_dir={'': 'src'},
    install_requires=requirements,
    author='Karotka',
    author_email='karotka@seznam.cz',
    test_suite='tests')

