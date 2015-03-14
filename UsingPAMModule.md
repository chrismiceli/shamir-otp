# Introduction #

This article will discuss how to compile, install, and use the PAM module.


# Details #

# Compilation #
Some prerequisites are required to build the PAM module. The C library libshamir\_otp must be able to build or else the PAM module will fail to build. The PAM module also requires that the PAM development library be installed.

Configure will determine whether your system is capable of building the PAM module. If configure is complaining, please check config.log for messages. If configure determined your system is capable of building the module, running `make` will build the module.

# Installation #
If properly built, running `make install` will install the module into /lib/security as this is the directory on most machines for PAM modules. If this is incorrect, you must edit the pamdir variable in the Makefile to point to the correct installation directory.

# Configuration #
Now that the module is successfully installed, to configure it for a PAM capable application, you need to add a line to the PAM configuration file for that application. The location of this directory is system dependent but most probably resides in /etc/pam.d/application\_name. A line as follows demonstrates basic usage of the module:

`auth  required libshamir_otp.so not_set_pass`

Now for each user that the system will authenticate against, there has to be some initial setup. An authenticator profile must be created with the client profile. To do this, we can run the command-line utility provided by the C library.

`shamir-load-profile client_profile authenticator_profile`

Now we need to install this authenticator profile into the directory `/etc/security/shamir-otp/` under the name of the user to be authenticated. So if this authenticator profile is for the user with the username `username`, then we can install the file as the name `username.conf`.

`cp authenticator_profile /etc/security/shamir-otp/username.conf`

# Utilization #
Now that the module is compiled, installed, and configured, it can be used by using the application it was configured for and following the on screen instructions. The module works great with the command line utility `shamir-generate-otp`.