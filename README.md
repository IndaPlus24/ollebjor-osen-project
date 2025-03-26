# Loonar engine

very good game enginge many such featurse such as make game button and add mutliplayer button. Rarely seen these days...

# Build

```bash
git clone https://github.com/IndaPlus24/ollebjor-osen-project.git --recursive --remote
cd ollebjor-osen-project
# If Linux or MacOS
cd vendor/SDL && ./configure && cd ../..
# If Windows
mv external/SDL_config_windows.h vendor/SDL/include/SDL_config.h
mkdir build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -Wno-dev -DCMAKE_BUILD_TYPE=Release -B build
mv build/compile_commands.json ./
cmake --build build
```

