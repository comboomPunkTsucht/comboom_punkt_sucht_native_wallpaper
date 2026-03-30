<script lang="ts">
  import ReloadPrompt from '$lib/components/ReloadPrompt.svelte';
	import './layout.css';

	let { children } = $props();
</script>
<ReloadPrompt />
{@render children()}
