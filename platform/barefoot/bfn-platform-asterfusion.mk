BFN_ASTERFUSION_PLATFORM = bfnplatform-asterfusion-master_1.0.0_amd64.deb
$(BFN_ASTERFUSION_PLATFORM)_URL = "https://github.com/asterfusion/bf-bsp-lts/releases/download/23.09/bfnplatform-asterfusion-master_1.0.0_amd64.deb"

SONIC_ONLINE_DEBS += $(BFN_ASTERFUSION_PLATFORM)
$(BFN_SAI_DEV)_DEPENDS += $(BFN_ASTERFUSION_PLATFORM)
