set_component root_sb_CCC_0_FCCC
# Microsemi Corp.
<<<<<<< HEAD
# Date: 2023-Feb-13 08:37:25
=======
# Date: 2022-Nov-27 18:04:54
>>>>>>> e34d31ef438c63e3bfd3979803f72878cb389376
#

create_clock -period 20 [ get_pins { CCC_INST/RCOSC_25_50MHZ } ]
create_generated_clock -multiply_by 2 -divide_by 2 -source [ get_pins { CCC_INST/RCOSC_25_50MHZ } ] -phase 0 [ get_pins { CCC_INST/GL0 } ]
