# Introduction #

An Android application is part of the Shamir OTP project that enables any Android powered device to act as a hardware token that generates passwords. The Application must first be initialized with the profile of the user. After being initialized, the application can begin to produce useful passwords.

# Initialization #
The first time the application is launched, a configuration page will be displayed that allows you to enter in the data that is provided by the profile. The information can be retrieved from the profile easily by using the `shamir-print-profile` command-line tool.

`shamir-print-profile client_profile`

This will label the data in the profile to allow you to enter it into the token.

# Utilization #
To use the token, simply open the application. A screen will be displayed that prompts the user when they are ready to generate the next password. By clicking the `Display OTP` button, another screen will launch.

This screen contains all the information that is required by the authenticator to authenticate the user. The screen also displays the information required to update the share. Some information from the authenticator will have to be entered into the application to complete the process.