idf_component_register(SRCS "HTTP_Server.c" "WIFI_StaAp.c" "main.c" "HTTP_Client.c" "ov2640.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES howsmyssl_com_root_cert.pem
                    postman_root_cert.pem)
