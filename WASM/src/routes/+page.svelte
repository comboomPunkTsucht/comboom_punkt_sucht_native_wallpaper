<script lang="ts">
  import { onMount } from "svelte";
  import { cn } from "$lib/utils";
  import { z } from "zod";
  import { page } from "$app/state"; // <-- Nativer SvelteKit Store

  const SearchParamsSchema = z.object({
    h1: z.string().optional().default("mcpeaps_HD"),
    h2: z.string().optional().default("comboom.sucht"),
  });

  // 1. Wir lesen die Parameter nativ aus und machen sie reaktiv ($derived)!
  let params = $derived(SearchParamsSchema.parse(Object.fromEntries(page.url.searchParams)));
  let h1 = $derived(params.h1);
  let h2 = $derived(params.h2);

  let quantity = 400;

  let canvas: HTMLCanvasElement | null = $state(null);
  let gl: WebGL2RenderingContext | null = $state(null);

  let mouse = { x: 0, y: 0 };
  let canvasSize = { w: 0, h: 0 };

  // --- WASM & Engine State ---
  let wasmInstance: WebAssembly.Instance | null = null;
  let enginePtr: number = 0;
  let heapBase = 1024;
  let frameHeapBase = 1024; // Speichert den Speicher-Startpunkt für jeden Frame
  let m_idx = 0, f_idx = 0, r_idx = 0;

  function handleMouseMove(event: PointerEvent) {
    if (!canvas) return;
    const rect = canvas.getBoundingClientRect();
    mouse.x = event.clientX - rect.left;
    mouse.y = event.clientY - rect.top;
  }

  function handleDeviceOrientation(event: DeviceOrientationEvent) {
    if (!canvas) return;
    const rect = canvas.getBoundingClientRect();
    let tiltX = Math.max(-45, Math.min(45, event.gamma || 0));
    let tiltY = Math.max(-45, Math.min(45, (event.beta || 0) - 45));
    mouse.x = (rect.width / 2) + (tiltX / 45) * (rect.width / 2);
    mouse.y = (rect.height / 2) + (tiltY / 45) * (rect.height / 2);
  }

  function handleResize() {
    if (!canvas) return;
    const dpr = window.devicePixelRatio || 1;
    canvasSize.w = window.innerWidth;
    canvasSize.h = window.innerHeight;
    canvas.width = canvasSize.w * dpr;
    canvas.height = canvasSize.h * dpr;
    canvas.style.width = `${canvasSize.w}px`;
    canvas.style.height = `${canvasSize.h}px`;
  }

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

  const vsSource = `#version 300 es
    in vec2 a_position;
    in vec4 a_color;
    in float a_size;
    uniform vec2 u_resolution;
    out vec4 v_color;
    void main() {
      vec2 zeroToOne = a_position / u_resolution;
      vec2 clipSpace = (zeroToOne * 2.0) - 1.0;
      gl_Position = vec4(clipSpace * vec2(1, -1), 0.0, 1.0);
      gl_PointSize = a_size;
      v_color = a_color;
    }`;

  const fsSource = `#version 300 es
    precision mediump float;
    in vec4 v_color;
    out vec4 outColor;
    void main() {
      float dist = distance(gl_PointCoord, vec2(0.5)) * 2.0;
      float alpha = 1.0 - smoothstep(0.8, 1.0, dist);
      if (dist > 1.0) discard;
      outColor = vec4(v_color.rgb, v_color.a * alpha);
    }`;

  onMount(() => {
    if (!canvas) return;
    gl = canvas.getContext("webgl2", { alpha: true, antialias: true });
    if (!gl) return;

    handleResize();

    const vShader = gl.createShader(gl.VERTEX_SHADER)!;
    gl.shaderSource(vShader, vsSource); gl.compileShader(vShader);
    const fShader = gl.createShader(gl.FRAGMENT_SHADER)!;
    gl.shaderSource(fShader, fsSource); gl.compileShader(fShader);
    const program = gl.createProgram()!;
    gl.attachShader(program, vShader); gl.attachShader(program, fShader);
    gl.linkProgram(program);

    const posLoc = gl.getAttribLocation(program, "a_position");
    const colorLoc = gl.getAttribLocation(program, "a_color");
    const sizeLoc = gl.getAttribLocation(program, "a_size");
    const resLoc = gl.getUniformLocation(program, "u_resolution");

    let animationFrameId: number;
    let particleData = new Float32Array(quantity * 7);
    let lastTime = performance.now();

    fetch("/cbps_we_core.wasm")
      .then(res => res.arrayBuffer())
      .then(bytes => WebAssembly.instantiate(bytes, { env: {} }))
      .then(({ instance }) => {
        wasmInstance = instance;

        const table = (instance.exports.__indirect_function_table || instance.exports.table) as WebAssembly.Table;
        if (!table) {
          console.error("🔥 VERFÜGBARE WASM EXPORTS:", instance.exports);
          alert("WASM Tabelle fehlt! Hast du nach 'just wasm' einen Hard-Refresh (Cmd+Shift+R) gemacht?");
          return;
        }

        m_idx = table.length;
        table.grow(3);

        f_idx = m_idx + 1;
        r_idx = m_idx + 2;

        // 1. Unsere puren JavaScript-Funktionen
        const jsMalloc = (size: number) => {
          let ptr = heapBase;
          heapBase = (heapBase + size + 7) & ~7;
          return ptr;
        };

        const jsFree = (ptr: number) => {};

        const jsRand = () => Math.floor(Math.random() * 2147483647);

        // 2. SAFARI-HACK: Wir schreiben die rohen Bytes für ein 60-Byte Wasm-Modul!
        // Dieses Modul importiert unsere JS-Funktionen und exportiert sie als Wasm-Funktionen.
        const wrapperBytes = new Uint8Array([
          0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, // Magic & Version
          0x01, 0x0e, 0x03, 0x60, 0x01, 0x7f, 0x01, 0x7f, // Type 0 (malloc)
          0x60, 0x01, 0x7f, 0x00, 0x60, 0x00, 0x01, 0x7f, // Type 1 (free), Type 2 (rand)
          0x02, 0x19, 0x03, 0x03, 0x65, 0x6e, 0x76, 0x01, // Import: env.m
          0x6d, 0x00, 0x00, 0x03, 0x65, 0x6e, 0x76, 0x01, // Import: env.f
          0x66, 0x00, 0x01, 0x03, 0x65, 0x6e, 0x76, 0x01, // Import: env.r
          0x72, 0x00, 0x02, 0x07, 0x0d, 0x03, 0x01, 0x6d, // Exports...
          0x00, 0x00, 0x01, 0x66, 0x00, 0x01, 0x01, 0x72,
          0x00, 0x02
        ]);

        // 3. Modul kompilieren und unsere JS-Funktionen hineinreichen (dauert 0.1 Millisekunden)
        const wrapperMod = new WebAssembly.Module(wrapperBytes);
        const wrapperInst = new WebAssembly.Instance(wrapperMod, {
          env: { m: jsMalloc, f: jsFree, r: jsRand }
        });

        // JETZT sind es "echte" Wasm-Funktionen und Safari liebt sie!
        table.set(m_idx, wrapperInst.exports.m as Function);
        table.set(f_idx, wrapperInst.exports.f as Function);
        table.set(r_idx, wrapperInst.exports.r as Function);

        // --- AB HIER WIE GEWOHNT WEITER ---
        let h1Ptr = writeWasmString(h1);
        let h2Ptr = writeWasmString(h2);

        enginePtr = (instance.exports.cbps_engine_create as Function)(
          canvasSize.w, canvasSize.h, quantity, h1Ptr, h2Ptr, m_idx, f_idx, r_idx
        );

        frameHeapBase = heapBase;

        const buffer = gl!.createBuffer();
        gl!.bindBuffer(gl!.ARRAY_BUFFER, buffer);
        gl!.bufferData(gl!.ARRAY_BUFFER, particleData.byteLength, gl!.DYNAMIC_DRAW);

        const stride = 7 * 4;
        gl!.enableVertexAttribArray(posLoc); gl!.vertexAttribPointer(posLoc, 2, gl!.FLOAT, false, stride, 0);
        gl!.enableVertexAttribArray(colorLoc); gl!.vertexAttribPointer(colorLoc, 4, gl!.FLOAT, false, stride, 8);
        gl!.enableVertexAttribArray(sizeLoc); gl!.vertexAttribPointer(sizeLoc, 1, gl!.FLOAT, false, stride, 24);

        function render(time: number) {
          if (canvasSize.w === 0 || canvasSize.h === 0) {
            animationFrameId = requestAnimationFrame(render);
            return;
          }

          const delta = (time - lastTime) / 1000.0;
          lastTime = time;

          const memView = new DataView((wasmInstance!.exports.memory as WebAssembly.Memory).buffer);

          // 1. SPEICHER ZURÜCKSETZEN & TEXTE LADEN
          heapBase = frameHeapBase;
          let dynH1Ptr = writeWasmString(h1);
          let dynH2Ptr = writeWasmString(h2);

          // 2. UPDATE AUFRUFEN (Mit exakt 11 Parametern, wie es C erwartet!)
          (wasmInstance!.exports.cbps_engine_update as Function)(
            enginePtr,
            delta,
            dynH1Ptr,
            dynH2Ptr,
            canvasSize.w,
            canvasSize.h,
            Math.round(mouse.x),  // Maus X direkt als Argument
            Math.round(mouse.y),  // Maus Y direkt als Argument
            m_idx,
            f_idx,
            r_idx
          );

          // 3. PARTIKEL AUSLESEN
          const particlesArrayPtr = memView.getUint32(enginePtr + 0, true);

          for (let i = 0; i < quantity; i++) {
            const pBase = particlesArrayPtr + (i * 36);

            const p_x = memView.getFloat32(pBase + 0, true);
            const p_y = memView.getFloat32(pBase + 4, true);
            const p_tx = memView.getFloat32(pBase + 8, true);
            const p_ty = memView.getFloat32(pBase + 12, true);
            const p_radius = memView.getInt32(pBase + 28, true);
            const r = memView.getUint8(pBase + 32);
            const g = memView.getUint8(pBase + 33);
            const b = memView.getUint8(pBase + 34);
            const a = memView.getUint8(pBase + 35);

            const idx = i * 7;
            particleData[idx + 0] = (p_x + p_tx) * window.devicePixelRatio;
            particleData[idx + 1] = (p_y + p_ty) * window.devicePixelRatio;
            particleData[idx + 2] = r / 255.0;
            particleData[idx + 3] = g / 255.0;
            particleData[idx + 4] = b / 255.0;
            particleData[idx + 5] = a / 255.0;
            particleData[idx + 6] = p_radius * 2.0 * window.devicePixelRatio;
          }

          // 4. ZEICHNEN
          gl!.bindBuffer(gl!.ARRAY_BUFFER, buffer);
          gl!.bufferSubData(gl!.ARRAY_BUFFER, 0, particleData);
          gl!.viewport(0, 0, canvas!.width, canvas!.height);
          gl!.clearColor(0, 0, 0, 0);
          gl!.clear(gl!.COLOR_BUFFER_BIT);
          gl!.useProgram(program);
          gl!.uniform2f(resLoc, canvas!.width, canvas!.height);
          gl!.enable(gl!.BLEND);
          gl!.blendFunc(gl!.SRC_ALPHA, gl!.ONE_MINUS_SRC_ALPHA);
          gl!.drawArrays(gl!.POINTS, 0, quantity);

          animationFrameId = requestAnimationFrame(render);
        }

        animationFrameId = requestAnimationFrame(render);
      })
      .catch(err => {
        console.error("🔥 KRITISCHER WASM FEHLER:", err);
      });

    return () => {
      if (animationFrameId) cancelAnimationFrame(animationFrameId);
      if (enginePtr && wasmInstance) {
        (wasmInstance.exports.cbps_engine_destroy as Function)(enginePtr, f_idx);
      }
      if (program) gl!.deleteProgram(program);
    };
  });
