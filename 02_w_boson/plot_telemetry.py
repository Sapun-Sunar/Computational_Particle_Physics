import pandas as pd
import matplotlib.pyplot as plt

# Execute via terminal: python3 plot_telemetry.py

print("[JARVIS]: Initializing Tri-Panel Visual Analytics Array...")

try:
    df = pd.read_csv("telemetry_data.csv")
except FileNotFoundError:
    print("[ERROR]: telemetry_data.csv missing. Run the C++ Oracle first, My Lord.")
    exit()

fig, axs = plt.subplots(1, 3, figsize=(18, 6), facecolor='white')
plt.style.use('seaborn-v0_8-darkgrid')

# THE FIX: Dynamically extracting the column names (which now contain the masses)
columns = df.columns[2:5] 
base_titles = ['Ultimate Truth', '2nd Best Fit', '3rd Best Fit']
colors = ['gold', 'blue', 'green']
linestyles = ['-', '--', ':']

for i in range(3):
    col_name = columns[i]
    mass_value = col_name.split('_')[1] # Extracts just the number from 'Top1_80.377'
    
    # Plotting the Real CMS Data
    axs[i].fill_between(df['Mass'], df['Data'], step="mid", color='maroon', alpha=0.2)
    axs[i].step(df['Mass'], df['Data'], where="mid", color='darkred', linewidth=1.5, label='CMS Data (Real)', zorder=3)
    
    # Plotting the Peak-Normalized Simulated Template
    axs[i].step(df['Mass'], df[col_name], where="mid", linestyle=linestyles[i], color=colors[i], linewidth=2.5, label=f'Simulation', zorder=4)
    
    axs[i].set_title(f'Template Match: {base_titles[i]}', fontsize=14, fontweight='bold')
    axs[i].set_xlabel(r'Transverse Mass $M_T$ [GeV]', fontsize=12)
    axs[i].set_ylabel('Events / 1.0 GeV', fontsize=12)
    
    # THE VISUAL BLACKOUT
    axs[i].set_xlim(40.0, 110.0) 
    
    # THE STAMP: Printing the locked mass directly onto the canvas
    textstr = f'Locked Mass:\n{mass_value} GeV'
    props = dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='gray')
    axs[i].text(0.05, 0.95, textstr, transform=axs[i].transAxes, fontsize=12,
            verticalalignment='top', bbox=props, fontweight='bold', color='black')
    
    axs[i].legend(fontsize=10, loc='upper right', frameon=True, shadow=True)

plt.tight_layout()
plt.savefig("w_boson_peak_normalized_tri_panel.png", dpi=300)
print("[JARVIS]: Telemetry rendered. Mass vectors successfully stamped to canvas.")