#! /bin/bash

__dir__=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )

pushd $__dir__
if [ `arch` = x86_64 ]; then
    arch=x64
else
    arch=x86
fi

bin_dir=${__dir__}/../bin/linux/${arch}
if [ -d $bin_dir ]; then
    rm -rf $bin_dir
fi
mkdir -p $bin_dir

pushd addon
node-gyp configure 
node-gyp build 
cp -f build/Release/plugin.node $bin_dir
popd

# converter example

mkdir -p plugin/build
pushd plugin/build
cmake ..
cmake --build .
cp -f ../build/libconverter.so $bin_dir

popd
popd