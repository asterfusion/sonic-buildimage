#######################################################################
# Asterfusion CX-T Devices Components                                 #
#                                                                     #
# Component contains an implementation of SONiC Platform Base API and #
# provides the components firmware management function                #
#                                                                     #
#######################################################################

try:
    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.component_base import ComponentBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Component(ComponentBase):
    """Platform-specific Component class"""

    def __init__(self, component_index, helper, hwver, hwsku, platform):
        ComponentBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.board = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init index
        self.component_index = component_index + 1
        self._init_component_info()
        self._init_component_version()

    def _init_component_info(self):
        component_info = COMPONENT.get(self.hwsku)
        assert component_info is not None, "invalid component info list"
        component_info_list = component_info.get("INFO")
        self._component_info = component_info_list[self.component_index - 1]

    def _init_component_version(self):
        self._version = self._api_helper.get_component_version(
            self._component_info.get("type"), self._component_info.get("target")
        )
        assert self._version != None, "invalid component version"

    def get_name(self):
        """
        Retrieves the name of the component

        Returns:
            A string containing the name of the component
        """
        return self._component_info.get("name")

    def get_description(self):
        """
        Retrieves the description of the component

        Returns:
            A string containing the description of the component
        """
        return self._component_info.get("desc")

    def get_firmware_version(self):
        """
        Retrieves the firmware version of the component

        Note: the firmware version will be read from HW

        Returns:
            A string containing the firmware version of the component
        """
        return self._version

    def get_available_firmware_version(self, image_path):
        """
        Retrieves the available firmware version of the component

        Note: the firmware version will be read from image

        Args:
            image_path: A string, path to firmware image

        Returns:
            A string containing the available firmware version of the component
        """
        raise NotImplementedError

    def get_firmware_update_notification(self, image_path):
        """
        Retrieves a notification on what should be done in order to complete
        the component firmware update

        Args:
            image_path: A string, path to firmware image

        Returns:
            A string containing the component firmware update notification if required.
            By default 'None' value will be used, which indicates that no actions are required
        """
        return None

    def install_firmware(self, image_path):
        """
        Installs firmware to the component

        This API performs firmware installation only: this may/may not be the same as firmware update.
        In case platform component requires some extra steps (apart from calling Low Level Utility)
        to load the installed firmware (e.g, reboot, power cycle, etc.) - this must be done manually by user

        Note: in case immediate actions are required to complete the component firmware update
        (e.g., reboot, power cycle, etc.) - will be done automatically by API and no return value provided

        Args:
            image_path: A string, path to firmware image

        Returns:
            A boolean, True if install was successful, False if not
        """
        raise NotImplementedError

    def update_firmware(self, image_path):
        """
        Updates firmware of the component

        This API performs firmware update: it assumes firmware installation and loading in a single call.
        In case platform component requires some extra steps (apart from calling Low Level Utility)
        to load the installed firmware (e.g, reboot, power cycle, etc.) - this will be done automatically by API

        Args:
            image_path: A string, path to firmware image

        Raises:
            RuntimeError: update failed
        """
        raise NotImplementedError

    def auto_update_firmware(self, image_path, boot_type):
        """
        Updates firmware of the component

        This API performs firmware update automatically based on boot_type: it assumes firmware installation
        and/or creating a loading task during the reboot, if needed, in a single call.
        In case platform component requires some extra steps (apart from calling Low Level Utility)
        to load the installed firmware (e.g, reboot, power cycle, etc.) - this will be done automatically during the reboot.
        The loading task will be created by API.

        Args:
            image_path: A string, path to firmware image
            boot_type: A string, reboot type following the upgrade
                         - none/fast/warm/cold

        Returns:
            Output: A return code
                return_code: An integer number, status of component firmware auto-update
                    - return code of a positive number indicates successful auto-update
                        - status_installed = 1
                        - status_updated = 2
                        - status_scheduled = 3
                    - return_code of a negative number indicates failed auto-update
                        - status_err_boot_type = -1
                        - status_err_image = -2
                        - status_err_unknown = -3

        Raises:
            RuntimeError: auto-update failure cause
        """
        raise NotImplementedError
