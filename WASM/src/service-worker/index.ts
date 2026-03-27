/// <reference types="@sveltejs/kit" />
/// <reference lib="webworker" />

declare let self: ServiceWorkerGlobalScope;

import { build, files, version } from "$service-worker";

const CACHE = `cache-${version}`;
const filteredFiles = files.filter((file) => !file.includes("/."));
const ASSETS = [...build, ...filteredFiles];

// install service worker

// 1. Filtere versteckte Dateien (wie /.assetsignore) aus dem SvelteKit-Array heraus,
// install service worker
self.addEventListener("install", (event) => {
  async function addFilesToCache() {
    const cache = await caches.open(CACHE);

    try {
      // Versuch 1: Alles auf einmal laden (schneller)
      await cache.addAll(ASSETS);
    } catch (err) {
      console.warn(
        "cache.addAll schlug fehl. Versuche Dateien einzeln zu laden...",
        err,
      );
      // Versuch 2: Falls EINE Datei kaputt ist, laden wir sie einzeln.
      // So stürzt nicht der gesamte Service Worker ab!
      for (const asset of ASSETS) {
        try {
          await cache.add(asset);
        } catch (e) {
          console.error(`Überspringe kaputte Datei für den Cache: ${asset}`, e);
        }
      }
    }
  }

  event.waitUntil(addFilesToCache());
});

// activate service worker

self.addEventListener("activate", (event) => {
  async function deleteOldCaches() {
    for (const key of await caches.keys()) {
      if (key !== CACHE) await caches.delete(key);
    }
  }

  event.waitUntil(deleteOldCaches());
});

//listen to fetch events

self.addEventListener("fetch", (event) => {
  if (event.request.method !== "GET") return;

  async function respond() {
    const url = new URL(event.request.url);
    const cache = await caches.open(CACHE);

    // serve build files from cache
    if (ASSETS.includes(url.pathname)) {
      const cachedResponse = await cache.match(url.pathname);
      if (cachedResponse) return cachedResponse;
    }

    // try the network first
    try {
      const response = await fetch(event.request);

      const isNotExtention =
        url.protocol === "http:" || url.protocol === "https:";

      const isSuccess = response.status === 200;

      if (isNotExtention && isSuccess) {
        cache.put(event.request, response.clone());
      }

      return response;
    } catch {
      // fallback  to cache

      const cachedResponse = await cache.match(event.request);
      if (cachedResponse) return cachedResponse;
    }
    return new Response("Not found", {
      status: 404,
    });
  }

  event.respondWith(respond());
});

self.addEventListener("message", (event) => {
  if (event.data && event.data.action === "skipWaiting") {
    self.skipWaiting();
  }
});
