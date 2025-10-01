require("lspconfig").clangd.setup({
	cmd = { "clangd", "--compile-commands-dir=build/Clang-Wayland-Debug" },
})

return {}
