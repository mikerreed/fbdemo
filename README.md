To build the demo...

1. Install harfbuzz into `third_party/externals/harfbuzz` like this:

```
    mkdir third_party
    cd third_party
    mkdir externals
    cd externals
    git clone git@github.com:harfbuzz/harfbuzz.git
```

This only needs to be done once, but it is not checked in as part of
this repo, so it must be done if you want to rebuild from sources.

2. Install `embind`

To install embind on macOS, you'll first need to make sure you have the following installed:

   A. A recent version of the `clang++` compiler, which you can get by installing the Xcode command line tools.
To do this, open a terminal and run the following command:

    xcode-select --install

The cmake build system, which you can install using a package manager like [brew](https://formulae.brew.sh/formula/cmake).
To do this, open a terminal and run the following command:

    brew install cmake

Once you have these prerequisites installed, you can install embind by running the following commands in a terminal:

Clone the `embind` repository from GitHub, navigate into the embind directory and create a build directory, then build and install:

    git clone https://github.com/emscripten-core/embind.git
    cd embind
    mkdir build
    cmake ..
    make
    make install

This should install embind on your system. You can test it by running the following command:

    python -c "import embind"

If the import statement doesn't produce any errors, then embind has been installed successfully.

3. Run make

    make

This should build everything, and copy the necessary files into `docs/`
