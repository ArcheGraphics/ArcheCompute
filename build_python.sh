#!/usr/bin/env bash

pip install .
pybind11-stubgen arche_compute._core -o python