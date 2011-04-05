/* Copyright (C) 2011
  This file is part of the shamir OTP library
  
  Shamir-otp is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Shamir-otp is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA */
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
