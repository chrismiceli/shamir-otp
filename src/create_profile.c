#include <stdio.h>
#include "shamir_OTP.h"

const unsigned long SECURITY = 8; /* 64 bit security for now */

int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      fprintf(stderr, "NAME\n");
      fprintf(stderr, "      shamir-create-profile - create a profile for use with shamirOTP\n\n");
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "      shamir-create-profile FILE\n\n");
      fprintf(stderr, "DESCRIPTION\n");
      fprintf(stderr, "      Create a profile and save it to FILE.\n\n");
      return 0;
   }
   else 
   {
      profile_t profile;

      printf("Creating profile...");
      create_generator_profile(&profile, SECURITY);
      fflush(stdout);

      printf("done\n");

      printf("Saving profile...");
      if(save_profile(profile, argv[1]) < 0)
      {
         perror("Error saving profile");
      }
      printf("done\n");
   }
   return 0;
}
