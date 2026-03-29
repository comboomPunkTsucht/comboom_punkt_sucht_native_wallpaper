<script lang="ts">
  import { cn } from "$lib/utils";
  import { z } from "zod";
  import { page } from "$app/state";
  import { Canvas } from '@threlte/core';
  import Scene from '$lib/components/Scene.svelte';

  const SearchParamsSchema = z.object({
    h1: z.string().optional().default("mcpeaps_HD"),
    h2: z.string().optional().default("comboom.sucht"),
  });

  // 1. Parameter nativ auslesen
  let params = $derived(SearchParamsSchema.parse(Object.fromEntries(page.url.searchParams)));
  let h1 = $derived(params.h1);
  let h2 = $derived(params.h2);

  let canvasSize = $state({ w: 0, h: 0 });

  let titleFontSize = $derived(Math.max(24, Math.min(160, canvasSize.w / 10)));
  let subFontSize = $derived(Math.max(12, Math.min(48, canvasSize.w / 30)));
  let lineWidth = $derived(canvasSize.w * 0.85);

  let BRLogoTargetH = $derived(Math.max(24, Math.min(128, canvasSize.h * 0.06)));
  let CBPSLogoTargetH = $derived(Math.max(48, Math.min(192, canvasSize.h * 0.12)));
</script>

<svelte:window bind:innerWidth={canvasSize.w} bind:innerHeight={canvasSize.h} />

<div class={cn("fixed inset-0 w-full h-full z-0 animate-fade-in pointer-events-none bg-[#2e3440]")} aria-hidden="true">
  {#if canvasSize.w > 0}
    <Canvas>
      <Scene bind:h1={h1} bind:h2={h2} bind:canvasSize={canvasSize} quantity={400} />
    </Canvas>
  {/if}
</div>

<div class="fixed inset-0 z-10 pointer-events-none flex flex-col items-center justify-center space-y-6 select-none">
  <div class="shrink-0 rounded-full" style="width: {lineWidth}px; height: 3px; background: linear-gradient(to right, #2e3440, #eceff4, #2e3440);"></div>

  <h1 class="text-[#eceff4] font-sans tracking-wider text-center" style="font-size: {titleFontSize}px;">
    {h1}
  </h1>

  <div class="shrink-0 rounded-full" style="width: {lineWidth}px; height: 3px; background: linear-gradient(to right, #2e3440, #eceff4, #2e3440);"></div>

  {#if h2.toLowerCase() === "comboom.sucht"}
    <img src="/pictures/cbps_logo.png" alt="comboom.sucht Logo" class="object-contain w-auto" style="height: {CBPSLogoTargetH}px;" />
  {:else}
    <h2 class="text-[#eceff4] font-sans text-center" style="font-size: {subFontSize}px;">
      {h2}
    </h2>
  {/if}
</div>

{#if h1.toLowerCase() === "mcpeaps_hd"}
  <img src="/pictures/mahd_logo.png" alt="mcpeaps_HD Logo" class="absolute bottom-4 right-4 pointer-events-none object-contain w-auto" style="height: {BRLogoTargetH}px;" />
{/if}
{#if h1.toLowerCase() === "blackdragon"}
  <img src="/pictures/bd_logo.png" alt="BlackDragon Logo" class="absolute bottom-4 right-4 pointer-events-none object-contain w-auto" style="height: {BRLogoTargetH}px;" />
{/if}
{#if h1.toLowerCase() === "knuddelzwerck"}
  <img src="/pictures/knuddelzwerck_logo.png" alt="Knuddelzwerck Logo" class="absolute bottom-4 right-4 pointer-events-none object-contain w-auto" style="height: {BRLogoTargetH}px;" />
{/if}
{#if h1.toLowerCase() === "fabelke"}
  <img src="/pictures/fabelke_logo.png" alt="Fabelke Logo" class="absolute bottom-4 right-4 pointer-events-none object-contain w-auto" style="height: {BRLogoTargetH}px;" />
{/if}
