############################################################################
# Asterfusion CX-T Devices PCIE                                            #
#                                                                          #
# Platform and model specific pcie subclass, inherits from the base class, #
# provides the pcie info which are available in the platform               #
#                                                                          #
############################################################################

try:
    import yaml

    from sonic_platform_base.sonic_pcie.pcie_common import PcieUtil
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Pcie(PcieUtil):
    """Platform-specific EEPROM class"""

    def __init__(self, *args, **kwargs):
        super(Pcie, self).__init__(*args, **kwargs)
        # Load config file
        self.load_config_file()

    # load the config file
    def load_config_file(self):
        conf_rev = "_{}".format(self._conf_rev) if self._conf_rev else ""
        config_file = "{}/pcie{}.yaml".format(self.config_path, conf_rev)
        try:
            with open(config_file) as conf_file:
                self.confInfo = yaml.safe_load(conf_file)
        except IOError as err:
            print("Config file is missing. Generating...")
            self.dump_conf_yaml()
