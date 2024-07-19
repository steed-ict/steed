## use setuptools to build python extension module and package
from setuptools import setup, Extension, find_packages

# steed extension module
steed_extension = Extension(
    "pysteed",
    sources=["./src/steed/pysteed.cpp"],
    include_dirs=[
        "./include",
        "./src/steed/assemble",
        "./src/steed/base",
        "./src/steed/conf",
        "./src/steed/parse",
        "./src/steed/schema",
        "./src/steed/store",
        "./src/steed/util",
        "./src/thirdparty/cli11/include",
    ],
    library_dirs=["/usr/local/lib"],
    libraries=["steed"],
    language="c++",
    # ## disable debug mode
    # extra_compile_args=["-std=c++11"]
    ## enable debug mode
    extra_compile_args=["-std=c++11", "-g", "-O0"]
)

setup(
    name="pysteed",
    version="1.0.0.1",
    author="zhiyiwang",
    author_email="wangzhiyi@ict.ac.cn",
    description="Python interface for Steed",
    long_description=open("README.md").read(),
    long_description_content_type='text/markdown',
    url="https://github.com/steed-ict/steed",
    packages=find_packages(),
    ext_modules=[steed_extension],
)
