shared_module("SkrGuiRenderer", "SKR_GUI_RENDERER", engine_version)
    public_dependency("SkrGui", engine_version)
    public_dependency("SkrRenderGraph", engine_version)
    add_includedirs("include", {public=true})
    add_includedirs("src", {public=false})
    add_files("src/**.cpp")
    -- add render graph shaders
    add_rules("c++.unity_build", {batchsize = default_unity_batch_size})
    add_rules("utils.dxc", {
        spv_outdir = "/../resources/shaders/GUI", 
        dxil_outdir = "/../resources/shaders/GUI"}
    )
    add_rules("c++.codegen", {
        files = {"include/**.h", "include/**.hpp"},
        rootdir = "include/SkrGuiRenderer/",
        api = "SKR_GUI_RENDERER"
    })
    add_files("shaders/*.hlsl")
    if (is_os("windows") or is_os("macosx")) then 
        public_dependency("SkrImageCoder", engine_version)
        add_defines("SKR_GUI_RENDERER_USE_IMAGE_CODER")
    end