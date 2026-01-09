This repo is forked from https://github.com/denschwarz/XConeCluster with additional functions and installation with pip.
More functions can easily be added by editing fastjet_contribs.cpp before compiling.

# Installation

1. Define FastJet installation path in current directory
```
mkdir fastjet
cd fastjet
mkdir fastjet-install
export FASTJET_PATH=$PWD/fastjet-install
```

2. Clone this repo
```
git clone https://github.com/AntoinePTJ/pybind_fastjet_contribs.git
```

2. Install FastJet and FastJet contrib
```
wget https://fastjet.fr/repo/fastjet-3.5.1.tar.gz
tar zxvf fastjet-3.5.1.tar.gz
cd fastjet-3.5.1/
./configure --prefix=$FASTJET_PATH --enable-allcxxplugins --disable-auto-ptr CXXFLAGS=-fPIC
make CXXFLAGS="-fPIC"
make install CXXFLAGS="-fPIC"
cd ..
wget http://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.101.tar.gz
tar zxvf fjcontrib-1.101.tar.gz
cd fjcontrib-1.101
./configure --fastjet-config=$FASTJET_PATH/bin/fastjet-config CXXFLAGS=-fPIC
make
make install
make fragile-shared
make fragile-shared-install
cd ..
export LD_LIBRARY_PATH=$FASTJET_PATH/lib:$LD_LIBRARY_PATH
```

2. Setup and compile python wrapper
- Activate the python environment in which to install fastjet_contribs
- Go to ```pybind_fastjet_contribs```, set the path to the fastjet installation and install
```
cd pybind_fastjet_contribs
sed -i "7s|.*|fastjet_dir = '$FASTJET_PATH'|" setup.py
pip install .
```
