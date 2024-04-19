# Asset Bundler

Asset bundler utility to bundle multiple assets into one binary file.

### The design is fairly simple:
```
head:
   file version, 4 bytes
   asset index count, 4 bytes
   data start position, 4 bytes
asset index tree:
   id, 8 bytes
   start, 4 bytes
   size, 4 bytes
data tree:
   bytes
```

- **File Version**: This is not important for now but later on
if I decide to change the design of the binary client should just look at the
version and decide how to read that file according to it.
- **Asset Index Count**: We use this to determine how many assets
provided in the binary in order to iterate through index tree.
- **Data Start Position**: We don't really need this but it makes things easier
on the client and if we change the data structure of an asset index client might
not know the new size of it.
- **Asset Index Tree**: This is the place where we have our all assets defined.
We are basicaly registering the assets by their ID's, starting position and it's
size.
- **Data Tree**: Finally we are keeping our all data here

### Usage:

After you clone and make you just need to provide the input asset file as an
argument.
```
./asset-bundler asset-pack.apkg
```

### Building:

I am using Makefiles to keep everything simple as possible but it should
work in any other build systems or compilers (MSVC, CLANG, GCC) as well.

- Clone the repository:
```bash
git clone https://github.com/Gl1tchs/asset-bundler.git
cd asset-bundler
```

- And build it:
```bash
make
```
