from setuptools import find_packages, setup

setup(
    name="sonic-platform",
    version="1.0",
    description="SONiC platform API implementation for Asterfusion Platforms",
    license="Apache 2.0",
    author="Asterfusion CX-T Team",
    author_email="sunzheng@asterfusion.com",
    url="https://git.asterfusion.com/projects/SONIC/repos/sonic-buildimage",
    maintainer="Asterfusion CX-T Team",
    maintainer_email="sunzheng@asterfusion.com",
    packages=find_packages(),
    package_data={"sonic_platform": ["pltfm_utils/logging.conf"]},
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Environment :: Plugins",
        "Intended Audience :: Developers",
        "Intended Audience :: Information Technology",
        "Intended Audience :: System Administrators",
        "License :: OSI Approved :: Apache Software License",
        "Natural Language :: English",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python :: 3.9",
        "Topic :: Utilities",
    ],
    keywords="sonic SONiC platform PLATFORM",
)
