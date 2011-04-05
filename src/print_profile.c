#include <stdio.h>
#include "shamir_OTP.h"

int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      fprintf(stderr, "NAME\n");
      fprintf(stderr, "      shamir-print-profile - print a profile for use with shamirOTP\n\n");
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "      shamir-print-profile PROFILE\n\n");
      fprintf(stderr, "DESCRIPTION\n");
      fprintf(stderr, "      Print a profile to standard out.\n\n");
      return 0;
   }
   else 
   {
      profile_t profile;
      load_profile(&profile, argv[1]);
      print_profile(profile);
   }
   return 0;
}
