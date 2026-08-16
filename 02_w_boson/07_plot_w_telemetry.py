import uproot
import numpy as np
import awkward as ak
import matplotlib.pyplot as plt
import re

print("[JARVIS]: Booting Tier VIII 2D Master Measurement Analyzer...")

bin_edges = np.linspace(20.0, 60.0, 101)
bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2.0
window_mask = (bin_centers >= 37.0) & (bin_centers <= 43.0)

print("[JARVIS]: Extracting and Purifying CMS Run2012B Baseline...")
try:
    cms_file = uproot.open("Run2012B_SingleMu.root")
    cms_tree = cms_file["Events"]
    
    pt = cms_tree["Muon_pt"].array(library="ak")
    eta = cms_tree["Muon_eta"].array(library="ak")
    
    available_branches = cms_tree.keys()
    if "Muon_pfRelIso04_all" in available_branches:
        iso = cms_tree["Muon_pfRelIso04_all"].array(library="ak")
    elif "Muon_iso" in available_branches:
        iso = cms_tree["Muon_iso"].array(library="ak")
    else:
        raise ValueError("Isolation branch missing from ROOT file.")

    has_muon = ak.num(pt) > 0
    leading_pt = pt[has_muon][:, 0].to_numpy()
    leading_eta = eta[has_muon][:, 0].to_numpy()
    leading_iso = iso[has_muon][:, 0].to_numpy()

    tight_mask = (np.abs(leading_eta) < 2.1) & (leading_iso < 0.12)
    clean_cms_pt = leading_pt[tight_mask]
    
    cms_counts, _ = np.histogram(clean_cms_pt, bins=bin_edges)
    
except Exception as e:
    print(f"[JARVIS]: FATAL ERROR reading CMS data. Details: {e}")
    exit()

print("[JARVIS]: Opening the 2D W-Measurement Matrix...")
try:
    matrix_file = uproot.open("orias_w_measurement_matrix.root")
except FileNotFoundError:
    print("[JARVIS]: FATAL ERROR. The C++ master engine matrix is missing.")
    exit()

best_chi2 = float('inf')
best_mass = 0.0
best_isr = 0.0
best_mc_counts = None

# Regex updated to extract both Mass and ISR
name_pattern = re.compile(r"muon_pt_m(\d+)_isr(\d+)")
total_keys = len(matrix_file.keys())
print(f"[JARVIS]: Scanning {total_keys} Universes across 2 Dimensions...")

for key in matrix_file.keys():
    clean_name = key.split(";")[0]
    
    match = name_pattern.search(clean_name)
    if not match:
        continue
        
    mass_val = float(match.group(1)) / 1000.0
    isr_val = float(match.group(2)) / 10.0
    
    mc_counts = matrix_file[key].values()
    
    mc_integral = np.sum(mc_counts[window_mask])
    cms_integral = np.sum(cms_counts[window_mask])
    
    if mc_integral == 0:
        continue
        
    scale_factor = cms_integral / mc_integral
    scaled_mc = mc_counts * scale_factor
    
    chi2 = np.sum( ((cms_counts[window_mask] - scaled_mc[window_mask])**2) / (scaled_mc[window_mask] + 1e-9) )
    
    if chi2 < best_chi2:
        best_chi2 = chi2
        best_mass = mass_val
        best_isr = isr_val
        best_mc_counts = scaled_mc

print("\n" + "="*60)
print("[JARVIS]: 2D TARGET NEUTRALIZED. ABSOLUTE GLOBAL MINIMUM LOCATED.")
print("="*60)
print(f"Measured W-Boson Mass : {best_mass:.3f} GeV")
print(f"Measured ISR Recoil   : {best_isr:.1f} GeV")
print(f"Minimum \u03C7\u00B2 Penalty    : {best_chi2:.2f}")
print("="*60)

plt.figure(figsize=(12, 8))

plt.errorbar(bin_centers, cms_counts, yerr=np.sqrt(cms_counts), fmt='ko', markersize=4, 
             label='CMS Run2012B (Tight Isolated Muons)')
plt.plot(bin_centers, best_mc_counts, 'r-', linewidth=2, 
         label=f'Optimal 2D Universe:\nMass = {best_mass:.3f} GeV | ISR = {best_isr:.1f} GeV')

plt.axvspan(37.0, 43.0, color='gray', alpha=0.15, label='Strict Jacobian \u03C7\u00B2 Fit Window')

plt.title("Tier VIII Measurement: 2D Global Minimum Hunt", fontsize=16, fontweight='bold')
plt.xlabel(r"Muon Transverse Momentum ($p_{T}$) [GeV]", fontsize=14)
plt.ylabel("Events / 0.4 GeV", fontsize=14)
plt.legend(fontsize=12)
plt.grid(True, linestyle='--', alpha=0.6)

plt.xlim(25, 55)
peak_height = np.max(cms_counts[(bin_centers >= 25) & (bin_centers <= 55)])
plt.ylim(0, peak_height * 1.15)

plt.tight_layout()
plt.savefig("orias_w_mass_2d_final.png", dpi=300)
print("[JARVIS]: Final 2D telemetry secured as 'orias_w_mass_2d_final.png'.")