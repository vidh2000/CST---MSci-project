
#%% 
from __future__ import annotations
from typing import List, Tuple  
from causets.sprinkledcauset import SprinkledCauset
from causets.embeddedcauset import EmbeddedCauset  
from causets.spacetimes import FlatSpacetime, deSitterSpacetime  
from causets.shapes import CoordinateShape  
from causets.causetevent import CausetEvent  
import causets.causetplotting as cplt  

import matplotlib.pyplot as plt
#%%

C: EmbeddedCauset = EmbeddedCauset()
S: CoordinateShape = CoordinateShape(3,"bicone",radius=4)
C.create_EmbeddedCauset_from_file("D:\Documents/Sola/Imperial College London/Year 4/MSci project/MSci_Schwarzschild_Causets/data/flatspace_bicone_causet.txt")


# Plotting setup:
cplt.setDefaultColors('UniYork')  # using University of York brand colours

dims: List[int] = [1,2,0]  # choose the (order of) plot dimensions
if len(dims) > 2:
    cplt.figure(figsize=(6.0, 6.0))
S.plot(dims)  # plot the embedding shape
# Add causet plots and show result:
cplt.plot(C, dims=dims, events={'alpha': 1},
          links={'alpha': 0.4, 'linewidth': 0.5}, labels=False)

ax: cplt.Axes = cplt.gca()
ax.set_xlabel('space' if dims[0] > 0 else 'time')
ax.set_ylabel('space' if dims[1] > 0 else 'time')
if len(dims) > 2:
    ax.set_zlabel('space' if dims[2] > 0 else 'time')
    ax.grid(False)
cplt.show()

# %%
