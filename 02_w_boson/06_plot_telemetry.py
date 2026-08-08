import uproot
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

print("[ORIAS]: Booting Telemetry Observation Deck...")

# 1. Locate the data vault
file_path = "orias_synthetic_data.root"
if not os.path.exists(file_path):
    print(f"[ERROR]: {file_path} not found. You must run the C++ Orias Core first.")
    sys.exit(1)

# 2. Extract the matrix
print("[ORIAS]: Extracting Tier IV Smeared Muon Data...")
with uproot.open(file_path) as data_vault:
    # uproot reads the ROOT histogram natively
    muon_hist = data_vault["muon_pt"]
    
    # Extract bin counts and bin edges
    counts = muon_hist.values()
    edges = muon_hist.axis().edges()
    
# 3. Calculate Bin Centers and Statistical Error (sqrt(N))
centers = (edges[:-1] + edges[1:]) / 2.0
errors = np.sqrt(counts)

# 4. Render the Publication-Grade Canvas
print("[ORIAS]: Rendering Quantum Telemetry to PDF...")
fig, ax = plt.subplots(figsize=(10, 8))

# Plot data points with error bars (Precision markers only)
ax.errorbar(centers, counts, yerr=errors, fmt='ko', markersize=5, 
            capsize=3, elinewidth=1.5, label='Orias Monte Carlo (8 TeV)')

# Aesthetics
ax.set_title("CMS Smeared Muon Transverse Momentum ($p_T$)", fontsize=16, fontweight='bold', pad=15)
ax.set_xlabel("Muon $p_T$ [GeV]", fontsize=14)
ax.set_ylabel("Events / Bin", fontsize=14)
ax.tick_params(axis='both', which='major', labelsize=12)
ax.grid(True, linestyle='--', alpha=0.6)
ax.legend(fontsize=12, loc='upper right')

# Add a watermark to denote the simulation tier
ax.text(0.05, 0.95, "Project Orias: Tier IV Active\nsqrt(s) = 8 TeV", 
        transform=ax.transAxes, fontsize=12, verticalalignment='top',
        bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

# 5. Export and secure
output_filename = "orias_muon_pt_telemetry.pdf"
plt.tight_layout()
plt.savefig(output_filename, dpi=300)
print(f"[ORIAS]: Telemetry rendered successfully. Secured as {output_filename}.")