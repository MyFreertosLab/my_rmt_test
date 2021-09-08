set(rc_srcs "src/rc.c")

idf_component_register(SRCS "${rc_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES main nvs_flash app_update
                      )
