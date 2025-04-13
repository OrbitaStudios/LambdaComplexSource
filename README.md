# Emscripten Source
Emscripten Source is a port of the entirety of the Source Engine to the [WASM platform](https://webassembly.org/) via the [Emscripten project](https://github.com/emscripten-core/emscripten/).

## Question and Answer
- Q: How do I compile this?
- A: Simply run the script named `scripts/build_emscripten.sh`
- Q: Does this contain leaked code?
- A: Unfortunately, yes, we do utilize the Source 2018 leak, but this is required to allow users to run it in their browsers.

## System Requirements
OS *: Linux
Processor: 3 Ghz.
Memory: 4-5 GB RAM.
Graphics: DirectX 9 level Graphics Card (requires support for SSE2)
Storage: 10 GB available space.
