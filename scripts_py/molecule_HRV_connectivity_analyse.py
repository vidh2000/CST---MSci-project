import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import os
from os.path import expanduser
from causets_py import causet_helpers as ch
from scipy.optimize import curve_fit
from scipy.stats import chisquare, pearsonr, skew, kurtosis
from scipy.sparse.csgraph import connected_components


###############################################################################
# Open files HRVs_connectivity_compress/M=<>_Rho=<>_Card=<>_r=<>_hollow=<>_dur=<>_n.txt
# each containing the HRVs molecules from the nth run of a causet with 
# specified parameters.
#
# Then save a file with columns being # clusters of HRVs with certain size
# and associated standard deviation (like with lambdas), of title
# HRV_connectivity_compress/M=<>_Rho=<>_Card=<>_r=<>_hollow=<>_dur=<>.txt
###############################################################################
    

##############################################################################
# 0. SET USER VARIABLES
##############################################################################
molecules = "HRVs" #lambdas, HRVs
varying_var = "M"     #variable varying: can be M, Rho, Nmult (M best)
fixed_var = "Rho"     #variable fixed:   can be, M, Rho, Nmult (Rho best)
fixed_val = 5000     #value of fixed_var 
use_selected_masses = True #gives equal spacing

#plt.rcParams['text.latex.preamble']=[r"\usepackage{lmodern}"]
#Options
params = {'text.usetex' : True,
          'font.size' : 20,
          'font.family' : 'lmodern',
          'axes.labelsize':34,
          'legend.fontsize': 18,
          'xtick.labelsize': 20,
          'ytick.labelsize': 20,
          'figure.figsize': [8.5, 6.5], 
          'axes.prop_cycle':plt.cycler(color=
                            plt.rcParams['axes.prop_cycle'].by_key()['color']
                            +['magenta'])
          }
plt.rcParams.update(params)


##############################################################################
# 1 SET USEFUL STRINGS & OTHER VARIABLES
##############################################################################
home = expanduser("~")
plotsDir = f"{home}/MSci_Schwarzschild_Causets/figures/N{molecules}_vs_Area/"
dataDir = f"{home}/MSci_Schwarzschild_Causets/data/{molecules}_connectivity/"
#dataDir = home + f"/MSci_Schwarzschild_Causets/data/linkcounting_files/Poiss=False/"
if not os.path.exists(dataDir):
    path = os.getcwd()
    plotsDir = path + f"/figures/N{molecules}_vs_Area/"
    dataDir = path + f"/data/{molecules}_connectivity_compress/"
print(dataDir)
# ensure that, if using Mor Rho, it is decimal with exactly 2 dec digits

if fixed_var == "M" or fixed_var == "Rho":
    fixed_string = f"{fixed_var}=" + format(fixed_val,".2f")
else:
    fixed_string = f"{fixed_var}={fixed_val}"


selected_masses = np.array([
                            0.53 ,0.65, 0.75, 0.84, 0.92, 0.99, 1.06, 
                            # 1.13, 1.19, 1.24, 1.30, 1.35, 1.40, 1.45, 
                            # 1.50, 1.55, 1.59, 1.63, 1.68, 1.72, 1.76, 
                            # 1.80, 1.84, 1.88, 1.91, 1.95, 1.98, 2.02, 
                            # 2.05, 2.09, 2.12, 2.15, 2.19, 2.22, 2.25, 
                            # 2.28, 2.31, 2.34, 2.37, 2.40, 
                            # 2.43, 2.46, 2.49,
                            ])



##############################################################################
# 2. GET INFO FROM FILES
##############################################################################

# an entry for each M
Nreps = []
varying_values = []
connectivities = [] # connectivities[n][i] is avg # of clusters of size n
                    # for the ith mass
connectivities_std = []


for root, dirs, files in os.walk(dataDir):
    # for each file file_i
    for i, file_i in enumerate(files):
        if fixed_string in file_i:
            go_on = False
            if varying_var+"=" in file_i:
                go_on = True
                file_i_path = os.path.join(root, file_i)
                file_i_pieces = file_i.split("_")
                # get the value of the varying variable of file_i
                for piece_j in file_i_pieces:
                    if varying_var+"=" in piece_j:
                        varying_var_i = float(piece_j.split("=")[1])
                        if use_selected_masses \
                            and varying_var_i not in selected_masses:
                            go_on = False
                            break #j loop
                        if varying_var_i not in varying_values:
                            print(varying_var+"=", varying_var_i)
                            varying_values.append(varying_var_i)
                            Nreps.append([])
                        break
            # if the file is correct
            if go_on: 
                # 2.1 get file and Nreps #####################################
                # oppositely to lambdas and HRVs, here there is NOT a "," at 
                # end of lines, thus no need of importing as strings
                everything_i = np.loadtxt(file_i_path, 
                                            delimiter = ",", skiprows=1,
                                            dtype = str)
                #make it 2D for later, and make it float
                if not hasattr(float(everything_i[0]), "__len__"):
                    everything_i = np.array([[float(x) for x in everything_i
                                               if x!= ""]])
                else:
                    everything_i = np.array([[float(x) for x in line if x!= ""]
                                                for line in everything_i])
                Nreps_i = everything_i[:,0]
                tot_Nreps_i = sum(Nreps_i)
                Nreps[-1] = tot_Nreps_i
                # 2.2 get clusters size distribution ##########################
                clust_info_i = everything_i[:,1:]
                nsizes_i = clust_info_i.shape[1]/2
                # should be int
                if (nsizes_i - int(nsizes_i)): 
                    raise Exception(f"Number of cols {nsizes_i*2} is odd")
                else:
                    nsizes_i = int(nsizes_i)
                # extract info on cluster of size n of mass i
                for n in range(nsizes_i):
                    avgs_clust_n_i = clust_info_i[:,2*n]
                    stds_clust_n_i = clust_info_i[:,2*n+1]
                    n_clust_n_i, std_clust_n_i = ch.combine_meass(Nreps_i,
                                                            avgs_clust_n_i,
                                                            stds_clust_n_i)
                    # if the nth molecule had already been found
                    try:
                        connectivities[n]    .append(n_clust_n_i)
                        connectivities_std[n].append(std_clust_n_i)
                    #if the nth molecule was never found
                    except IndexError: 
                        # make zeros for previous (general for rho and nmult)
                        n_prev_values = len(varying_values) -1
                        zeros = np.zeros(n_prev_values).tolist()
                        connectivities    .append(zeros + [n_clust_n_i]) 
                        connectivities_std.append(zeros + [std_clust_n_i])

