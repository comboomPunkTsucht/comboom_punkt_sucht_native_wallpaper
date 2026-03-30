<script lang="ts">
import { onMount } from "svelte";

async function detectSWUpdate() {
  const registration = await navigator.serviceWorker.ready;

  registration.addEventListener("updatefound", () => {
    const newSW = registration.installing;

    if (newSW) {
      newSW.addEventListener("statechange", () => {
        if (newSW.state === "installed") {
          if ( window.confirm("A new version is available. Please refresh the page.") ) {
                newSW.postMessage({ action: "skipWaiting" });
                window.location.reload();
              }
        }
      });
    }
  });
}

onMount(() => {
  detectSWUpdate();
});
</script>
