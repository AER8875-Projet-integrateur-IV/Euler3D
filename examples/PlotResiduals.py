import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import sys

title = ""
if len(sys.argv) == 1 or sys.argv[1] == "-h" or sys.argv[1] == "--help" :
    print('usage: python PlotResiduals.py <residuals> <title>\n'
          '       residuals         : path to the residuals file to be plotted\n'
          '       title(optionnal)  : plot title\n')
    exit(1)
else:
    path = Path(sys.argv[1])
    if len(sys.argv) == 3:
        title = sys.argv[2]


data: np.ndarray = np.loadtxt(path,skiprows=1)
x = [i for i in range(0,data.size) ]
plt.plot(x,data)
plt.yscale("log")
plt.xlabel("Iterations")
plt.ylabel("RMS rho")
plt.title(title)

savePath = path.parent / (str(path.stem) + "_plot.png")
plt.savefig(savePath)