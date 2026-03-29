import adapter from "@sveltejs/adapter-vercel";
import { type Config } from "@sveltejs/kit";

const config: Config = {
	kit: {
		adapter: adapter(),
		serviceWorker: {
			register: true,
			options: {
				type: "module",
				updateViaCache: "all",
			},
		},
	},
	vitePlugin: {
		inspector: true,
	},
	compilerOptions: {
		dev: process.env.NODE_ENV === "development",
		hmr: process.env.NODE_ENV === "development",
	},
	prebundleSvelteLibraries: true,
};

export default config;
