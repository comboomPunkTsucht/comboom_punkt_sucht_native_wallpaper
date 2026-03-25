#! /usr/bin/env bash
# Dieses Skript baut sowohl die WebAssembly (.wasm) als auch die SvelteKit App.

set -e

# install Just, falls nicht vorhanden
if ! command -v just &> /dev/null; then
    npm install -g rust-just
fi

# Baue WebAssembly und SvelteKit App
just wasm
cd WASM
bun run build
cd ..
echo "✅ Alle Builds abgeschlossen!"
