set(flysky_srcs "src/ppm8.c")

idf_component_register(SRCS "${flysky_srcs}"
                       INCLUDE_DIRS "include")
