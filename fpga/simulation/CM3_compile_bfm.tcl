# ===========================================================
<<<<<<< HEAD
# Created by Microsemi SmartDesign Mon Feb 13 08:37:21 2023
=======
# Created by Microsemi SmartDesign Sun Nov 27 18:04:50 2022
>>>>>>> e34d31ef438c63e3bfd3979803f72878cb389376
# 
# Warning: Do not modify this file, it may lead to unexpected 
#          simulation failures in your design.
#
# ===========================================================

if {$tcl_platform(os) == "Linux"} {
  exec "$env(ACTEL_SW_DIR)/bin64/bfmtovec" -in test.bfm -out test.vec
} else {
  exec "$env(ACTEL_SW_DIR)/bin64/bfmtovec.exe" -in test.bfm -out test.vec
}
