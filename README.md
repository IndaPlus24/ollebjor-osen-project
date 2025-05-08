# Loonar engine

very good game enginge many such featurse such as make game button and add mutliplayer button. Rarely seen these days...

# Specification

[Here](./spec.md)

# Build

```bash
git clone https://github.com/IndaPlus24/ollebjor-osen-project.git
cd ollebjor-osen-project
cmake -Wno-dev -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -B build
# Linux/OSX
cmake --build build
# Or Windows
cmake --build build --config Release
```

# Usage

```bash
# Unix
./build/Loonar
# Windows
start build\Release\Loonar.exe
```

# Example

![Simple](./simple.png)

![Sponza](./sponza.png)
