set_device \
    -fam SmartFusion2 \
    -die PA4M1000_N \
    -pkg tq144
set_input_cfg \
	-path {C:/Users/jl/source/repos/hermess/hermess-MCU/hermess/component/work/root_sb_MSS/ENVM.cfg}
set_output_efc \
    -path {C:\Users\jl\source\repos\hermess\hermess-MCU\hermess\designer\root\root.efc}
set_proj_dir \
    -path {C:\Users\jl\source\repos\hermess\hermess-MCU\hermess}
set_is_relative_path \
    -value {FALSE}
set_root_path_dir \
    -path {}
gen_prg -use_init false
