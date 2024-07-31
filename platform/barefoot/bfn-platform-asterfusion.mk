BFN_INGRASYS_PLATFORM = bfnplatform-asterfusion-master_1.0.0_amd64.deb
$(BFN_INGRASYS_PLATFORM)_URL = "https://github.com/asterfusion/bf-bsp-lts/releases/download/24.06/bfnplatform-asterfusion-master_1.0.0_amd64.deb"

SONIC_ONLINE_DEBS += $(BFN_ASTERFUSION_PLATFORM) # $(BFN_SAI_DEV)
$(BFN_SAI_DEV)_DEPENDS += $(BFN_ASTERFUSION_PLATFORM)
