import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { viteSingleFile } from "vite-plugin-singlefile";
import viteCompression from "vite-plugin-compression";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react(), viteSingleFile(), viteCompression()],
  server: {
    proxy: {
      // string shorthand
      "/api": "http://localhost:3000",
    },
  },
});
