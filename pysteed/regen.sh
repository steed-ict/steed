#!/bin/bash

# run this scrip in docker to rebuild

# re-build the package
cd /io/build ; rm -rf * ; cmake -DCMAKE_BUILD_TYPE=Debug .. ; make clean ; make all -j ; sudo make install

# re-generate wheels
rm /io/wheelhouse/*
/opt/python/cp36-cp36m/bin/pip wheel /io/ -w /io/wheelhouse/
/opt/python/cp38-cp38/bin/pip wheel /io/ -w /io/wheelhouse/
/opt/python/cp310-cp310/bin/pip wheel /io/ -w /io/wheelhouse/
ls /io/wheelhouse # *linux_x86_64.whl

# re-repair by manylinux
auditwheel repair /io/wheelhouse/*.whl -w /io/wheelhouse/
ls /io/wheelhouse/* # *manylinux_2_17_x86_64.manylinux2014_x86_64.whl

# # uninstall and re-install the package
# /opt/python/cp36-cp36m/bin/pip uninstall pysteed
# /opt/python/cp36-cp36m/bin/pip install /io/wheelhouse/pysteed-1.0.0.1-cp36-cp36m-manylinux_2_17_x86_64.manylinux2014_x86_64.whl

# # re-run to debug
# cd /io/pysteed
# /opt/python/cp36-cp36m/bin/python pysteed_demo.py