#############################################################
# INFO ON NREPS
vals = np.array(varying_values)
maxrep = max(Nreps)
vals_not_max = [vals [i] for i in range(len(vals)) if Nreps[i] < maxrep]
reps_not_max = [Nreps[i] for i in range(len(vals)) if Nreps[i] < maxrep]
repstable = pd.DataFrame(
                np.column_stack(
                [vals_not_max, reps_not_max, maxrep-np.array(reps_not_max)]),
                columns = ["M Value", "Current Nreps", "Reps to Max"])
print(f"Maximum number of Nreps is {maxrep}")
print("For the following, the maximum number of rep was not reached")
print(repstable, "\n")

print(connectivities)




############################################################
# PLOTS OF RESULTS

# Set the right x for the varying-fixed values -> Area in l^2 units
x = np.array(varying_values)
if varying_var=="M":
    x = 4*np.pi*(2*x)**2 #==Area
    if fixed_var == "Nmult":    
        Rho = fixed_val/(4/3*np.pi*26)
        print(f"You're doing Nmult {fixed_val} fixed!") 
    elif fixed_var == "Rho":
        Rho = fixed_val
    x /= Rho**(-1/2)
r_S_norm = np.sqrt(x / 4 / np.pi)   
    # x_A = x/divide by fund units


###############################################################################
# Clusters' Number per size
print(f"Rho = {Rho:.0f}")
fixed_string = rf"Rho = {Rho:.0f}"

plt.figure(f"Molecules for {fixed_string}")
for n in range(len(connectivities)):
    label = str(n+3) + "-Clusters"
    y    = connectivities[n]
    yerr = connectivities_std[n]
    if len(y) < len(x):
        for i in range(len(x)-len(y)):
            y.append(0)
            yerr.append(0)
    plt.errorbar(x, y, yerr, 
                fmt = '.', capsize = 4, 
                label = label)
plt.legend(ncol = 2)
plt.xlabel(r'Horizon Area $[\ell^2]$')
plt.ylabel(r"Number of Clusters")
plt.grid(alpha = 0.2)
plt.tight_layout()
# plt.savefig(plotsDir + f"{fixed_string}_{molecules}.png") 
# plt.savefig(plotsDir + f"{fixed_string}_{molecules}.pdf") 


##############################################################################
#Clusters' Distribution
# Get Probs from Counts directly - more weight on larger M
# lambd_occurs[n] is the number of mols found in total of size n
lambd_occurs = np.array([sum(connectivities[n]) 
                    for n in range(len(connectivities))])
lambd_occurs_stds = np.sqrt([sum(np.array(connectivities_std[n])**2)
                            for n in range(len(connectivities))])
S = sum(lambd_occurs)*1.
lambd_probs = lambd_occurs/S
lambd_probs_uncs = np.sqrt(((S - lambd_occurs)/S**2)**2 * lambd_occurs_stds**2 \
                        +\
                        lambd_occurs**2/S**4 * sum(lambd_occurs_stds**2))
print("p_n list and sigma_p_n obtained with overall occurrences")
print([round(pn,5) for pn in lambd_probs])
print([round(spn,5) for spn in lambd_probs_uncs])

plt.figure()
plt.errorbar(np.arange(3, len(lambd_probs)+3), lambd_probs, 
             yerr = lambd_probs_uncs,
             fmt = ".", capsize = 4, )
plt.xlabel('Cluster Size')
plt.ylabel(r"Probability")
plt.grid(alpha = 0.2)
plt.tight_layout()

plt.figure()
plt.errorbar(np.arange(4, len(lambd_probs)+3), lambd_probs[1:], 
             yerr = lambd_probs_uncs[1:],
             fmt = ".", capsize = 4, )
plt.xlabel('Cluster Size')
plt.ylabel(r"Probability")
plt.yscale('log')
plt.grid(alpha = 0.2)
plt.tight_layout()



plt.show()
