idf_component_register(
    INCLUDE_DIRS "include"
    EMBED_FILES "websensact_builder/dist_browser_compressed/${NODE_ID}/websensact.html.gz"
    REQUIRES esp_http_server common esp_partition)
