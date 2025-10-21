#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/contrib/NjettinessPlugin.hh>
#include <fastjet/contrib/XConePlugin.hh>
#include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/SoftDrop.hh>

namespace py = pybind11;

std::vector<std::vector<std::tuple<double, double, double, double>>> cluster_xcone(
    std::vector<std::tuple<double, double, double, double>> input_particles,
    int Nfatjets, double Rfat, int Nsubjets, double Rsub, double beta)
{

    // Convert input particles from tuples to fastjet::PseudoJet
    std::vector<fastjet::PseudoJet> particles;
    for (const auto &p : input_particles)
    {
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
    for (unsigned int i = 0; i < Nfatjets; i++)
    {
        std::vector<fastjet::PseudoJet> constituents_thisjet;
        for (unsigned int ipart = 0; ipart < particles.size(); ++ipart)
        {
            if (list_fat[ipart] == i)
            {
                constituents_thisjet.push_back(particles.at(ipart));
            }
        }
        constituents.push_back(constituents_thisjet);
    }

    // Cluster subjets, using the list of constituents for each fatjet
    std::vector<std::vector<fastjet::PseudoJet>> subjets;
    for (unsigned int i = 0; i < Nfatjets; i++)
    {
        std::vector<fastjet::PseudoJet> subjetsThisJet;
        if (constituents[i].size() >= Nsubjets)
        {
            fastjet::contrib::PseudoXConePlugin plugin_xcone_sub(Nsubjets, Rsub, beta);
            fastjet::JetDefinition jet_def_sub(&plugin_xcone_sub);
            fastjet::ClusterSequence clust_seq_sub(constituents[i], jet_def_sub);
            subjetsThisJet = clust_seq_sub.inclusive_jets(0.0);
        }
        subjets.push_back(subjetsThisJet);
    }

    // Now convert into tuples of doubles to be returned to python
    std::vector<std::vector<std::tuple<double, double, double, double>>> return_jets;
    return_jets.reserve(subjets.size());
    for (const auto &subjetsPerFat : subjets)
    {
        std::vector<std::tuple<double, double, double, double>> subjetsInJet;
        subjetsInJet.reserve(subjetsPerFat.size());
        for (const auto &jet : subjetsPerFat)
        {
            subjetsInJet.emplace_back(jet.px(), jet.py(), jet.pz(), jet.e());
        }
        return_jets.push_back(subjetsInJet);
    }

    return return_jets;
}

double compute_nsubjettiness(
    const std::vector<std::tuple<double, double, double, double>> &input_particles,
    int N,
    double beta,
    double R0,
    int axis_mode,
    bool use_normalized_measure)
{
    // Convert input particles to PseudoJet objects
    std::vector<fastjet::PseudoJet> particles;
    particles.reserve(input_particles.size());
    for (const auto &p : input_particles)
    {
        particles.emplace_back(std::get<0>(p), std::get<1>(p), std::get<2>(p), std::get<3>(p));
    }

    // Combine particles into a single jet
    fastjet::PseudoJet jet = fastjet::join(particles);

    // Define axes based on axis_mode
    fastjet::contrib::AxesDefinition *axes_def;
    switch (axis_mode)
    {
    case 1:
        axes_def = new fastjet::contrib::WTA_KT_Axes();
        break;
    case 2:
        axes_def = new fastjet::contrib::OnePass_WTA_KT_Axes();
        break;
    case 3:
        axes_def = new fastjet::contrib::KT_Axes();
        break;
    case 4:
        axes_def = new fastjet::contrib::OnePass_KT_Axes();
        break;
    case 5:
        axes_def = new fastjet::contrib::WTA_CA_Axes();
        break;
    default:
        throw std::invalid_argument("Invalid axis_mode. Use 1 (wta_kt_axes), 2 (onepass_wta_kt), 3 (kt), 4 (onepass_kt) or 5 (wta_ca)");
    }

    // Define measure
    fastjet::contrib::MeasureDefinition *measure_def;
    if (use_normalized_measure)
    {
        measure_def = new fastjet::contrib::NormalizedMeasure(beta, R0);
    }
    else
    {
        measure_def = new fastjet::contrib::UnnormalizedMeasure(beta);
    }

    // Compute N-subjettiness
    fastjet::contrib::Nsubjettiness nsub(N, *axes_def, *measure_def);
    double result = nsub.result(jet);

    // Clean up
    delete axes_def;
    delete measure_def;

    return result;
}

std::tuple<std::tuple<double, double, double, double>, double> apply_soft_drop(
    const std::vector<std::tuple<double, double, double, double>> &input_particles,
    double beta,
    double zcut,
    double R0,
    const std::string &jet_algorithm)
{
    // Convert input particles to PseudoJet objects
    std::vector<fastjet::PseudoJet> particles;
    particles.reserve(input_particles.size());
    for (const auto &p : input_particles)
    {
        particles.emplace_back(std::get<0>(p), std::get<1>(p), std::get<2>(p), std::get<3>(p));
    }

    // Define jet algorithm
    fastjet::JetAlgorithm algorithm;
    if (jet_algorithm == "cambridge")
    {
        algorithm = fastjet::cambridge_algorithm;
    }
    else if (jet_algorithm == "kt")
    {
        algorithm = fastjet::kt_algorithm;
    }
    else if (jet_algorithm == "antikt")
    {
        algorithm = fastjet::antikt_algorithm;
    }
    else
    {
        throw std::invalid_argument("Invalid jet_algorithm. Use 'cambridge', 'kt', or 'antikt'");
    }

    // Cluster jets
    fastjet::JetDefinition jet_def(algorithm, R0);
    fastjet::ClusterSequence cs(particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = cs.inclusive_jets(0.0);

    if (jets.empty())
    {
        throw std::runtime_error("No jet found after clustering");
    }

    // Apply Soft Drop to leading jet
    fastjet::PseudoJet jet = jets[0];
    fastjet::contrib::SoftDrop sd(beta, zcut, R0);
    fastjet::PseudoJet groomed_jet = sd(jet);

    double zg = -1.0; // Default value if no valid splitting is found

    if (groomed_jet.has_structure_of<fastjet::contrib::SoftDrop>())
    {
        zg = groomed_jet.structure_of<fastjet::contrib::SoftDrop>().symmetry();
    }

    return std::make_tuple(
        std::make_tuple(groomed_jet.px(), groomed_jet.py(), groomed_jet.pz(), groomed_jet.e()),
        zg);
}

PYBIND11_MODULE(fastjet_contribs, m)
{
    m.doc() = "Python bindings for FastJet contrib tools";

    m.def("cluster_xcone", &cluster_xcone,
          "Cluster XCone jets",
          py::arg("input_particles"),
          py::arg("Nfatjets") = 2,
          py::arg("Rfat") = 1.2,
          py::arg("Nsubjets") = 3,
          py::arg("Rsub") = 0.4,
          py::arg("beta") = 2.0);

    m.def("compute_nsubjettiness", &compute_nsubjettiness,
          "Compute N-subjettiness of a jet",
          py::arg("input_particles"),
          py::arg("N") = 1,
          py::arg("beta") = 1.0,
          py::arg("R0") = 1.0,
          py::arg("axis_mode") = 4,
          py::arg("use_normalized_measure") = true);

    m.def("apply_soft_drop", &apply_soft_drop,
          "Apply Soft Drop grooming to a jet",
          py::arg("input_particles"),
          py::arg("beta") = 0.0,
          py::arg("zcut") = 0.1,
          py::arg("R0") = 1.0,
          py::arg("jet_algorithm") = "antikt");
}