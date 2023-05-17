#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import os
import sys

from typing import *

"""
Generates plots from benchmark data

args[1] is the folder where the generated plots will be saved.
"""

MIN_LIMBS = 1
MAX_LIMBS = 10


def plot_bn(df: pd.DataFrame, nlimbs: int) -> None:
    pass


def plot_fp(df: pd.DataFrame, nlimbs: int) -> None:
    pass


def plot_ecc(df: pd.DataFrame, nlimbs: int) -> None:
    pass


def main():
    if len(sys.argv) > 1:
        out_dir = sys.argv[1]
        print(out_dir)
    else:
        out_dir = "plots"
        print(out_dir)

    for i in range(MIN_LIMBS, MAX_LIMBS + 1):
        ct_df: pd.DataFrame = None  # Todo
        sct_df: pd.DataFrame = None # Todo
        print(i)


if __name__ == "__main__":
    main()
