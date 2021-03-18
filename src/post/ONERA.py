#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Pytecplot_ONERA.py
Auteur: El Hadji Abdou Aziz Ndiaye
Date de création: Thu Mar 18 2021
Dernière modification: Thu Mar 18 2021
Description: Post-traitement avec Pytecplot ( cas de l'ONERA)
             contour, courbes de pression
"""
# Pour exécuter le script, il faut créer un dossier nommé output et faire:
# ONERA.py pathToSurfaceFile.plt

# ==============================================================================
# IMPORTATION DES MODULES
# ==============================================================================

import tecplot
import os
import sys
import logging

logging.basicConfig(stream=sys.stdout, level=logging.INFO)


# Run this script with "-c" to connect to Tecplot 360 on port 7600
# To enable connections in Tecplot 360, click on:
#   "Scripting" -> "PyTecplot Connections..." -> "Accept connections"
if '-c' in sys.argv:
    tecplot.session.connect()

datafile = sys.argv[1]
# expfile = sys.argv[2]
expfile = "ONERA_EXP.dat"

# ==============================================================================
# CLASSES ET FONCTIONS
# ==============================================================================

# Lecture des fichiers Tecplot
dataset = tecplot.data.load_tecplot(datafile)
dataExp = tecplot.data.load_tecplot(expfile)

# Tracé des contours
Variables = ["Rho", "U", "V", "W", "P", "E", "Cp"]
var = "Cp"
page_contour = tecplot.add_page()
page_contour.name = 'Page for contour plot'
for i, var in enumerate(Variables):
    print("Traçage du contour de", var)
    frame = page_contour.add_frame()
    frame.plot_type = tecplot.constant.PlotType.Cartesian3D
    plot = frame.plot()
    plot.activate()

    # Adjust view
    plot.view.alpha = 0
    plot.view.theta = 0
    plot.view.psi = 0
    plot.view.position = (0.57096637, 0.60895912, -6.11326E-007)

    # Contour avec ligne
    plot.show_contour = True
    contour = plot.contour(0)
    fmaps = plot.fieldmaps()
    fmaps.contour.contour_type = tecplot.constant.ContourType.Overlay
    contour.variable = dataset.variable(var)
    contour.colormap_name = 'Small Rainbow'
    contour.lines.mode = tecplot.constant.ContourLineMode.DashNegative

    # Reset contour levels value
    contour.levels.reset_to_nice()

    # position de l'orientation des axes
    plot.axes.orientation_axis.position = (15, 80)

    # export image
    figName = "output/ONERA_M6_"+var+".png"
    tecplot.export.save_png(figName, 600, supersample=3)

# Courbes de pression
page_pression = tecplot.add_page()
page_pression.name = 'Page for Cp slice'
Coupes = [0.2, 0.44, 0.65, 0.8, 0.9, 0.96, 0.99]
for i, b in enumerate(Coupes):
    print("Traçage de la coupe du Cp à y =", b)
    # Création d'une frame
    frame = page_pression.add_frame()
    frame.plot_type = tecplot.constant.PlotType.Cartesian3D

    # Coupe de la solution numérique (Euler3D)
    extracted_slice = tecplot.data.extract.extract_slice(
        origin=(0, b, 0),
        normal=(0, 1, 0),
        source=tecplot.constant.SliceSource.SurfaceZones,
        dataset=dataset, frame=frame)

    # Changer le type de plot
    frame.plot_type = tecplot.constant.PlotType.XYLine
    plot = frame.plot()

    # Effacer les anciennes figures
    plot.delete_linemaps()

    # Normaliser la coordonnées en x avec la corde
    extracted_x = extracted_slice.values('X')
    x = extracted_x.as_numpy_array()
    xc = (x - x.min()) / (x.max() - x.min())
    extracted_x[:] = xc

    # Creation de la ligne pour la solution numérique
    cp_linemap = plot.add_linemap(
        name="Euler3D",
        zone=extracted_slice,
        x=dataset.variable('X'),
        y=dataset.variable('Cp'))

    # Creation de la ligne pour la solution expérimentale
    cpExp_linemap = plot.add_linemap(
        name="Experimental",
        zone=dataExp.zone(f"Section  {i+1}"),
        x=dataExp.variable('X/L'),
        y=dataExp.variable('Cp'))

    # Activer les symboles (pour la solution expérimentale)
    plot.show_symbols = True

    # Texte pour indiquer la coupe
    frame.add_text('coupe à y/b = %.2f' % b, (65, 85), bold=True, size=20)

    # Paramètre de la ligne (solution numérique)
    cp_linemap.line.color = tecplot.constant.Color.Blue
    cp_linemap.line.line_thickness = 0.8
    cp_linemap.y_axis.reverse = True
    cp_linemap.symbols.show = False

    # Paramètre de la ligne (solution expérimentale)
    cpExp_linemap.symbols.show = True
    cpExp_linemap.line.show = False
    cpExp_linemap.symbols.symbol().shape = tecplot.constant.GeomShape.Square
    cpExp_linemap.symbols.size = 2.5
    cpExp_linemap.symbols.color = tecplot.constant.Color.Red
    cpExp_linemap.symbols.line_thickness = 0.4
    cpExp_linemap.symbols.fill_mode = tecplot.constant.FillMode.UseSpecificColor
    cpExp_linemap.symbols.fill_color = tecplot.constant.Color.Red
    cpExp_linemap.y_axis.reverse = True

    # Update axes limits to show data
    plot.view.fit()

    # Titres des axes
    plot.axes.y_axis(
        0).title.title_mode = tecplot.constant.AxisTitleMode.UseText
    plot.axes.y_axis(0).title.text = 'Coefficient de pression'

    plot.axes.x_axis(
        0).title.title_mode = tecplot.constant.AxisTitleMode.UseText
    plot.axes.x_axis(0).title.text = 'x/c'

    # Legende
    legend = plot.legend
    legend.show = True
    legend.box.box_type = tecplot.constant.TextBox.Filled
    legend.box.color = tecplot.constant.Color.Purple
    legend.box.fill_color = tecplot.constant.Color.LightGrey
    legend.box.line_thickness = .4
    legend.box.margin = 5

    legend.position = (70, 25)

    # Export image
    figName = 'output/cp_vs_x_'+str(b)+'.png'
    tecplot.export.save_png(figName, 600, supersample=3)
