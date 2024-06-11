
#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "detection_responder.cc"
        "image_provider.cc"
        "main.cc"
        "main_functions.cc"
        "model_settings.cc"
        "person_detect_model_data.cc"
        "app_camera_esp.c"
        "esp_cli.c"
	"wifi.c"
	"tcp_client.c"

    PRIV_REQUIRES console static_images spi_flash esp_netif esp_wifi nvs_flash
    INCLUDE_DIRS "")
