require("lspconfig").clangd.setup({
	cmd = { "clangd", "--compile-commands-dir=build/Emscripten-Debug", "--query-driver=**/em++,**/clang*" },
})

return {}
