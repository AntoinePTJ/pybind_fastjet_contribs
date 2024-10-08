# Installation

- change fastjet_dir in setup_binder.py to fastjet location
- python3 setup_binder.py build_ext --inplace


1. Install fastjet
```
wget https://fastjet.fr/repo/fastjet-3.4.3.tar.gz
tar zxvf fastjet-3.4.3.tar.gz
cd fastjet-3.4.3/
./configure --prefix=/home/dennis/fastjet-install --enable-allcxxplugins --enable-pyext --disable-auto-ptr CXXFLAGS=-fPIC
make CXXFLAGS="-fPIC"
make install CXXFLAGS="-fPIC"
cd ..
```

2. Install fastjet contribs
```
wget http://fastjet.hepforge.org/contrib/downloads/fjcontrib-1.055.tar.gz
tar zxvf fjcontrib-1.055.tar.gz
cd fjcontrib-1.055
./configure --fastjet-config=PATH-TO-FASTJET-CONFIG CXXFLAGS=-fPIC
make
make install
make fragile-shared
make fragile-shared-install
```

3. Clone Github
```
git clone git@github.com:denschwarz/XConeCluster.git
```

4. Setup and compile python wrapper

- Change ```fastjet_dir``` in ```setup_binder.py``` to fastjet location in your system
- Run ```python3 setup_binder.py build_ext --inplace```
