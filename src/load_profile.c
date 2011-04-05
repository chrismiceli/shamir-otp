#include <stdio.h>
#include "shamir_OTP.h"

int main(int argc, char *argv[])
{
   if(argc < 3)
   {
      fprintf(stderr, "NAME\n");
      fprintf(stderr, "      shamir-load-profile - Load a profile for use with shamirOTP\n\n");
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "      shamir-load-profile CLIENT_FILE OUTPUT_FILE\n\n");
      fprintf(stderr, "DESCRIPTION\n");
      fprintf(stderr, "      Load a profile saved at FILE and create the");
      fprintf(stderr, "corresponding authenticator profile to OUTPUT_FILE.\n\n");
      return 0;
   }
   else 
   {
      profile_t authenticator_profile;
      profile_t generator_profile;

      printf("Loading client profile...");
      fflush(stdout);

      if(load_profile(&generator_profile, argv[1]) < 0)
      {
         perror("Error loading profile");
         return 0;
      }

      /* Overwrite the client's share with the authenticator's */
      create_authenticator_profile(&authenticator_profile, generator_profile);
      printf("done\n");

      printf("Saving authenticator profile...");
      fflush(stdout);
      if(save_profile(authenticator_profile, argv[2]) < 0)
      {
         perror("Error saving profile");
         return 0;
      }
      printf("done\n");
   }
   return 0;
}
