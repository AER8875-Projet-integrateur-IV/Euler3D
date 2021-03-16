#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Auteur: El Hadji Abdou Aziz Ndiaye
Date de création: Tue Mar  9 11:43:47 2021
Dernière modification: Tue Mar  9 11:43:47 2021
Description: Générateur de fichiers solution pour le post-traitement
"""

# ==============================================================================
# IMPORTATION DES MODULES
# ==============================================================================

import numpy as np
import time
import random as rd

# ==============================================================================
# CLASSES ET FONCTIONS
# ==============================================================================


def initialisation(nPart, partFile):
    files = []
    for i in range(nPart):
        file = partFile.replace("#", str(i))+".sol"
        files.append(file)

    return files


def writeFile(file, nElem):
    with open(file, "w") as fid:
        fid.write("rho\t\t\t\tu\t\t\t\tv\t\t\t\tw\t\t\t\tp\t\t\t\tE\n")
        for _ in range(nElem):
            rho = 1e6*rd.random()
            u = 1e6*rd.random()
            v = 1e6*rd.random()
            w = 1e6*rd.random()
            p = 1e6*rd.random()
            E = 1e6*rd.random()
            fid.write(
                f"{rho:.8e}\t{u:.8e}\t{v:.8e}\t{w:.8e}\t{p:.8e}\t{E:.8e}\n")


# ==============================================================================
# FONCTION PRINCIPALE
# ==============================================================================


def main():
    """Fonction principale"""
    # onera
    partFile = "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D_tests/Partition/mesh_ONERAM6_inv_ffd/mesh_ONERAM6_inv_ffd.su2_p#.par"
    nElems = [116777, 116751, 116746, 116239, 116239]
    # prism
    partFile = "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D_tests/Partition/NACA0012_65_prisms/NACA0012_65_prisms.su2_p#.par"
    nElems = [765, 789, 798]
    # prism
    partFile = "/home/aziz/Bureau/A2020/PI4/Codes/Euler3D_tests/Partition/naca0012_euler_129x129x2/naca0012_euler_129x129x2.su2_p#.par"
    nElems = [4093, 4098, 4096, 4097]
    nPart = len(nElems)
    files = initialisation(nPart, partFile)
    for i in range(nPart):
        writeFile(files[i], nElems[i])

    return


if __name__ == '__main__':
    main()


# ==============================================================================
