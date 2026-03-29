<script lang="ts">
  import { onMount, onDestroy } from 'svelte';
  import { T, useTask } from '@threlte/core';
  import * as THREE from 'three';

  interface Props {
    h1: string;
    h2: string;
    canvasSize: { w: number, h: number };
    quantity?: number;
  }
let wasmpath = "/cbps_we_core.wasm";

  let { h1 = $bindable(), h2 = $bindable(), canvasSize = $bindable(), quantity = 150 }: Props = $props();
  let mouse = { x: 0, y: 0 };

  // WASM State
  let wasmInstance: WebAssembly.Instance | null = null;
  let enginePtr: number = 0;
  let heapBase = 50000;
  // m_idx, f_idx, r_idx WURDEN GELÖSCHT!

  // Three.js / Threlte State
  let isReady = $state(false);
  let geometry = new THREE.BufferGeometry();
  let floatBuffer: THREE.InterleavedBuffer;
  let intBuffer: THREE.InterleavedBuffer;
  let byteBuffer: THREE.InterleavedBuffer;

  const material = new THREE.ShaderMaterial({
    glslVersion: THREE.GLSL3, // WICHTIG (Lösung 1 von vorhin für 'int radius')
    uniforms: {
      u_resolution: { value: new THREE.Vector2(canvasSize.w, canvasSize.h) },
      u_dpr: { value: window.devicePixelRatio || 1 }
    },
    vertexShader: `
      in vec2 translate;
      in int radius;
      in vec4 color;
      uniform vec2 u_resolution;
      uniform float u_dpr;
      out vec4 vColor;
      void main() {
        vec2 finalPos = position.xy + translate;
        vec2 zeroToOne = finalPos / u_resolution;
        vec2 clipSpace = (zeroToOne * 2.0) - 1.0;
        gl_Position = vec4(clipSpace * vec2(1.0, -1.0), 0.0, 1.0);
        gl_PointSize = float(radius) * 2.0 * u_dpr;
        vColor = color;
      }
    `,
    fragmentShader: `
      precision mediump float;
      in vec4 vColor;
      out vec4 fragColor;
      void main() {
        float dist = distance(gl_PointCoord, vec2(0.5)) * 2.0;
        float alpha = 1.0 - smoothstep(0.8, 1.0, dist);
        if (dist > 1.0) discard;
        fragColor = vec4(vColor.rgb, vColor.a * alpha);
      }
    `,
    transparent: true,
    blending: THREE.NormalBlending,
    depthWrite: false
  });

  function writeWasmString(str: string): number {
    if (!wasmInstance) return 0;
    const encoder = new TextEncoder();
    const buf = encoder.encode(str + '\0');
    const ptr = heapBase;
    heapBase = (heapBase + buf.length + 7) & ~7;
    const mem = new Uint8Array((wasmInstance.exports.memory as WebAssembly.Memory).buffer);
    mem.set(buf, ptr);
    return ptr;
  }

  function handleMouseMove(event: PointerEvent) {
    mouse.x = event.clientX;
    mouse.y = event.clientY;
  }

  function handleDeviceOrientation(event: DeviceOrientationEvent) {
    let tiltX = Math.max(-45, Math.min(45, event.gamma || 0));
    let tiltY = Math.max(-45, Math.min(45, (event.beta || 0) - 45));
    mouse.x = (canvasSize.w / 2) + (tiltX / 45) * (canvasSize.w / 2);
    mouse.y = (canvasSize.h / 2) + (tiltY / 45) * (canvasSize.h / 2);
  }

  // Hilfsfunktion: Wandelt einen C-Speicherpointer in einen JS-String um
  function readWasmStringFromMemory(ptr: number): string {
    if (!wasmInstance) return "";
    const memory = (wasmInstance.exports.memory as WebAssembly.Memory).buffer;
    const bytes = new Uint8Array(memory, ptr);
    let len = 0;
    while (bytes[len] !== 0) len++; // Suche das Null-Byte (\0) des C-Strings
    return new TextDecoder().decode(new Uint8Array(memory, ptr, len));
  }

  onMount(() => {
    const env = {
      js_log: (strPtr: number) => {
        console.log("🛠️ [C-ENGINE]:", readWasmStringFromMemory(strPtr));
      },
      js_log_num: (strPtr: number, num: number) => {
        console.log("📊 [C-ENGINE]:", readWasmStringFromMemory(strPtr), num);
      }
    };

    fetch(wasmpath)
      .then(res => res.arrayBuffer())
      .then(bytes => WebAssembly.instantiate(bytes, { env }))
      .then(({ instance }) => {
        wasmInstance = instance;

        // KEIN SAFARI HACK MEHR!
        let h1Ptr = writeWasmString(h1);
        let h2Ptr = writeWasmString(h2);

        // Aufruf GANZ OHNE m_idx, f_idx, r_idx
        enginePtr = (instance.exports.cbps_engine_create as Function)(
          canvasSize.w, canvasSize.h, quantity, h1Ptr, h2Ptr
        );

        const memView = new DataView((wasmInstance.exports.memory as WebAssembly.Memory).buffer);
        const particlesArrayPtr = memView.getUint32(enginePtr + 0, true);
        const arrayBuffer = (wasmInstance.exports.memory as WebAssembly.Memory).buffer;

        floatBuffer = new THREE.InterleavedBuffer(new Float32Array(arrayBuffer, particlesArrayPtr, quantity * 9), 9);
        geometry.setAttribute('position', new THREE.InterleavedBufferAttribute(floatBuffer, 2, 0));
        geometry.setAttribute('translate', new THREE.InterleavedBufferAttribute(floatBuffer, 2, 2));

        intBuffer = new THREE.InterleavedBuffer(new Int32Array(arrayBuffer, particlesArrayPtr, quantity * 9), 9);
        geometry.setAttribute('radius', new THREE.InterleavedBufferAttribute(intBuffer, 1, 7));

        byteBuffer = new THREE.InterleavedBuffer(new Uint8Array(arrayBuffer, particlesArrayPtr, quantity * 36), 36);
        geometry.setAttribute('color', new THREE.InterleavedBufferAttribute(byteBuffer, 4, 32, true));

        isReady = true;
      });
  });

  onDestroy(() => {
    if (enginePtr && wasmInstance) {
      // Zerstören ohne f_idx
      (wasmInstance.exports.cbps_engine_destroy as Function)(enginePtr);
    }
  });

  useTask((delta) => {
    if (!isReady || !wasmInstance) return;

    material.uniforms.u_resolution.value.set(canvasSize.w, canvasSize.h);

    // dynH1Ptr und dynH2Ptr WURDEN GELÖSCHT!

    // Wir übergeben 0 für die Strings, C ignoriert sie dann sicher.
    (wasmInstance.exports.cbps_engine_update as Function)(
      enginePtr, delta, 0, 0, canvasSize.w, canvasSize.h,
      Math.round(mouse.x), Math.round(mouse.y)
    );

    floatBuffer.needsUpdate = true;
    intBuffer.needsUpdate = true;
    byteBuffer.needsUpdate = true;
  });
</script>

<svelte:window
  onpointermove={handleMouseMove}
  ondeviceorientation={handleDeviceOrientation}
/>

{#if isReady}
  <T.Points {geometry} {material} frustumCulled={false} />
{/if}
