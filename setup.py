from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11
import os

# Path to FastJet installation
fastjet_dir = "/path/to/fastjet"
fastjet_lib = os.path.join(fastjet_dir, "lib")
fastjet_include = os.path.join(fastjet_dir, "include")

ext_modules = [
    Extension(
        "fastjet_contribs",
        ["fastjet_contribs.cpp"],
        include_dirs=[
            pybind11.get_include(),
            fastjet_include,
        ],
        library_dirs=[fastjet_lib],
        libraries=["fastjet", "fastjettools", "fastjetcontribfragile"],
        extra_compile_args=["-std=c++11", "-fPIC"],
        extra_link_args=[
            f"-Wl,-rpath,{fastjet_lib}",
            "-Wl,--no-as-needed",
        ],
    )
]

setup(
    name="fastjet_contribs",
    version="0.2.0",
    description="Python bindings for FastJet contrib tools",
    ext_modules=ext_modules,
    zip_safe=False,
)