</script>

<svelte:window
  onpointermove={handleMouseMove}
  ondeviceorientation={handleDeviceOrientation}
  onresize={handleResize}
/>

<canvas
  bind:this={canvas}
  class={cn("fixed inset-0 w-full h-full z-0 animate-fade-in pointer-events-none bg-[#2e3440]")}
  aria-hidden="true"
></canvas>

<div class="fixed inset-0 z-10 pointer-events-none flex flex-col items-center justify-center space-y-6 select-none">

  <div class="w-[85vw] max-w-7xl shrink-0 rounded-full" style="height: 3px; background: linear-gradient(to right, #2e3440, #eceff4, #2e3440);"></div>

  <h1 class="text-[#eceff4] font-sans font-bold tracking-wider text-center text-4xl sm:text-5xl md:text-6xl lg:text-7xl xl:text-8xl">
    {h1}
  </h1>

  <div class="w-[85vw] max-w-7xl shrink-0 rounded-full" style="height: 3px; background: linear-gradient(to right, #2e3440, #eceff4, #2e3440);"></div>

  {#if h2.toLowerCase() === "comboom.sucht"}
    <img
      src="/pictures/cbps_logo.png"
      alt="comboom.sucht Logo"
      class="object-contain w-50 h-50"
    />
  {:else}
    <h2 class="text-[#eceff4] font-sans text-center text-2xl md:text-3xl lg:text-4xl">
      {h2}
    </h2>
  {/if}

</div>

{#if h1.toLowerCase() === "mcpeaps_hd"}
<img src="/pictures/mahd_logo.png" alt="mcpeaps_HD Logo" class="w-16 h-16 absolute bottom-6 right-6 pointer-events-none" />
{/if}
{#if h1.toLowerCase() === "blackdragon"}
<img src="/pictures/bd_logo.png" alt="BlackDragon Logo" class="w-16 h-16 absolute bottom-6 right-6 pointer-events-none" />
{/if}
{#if h1.toLowerCase() === "knuddelzwerck"}
<img src="/pictures/knuddelzwerck_logo.png" alt="Knuddelzwerck Logo" class="w-16 h-16 absolute bottom-6 right-6 pointer-events-none" />
{/if}
{#if h1.toLowerCase() === "fabelke"}
<img src="/pictures/fabelke_logo.png" alt="Fabelke Logo" class="w-16 h-16 absolute bottom-6 right-6 pointer-events-none" />
{/if}
