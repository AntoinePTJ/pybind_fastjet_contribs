#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // To convert between C++ and Python containers
#include <fastjet/ClusterSequence.hh>
// #include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/NjettinessPlugin.hh>
#include <fastjet/contrib/XConePlugin.hh>


namespace py = pybind11;

std::vector<std::vector<std::tuple<double, double, double, double>>> cluster_xcone(
    std::vector<std::tuple<double, double, double, double>> input_particles,
    int Nfatjets, double Rfat, int Nsubjets, double Rsub, double beta) {

    // Convert input particles from tuples to fastjet::PseudoJet
    std::vector<fastjet::PseudoJet> particles;
    for (const auto& p : input_particles) {
        particles.emplace_back(std::get<0>(p), std::get<1>(p), std::get<2>(p), std::get<3>(p));
    }

    // Setup XCone definition
    fastjet::contrib::PseudoXConePlugin plugin_xcone = fastjet::contrib::PseudoXConePlugin(Nfatjets, Rfat, beta);
    fastjet::JetDefinition jet_def(&plugin_xcone);

    // Cluster Fatjets with XCone
    fastjet::ClusterSequence cluster_sequence(particles, jet_def);
    std::vector<fastjet::PseudoJet> fatjets = cluster_sequence.inclusive_jets(0.0);

    // For the subjets we need a list of particles for each fatjet
    // We do this by getting the indices that are stored in FastJet
    std::vector<int> list_fat;
    list_fat.clear();
    list_fat = cluster_sequence.particle_jet_indices(fatjets);

    std::vector<std::vector<fastjet::PseudoJet>> constituents;
    for(unsigned int i=0; i<Nfatjets; i++){
      std::vector<fastjet::PseudoJet> constituents_thisjet;
      for (unsigned int ipart=0; ipart < particles.size(); ++ipart){
        if(list_fat[ipart]==i){
          constituents_thisjet.push_back(particles.at(ipart));
        }
      }
      constituents.push_back(constituents_thisjet);
    }

    // Cluster subjets, using the list of constituents for each fatjet
    std::vector<std::vector<fastjet::PseudoJet>> subjets;
    for(unsigned int i=0; i<Nfatjets; i++){
      std::vector<fastjet::PseudoJet> subjetsThisJet;
      if(constituents[i].size() >= Nsubjets){
        fastjet::contrib::PseudoXConePlugin plugin_xcone_sub(Nsubjets, Rsub, beta);
        fastjet::JetDefinition jet_def_sub(&plugin_xcone_sub);
        fastjet::ClusterSequence clust_seq_sub(constituents[i], jet_def_sub);
        subjetsThisJet = clust_seq_sub.inclusive_jets(0.0);
      }
      subjets.push_back(subjetsThisJet);
    }

    // Now convert into tuples of doubles to be returned to python
    std::vector<std::vector<std::tuple<double, double, double, double>>> return_jets;
    for(const auto& subjetsPerFat : subjets){
      std::vector<std::tuple<double, double, double, double>> subjetsInJet;
      for(const auto& jet : subjetsPerFat){
        subjetsInJet.emplace_back(jet.px(), jet.py(), jet.pz(), jet.e());
      }
      return_jets.push_back(subjetsInJet);
    }

    return return_jets;
}

PYBIND11_MODULE(fastjet_contribs, m) {
    m.def("cluster_xcone", &cluster_xcone, "Cluster XCone jets", py::arg("input_particles"), py::arg("Nfatjets") = 2, py::arg("Rfat") = 1.2, py::arg("Nsubjets") = 3, py::arg("Rsub") = 0.4, py::arg("beta") = 2.0);
}
