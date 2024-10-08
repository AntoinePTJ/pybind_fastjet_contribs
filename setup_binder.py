from setuptools import setup, Extension
import pybind11
import os

# Get the path to FastJet
fastjet_dir = "/home/dennis/fastjet-install"  # Modify this to point to your FastJet installation


# os.environ["CC"] = "x86_64-linux-gnu-g++"
# os.environ["CXX"] = "x86_64-linux-gnu-g++"

# Define the C++ extension module
ext_modules = [
    Extension(
        "fastjet_contribs",  # Name of the Python module
        ["fastjet_contribs.cpp"],  # C++ source file
        include_dirs=[
            pybind11.get_include(),  # Include pybind11 headers
            os.path.join(fastjet_dir, "include"),  # FastJet include directory
            os.path.join(fastjet_dir, "include/fastjet/contrib")  # FastJet Contrib include directory
        ],
        library_dirs=[os.path.join(fastjet_dir, "lib")],  # FastJet library directory
        libraries=["fastjettools", "fastjet", "Nsubjettiness", "RecursiveTools"],  # Link against FastJet and contribs
        extra_compile_args=["-std=c++11", "-fPIC", "-no-pie"],  # Use C++11 standard
    )
]

# Set up the Python package without using the deprecated pybind11 setup_helpers
setup(
    name="fastjet_contribs",
    version="0.1",
    ext_modules=ext_modules,
    # pybind11 provides a custom build extension class but it is not required.
    # setuptools handles the compilation.
)
