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
#define USERSDIR "/etc/security/shamir-otp"
#define BUFSIZE   512


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>

/*#include <security/pam_appl.h>*/

#define PAM_SM_AUTH
#include <security/pam_modules.h>
#include "shamir_OTP.h"

#ifdef SOLARIS
#define PAM_EXTERN
#endif

/* logging function ripped from pam_listfile.c */
void pam_log(int err, const char *format, ...) {
   va_list args; 
   va_start(args, format);
   openlog("pam_shamir_otp", LOG_CONS|LOG_PID, LOG_AUTH);
   vsyslog(err, format, args);
   va_end(args);
   closelog();
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh,
                                   int flags,
                                   int argc,
                                   const char **argv) {    
   int retval, i;
   const char *usersdir = USERSDIR;
   const char *user;
   struct pam_conv *conv;
   char passcode[BUFSIZE];
   short debug = 0;
   short use_first_pass = 0;

   fprintf(stderr, "pam_sm_authenticate\n");
   /* read options */
   for (i = 0; i < argc; i++)
   {  
      if(strncmp(argv[i], "users=", strlen("users=")) == 0)
      {
         usersdir = strchr(argv[i], '=');
         usersdir++;
      }
      else if(strcmp(argv[i], "debug") == 0)
      {
         debug=1;
      }
      else if(strcmp(argv[i], "use_first_pass") == 0)
      {
         use_first_pass=1;
      }
      else 
      {
         pam_log(LOG_ERR,"option not implemented: %s",argv[i]);
      }
   }

   retval = pam_get_item (pamh, PAM_CONV, (const void **) &conv);
   if(retval != PAM_SUCCESS) 
   {
      return retval;
   }

   /* read user name */
   if ((retval = pam_get_user(pamh, &user, NULL)) != PAM_SUCCESS)
   {
      pam_log(LOG_ERR, "error: get username");
      return retval;
   }
   pam_log(LOG_NOTICE, "user: %s\n", user);

   /* get passcode */
   if (use_first_pass)
   {
      char *p;
      retval = pam_get_item(pamh,PAM_AUTHTOK,(void *)&p);
      if(retval != PAM_SUCCESS)
      {
         return PAM_AUTHINFO_UNAVAIL;
      }
      if(p) 
      {
         strncpy(passcode, p, BUFSIZE);
         passcode[BUFSIZE - 1] = 0;
      }
      fprintf(stderr,"old passcode: %s\n",passcode);
      if(debug)
      {
         pam_log(LOG_DEBUG, "using old passcode (%s) use_first_pass.");
      }
   }
   else {
      struct pam_message msg, *pmsg[1];
      struct pam_response *resp;
      msg.msg = "passcode: ";
      msg.msg_style = PAM_PROMPT_ECHO_OFF;
      pmsg[0] = &msg;
      
      retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
      if(retval != PAM_SUCCESS) 
      {
         return retval;
      }
      if(!resp)
      {
         return PAM_CONV_ERR;
      }
      strncpy(passcode, resp->resp, BUFSIZE);
      passcode[BUFSIZE - 1] = '\0';
      if(resp) 
      {
         free(resp);
      }
   }

   fprintf(stderr,"user: %s, passcode: %s\n", user, passcode);
  
   /* read secret from users file */
   {
      mpz_t otp;          /* The provided OTP */
      profile_t profile;  /* The loaded profile from users file */
      char userfile[BUFSIZE];
      char buffer[BUFSIZE];
      struct pam_message msg, *pmsg[1];
      struct pam_response *resp;

      snprintf(userfile, BUFSIZE, "%s/%s.conf", usersdir, user);
      pam_log(LOG_NOTICE, "userfile = %s\n", userfile);
      if(load_profile(&profile, userfile) < 0)
      {
         pam_log(LOG_NOTICE, "could not load user file %s\n", userfile);
         return PAM_USER_UNKNOWN;
      }


      mpz_init_set_str(otp, passcode, 16);
      if(check_otp(profile, otp) == 0)
      {
         /* Provided to authetnciator and generated by generator */
         verification_t for_generator_verification;
         /* Given to the generator provided by authenticator */
         verification_t from_generator_verification;
         /* Provided by generator */
         mpz_t from_generator_material;
         /* Update information */
         update_t update;

         pam_log(LOG_ERR, "The OTP is valid.\n");

         /* Get our share and their share */
         generate_update_material(profile, &update);

         /* Output information for user */
         mpz_init(from_generator_material);
         snprintf(buffer, BUFSIZE, "Please provide the Update number to your OTP generator: %s", 
                                   mpz_get_str(NULL, 16, update.for_generator));
         msg.msg = buffer;
         msg.msg_style = PAM_TEXT_INFO;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(resp)
         {
            free(resp);
         }

         /* Request information from user */
         msg.msg = "Please enter the Update number provided by your OTP generator: ";
         msg.msg_style = PAM_PROMPT_ECHO_OFF;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(!resp)
         {
            return PAM_CONV_ERR;
         }
         else
         {
            mpz_init_set_str(from_generator_material, resp->resp, 16);
            free(resp);
         }

         /* Output verification information */
         generate_verification_material(profile, &for_generator_verification, update);
         snprintf(buffer, BUFSIZE, "Please provide the E0 number to your OTP generator: %s", 
                                   mpz_get_str(NULL, 16, for_generator_verification.E0));
         msg.msg = buffer;
         msg.msg_style = PAM_TEXT_INFO;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(resp)
         {
            free(resp);
         }
         snprintf(buffer, BUFSIZE, "Please provide the E1 number to your OTP generator: %s", 
                                   mpz_get_str(NULL, 16, for_generator_verification.E1));
         msg.msg = buffer;
         msg.msg_style = PAM_TEXT_INFO;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(resp)
         {
            free(resp);
         }
         snprintf(buffer, BUFSIZE, "Please provide the r number to your OTP generator: %s", 
                                   mpz_get_str(NULL, 16, for_generator_verification.rOfI));
         msg.msg = buffer;
         msg.msg_style = PAM_TEXT_INFO;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(resp)
         {
            free(resp);
         }

         /* Accept verification information */
         mpz_init(from_generator_verification.E0);
         mpz_init(from_generator_verification.E1);
         mpz_init(from_generator_verification.rOfI);
         msg.msg = "Please enter the E0 number provided by your generator: ";
         msg.msg_style = PAM_PROMPT_ECHO_OFF;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(!resp)
         {
            return PAM_CONV_ERR;
         }
         else
         {
            mpz_init_set_str(from_generator_verification.E0, resp->resp, 16);
            free(resp);
         }
         msg.msg = "Please enter the E1 number provided by your generator: ";
         msg.msg_style = PAM_PROMPT_ECHO_OFF;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(!resp)
         {
            return PAM_CONV_ERR;
         }
         else
         {
            mpz_init_set_str(from_generator_verification.E1, resp->resp, 16);
            free(resp);
         }
         msg.msg = "Please enter the r number provided by your generator: ";
         msg.msg_style = PAM_PROMPT_ECHO_OFF;
         pmsg[0] = &msg;
         retval = conv->conv (1, (const struct pam_message **)pmsg, &resp, conv->appdata_ptr);
         if(retval != PAM_SUCCESS) 
         {
            return retval;
         }
         if(!resp)
         {
            return PAM_CONV_ERR;
         }
         else
         {
            mpz_init_set_str(from_generator_verification.rOfI, resp->resp, 16);
            free(resp);
         }

         /* Finalize update */
         if(verify_update_material(profile,
                                   from_generator_verification,
                                   from_generator_material) == 0)
         {
            pam_log(LOG_NOTICE, "The OTP was updated successfully.\n");
            update_profile(&profile, update.for_authenticator, from_generator_material);
            /* Save profile */
            save_profile(profile, userfile);
            return PAM_SUCCESS;
         }
         else
         {
            pam_log(LOG_ERR, "Error: The update material appears to be malicious!\n");
            return PAM_AUTH_ERR;
         }
      }
      else
      {
         pam_log(LOG_NOTICE, "The OTP is invalid.\n");
         return PAM_AUTH_ERR;
      }
   }
}


PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, 
                              int flags,
                              int argc,
                              const char **argv)
{
   return PAM_SUCCESS;
}

#ifdef PAM_STATIC
struct pam_module _pam_mobile_otp_modstruct = {
  "pam_shamir_otp",
  pam_sm_authenticate,
  pam_sm_setcred,
  NULL,
  NULL,
  NULL,
  NULL,
};
#endif