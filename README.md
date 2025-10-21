This repo is forked from https://github.com/denschwarz/XConeCluster with additional observables and installation with pip.

# Installation

1. Install fastjet and fastjet contribs
```
FASTJET_PATH=/path/to/fastjet/
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
```

2. Setup and compile python wrapper

- Activate the python environment in which you want to install fastjet_contribs
- Change ```fastjet_dir``` in ```setup.py``` to $FASTJET_PATH.

On Linux
```
sed -i "7s|.*|fastjet_dir = '$FASTJET_PATH'|" setup.py
```
- Run ```pip install .```


## Issues
If a library cannot be found, try running this in bash (inserting the path to the fastjet libs)
```export LD_LIBRARY_PATH=$FASTJET_PATH/lib:$LD_LIBRARY_PATH```
