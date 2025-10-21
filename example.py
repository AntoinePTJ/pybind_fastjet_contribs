import fastjet_contribs
import random

# Get some random particles
# Particles are a tuple pf 4 doubles/floats: (px, py, pz, E)
# Make sure to keep this format
particles = []
for i in range(100):
    particles.append( (random.uniform(-50, 50), random.uniform(-50, 50), random.uniform(-50, 50), random.uniform(0.0, 200.)) )

# Cluster XCone jets
# It returns subjets as a list of lists,
# where the j-th subjet of the i-th fatjet is allsubjets[i][j]
allsubjets = fastjet_contribs.cluster_xcone(particles, Nfatjets=2, Rfat=1.2, Nsubjets=3, Rsub=0.4, beta=2.0)

# Print the jets
for i, subjetsThisJet in enumerate(allsubjets):
    print(f"XCone fatjet {i} has {len(subjetsThisJet)} subjets")
    for j,jet in enumerate(subjetsThisJet):
        print(f"  - XCone subjet {j}: px={jet[0]}, py={jet[1]}, pz={jet[2]}, E={jet[3]}")

# Compute N-subjettiness
# It returns the N-subjettiness value tau_N for the jet given as a list of particles
tau1 = fastjet_contribs.compute_nsubjettiness(particles, N=1, R0=1.0, beta=1.0, axis_mode=1)
tau2 = fastjet_contribs.compute_nsubjettiness(particles, N=2, R0=1.0, beta=1.0, axis_mode=1)
print(f"1-subjettiness: {tau1}")
print(f"2-subjettiness: {tau2}")
print(f"Subjettiness ratio tau21: {tau2/tau1 if tau1 != 0 else 'undefined (zero tau1)'}")

# Compute soft drop jet and symmetry
# It returns the groomed jet and the soft drop symmetry as ((px, py, pz, E), zg)
# zg = -1 if no splitting was found
groomed_jet, z = fastjet_contribs.apply_soft_drop(particles, zcut=0.1, beta=0.0)
print(f"Soft drop symmetry zg: {z}")
if z != -1:
    print(f"Soft drop groomed jet: px={groomed_jet[0]}, py={groomed_jet[1]}, pz={groomed_jet[2]}, E={groomed_jet[3]}")
else:
    print("No valid soft drop splitting found.")