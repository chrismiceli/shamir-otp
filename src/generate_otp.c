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
   if(argc < 2)
   {
      fprintf(stderr, "NAME\n");
      fprintf(stderr, "      shamir-generate-otp - generate the next OTP\n\n");
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "      shamir-test-otp PROFILE\n\n");
      fprintf(stderr, "DESCRIPTION\n");
      fprintf(stderr, "      Generate a one-time password for a profile PROFILE.\n\n");
      return 0;
   }
   else 
   {
      profile_t profile;
      /* Provided to authetnciator and generated by generator */
      verification_t for_authenticator_verification;
      /* Given to the generator provided by authenticator */
      verification_t from_authenticator_verification;
      mpz_t otp;
      /* Provided by authenticator */
      mpz_t from_authenticator_material; 
      /* Update material */
      update_t update;

      load_profile(&profile, argv[1]);
      get_otp(otp, profile);

      gmp_printf("one-time password: %Zx\n", otp);

      /* Generate a random polynomial */
      generate_update_material(profile, &update);

      /* Request information from user */
      mpz_init(from_authenticator_material);
      printf("Please enter the Update number provided by your authenticator:\n");
      gmp_scanf("%Zx", from_authenticator_material);

      /* Output information for user */
      printf("Please provide the Update number to your authenticator: ");
      gmp_printf("%Zx\n", update.for_authenticator);

      /* Accept verification information */
      mpz_init(from_authenticator_verification.E0);
      mpz_init(from_authenticator_verification.E1);
      mpz_init(from_authenticator_verification.rOfI);
      printf("Please enter the E0 number provided by your authenticator:\n");
      gmp_scanf("%Zx", from_authenticator_verification.E0);
      printf("Please enter the E1 number provided by your authenticator:\n");
      gmp_scanf("%Zx", from_authenticator_verification.E1);
      printf("Please enter the r number provided by your authenticator:\n");
      gmp_scanf("%Zx", from_authenticator_verification.rOfI);

      /* Finalize update */
      if(verify_update_material(profile, 
                                from_authenticator_verification,
                                from_authenticator_material) == 0)
      {
         /* Output verification information */
         generate_verification_material(profile, &for_authenticator_verification, update);
         printf("Please provide the E0 number to your OTP generator: ");
         gmp_printf("%Zx\n", for_authenticator_verification.E0);
         printf("Please provide the E1 number to your OTP generator: ");
         gmp_printf("%Zx\n", for_authenticator_verification.E1);
         printf("Please provide the r number to your OTP generator: ");
         gmp_printf("%Zx\n", for_authenticator_verification.rOfI);

         update_profile(&profile, from_authenticator_material, update.for_generator);
         /* Save profile */
         printf("Saving updated profile...");
         fflush(stdout);
         save_profile(profile, argv[1]);
         printf("done\n");
      }
      else
      {
         printf("Error! The update information seems to be corrupted!\n");
      }
   }
   return 0;
}
