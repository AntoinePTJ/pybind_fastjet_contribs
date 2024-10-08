import fastjet_contribs
import random

# Get some random particles
particles = []
for i in range(100):
    particles.append( (random.uniform(-50, 50), random.uniform(-50, 50), random.uniform(-50, 50), random.uniform(0.0, 200.)) )

# Cluster XCone jets
allsubjets = fastjet_contribs.cluster_xcone(particles, Nfatjets=2, Rfat=1.2, Nsubjets=3, Rsub=0.4, beta=2.0)


# Print the jets
for i, subjetsThisJet in enumerate(allsubjets):
    print(f"XCone fatjet {i} has {len(subjetsThisJet)} subjets")
    for j,jet in enumerate(subjetsThisJet):
        print(f"  - XCone subjet {j}: px={jet[0]}, py={jet[1]}, pz={jet[2]}, E={jet[3]}")
