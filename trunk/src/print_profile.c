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
