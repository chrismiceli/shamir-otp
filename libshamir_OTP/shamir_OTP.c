#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "shamir_OTP.h"

void print_profile(const profile_t profile)
{
   printf("LOADED profile: \n");
   printf("uid: %lx\n", profile.uid);
   gmp_printf("secret: %Zx\n", profile.secret);
   gmp_printf("share: %Zx\n", profile.share);
   gmp_printf("p: %Zx\n", profile.p);
   gmp_printf("q: %Zx\n", profile.q);
   gmp_printf("g: %Zx\n", profile.g);
   gmp_printf("h: %Zx\n", profile.h);
   if(profile.role == SHAMIR_AUTHENTICATOR)
   {
      printf("role: SHAMIR_AUTHENTICATOR\n");
   }
   else
   {
      printf("role: SHAMIR_GENERATOR\n");
   }
}

int load_profile(profile_t *profile, const char *filename)
{
   int temp;
   FILE *file = fopen(filename, "r");

   if(file == NULL)
   {
      return -1;
   }
   if(fscanf(file, "%lx", &(profile->uid)) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   mpz_init(profile->secret);
   mpz_init(profile->share);
   mpz_init(profile->p);
   mpz_init(profile->q);
   mpz_init(profile->g);
   mpz_init(profile->h);
   if(gmp_fscanf(file, "%Zx", profile->secret) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(gmp_fscanf(file, "%Zx", profile->share) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(gmp_fscanf(file, "%Zx", profile->p) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(gmp_fscanf(file, "%Zx", profile->q) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(gmp_fscanf(file, "%Zx", profile->g) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(gmp_fscanf(file, "%Zx", profile->h) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   if(fscanf(file, "%d", &temp) <= 0)
   {
      errno = EINVAL;
      return -1;
   }
   else
   {
      profile->role = temp;
   }
#ifdef SHAMIR_DEBUG
   printf("LOADED profile: \n");
   print_profile(*profile);
#endif
   if(fclose(file) != 0)
   {
      return -1;
   }
   return 0;
}

int save_profile(const profile_t profile, const char *filename)
{
   FILE *file = fopen(filename, "w");
   if(file == NULL)
   {
      return -1;
   }
   fprintf(file, "%lx ", profile.uid);
   gmp_fprintf(file, "%Zx ", profile.secret);
   gmp_fprintf(file, "%Zx ", profile.share);
   gmp_fprintf(file, "%Zx ", profile.p);
   gmp_fprintf(file, "%Zx ", profile.q);
   gmp_fprintf(file, "%Zx ", profile.g);
   gmp_fprintf(file, "%Zx ", profile.h);
   fprintf(file, "%d ", profile.role);
#ifdef SHAMIR_DEBUG
   printf("Saved profile: \n");
   print_profile(profile);
#endif
   if(fclose(file) != 0)
   {
      return -1;
   }
   return 0;
}

/**
 * Retrieve a secure random number
 *
 * Retrieves a secure random number size bits long and
 * stores the result in rop.
 *
 * @param rop Used to store the retrieved random number.
 * @param size Number of bits long result is to be.
 * @return 0 if successful, -1 otherwise. errno is set appropriately
*/
int get_random_number(mpz_t rop, const mpz_t modulation)
{
   /* for now assume 512 digits is enough */
   unsigned int size = 512;
   char buffer[size]; 
   FILE *file = fopen("/dev/urandom", "r");
   if(file == NULL)
   {
      return -1;
   }
   int read = fread(buffer, 1, size, file);
   if(read <= 0)
   {
      errno = EIO;
      return -1;
   }
   mpz_import(rop, size, 1, 1, 0, 0, buffer);
   mpz_mod(rop, rop, modulation);
   if(fclose(file) != 0)
   {
      return -1;
   }
   return 0;
}

int check_otp(const profile_t profile, const mpz_t otp)
{
   mpz_t difference;
   mpz_t secret;
   int result;

   mpz_init(difference);
   mpz_init(secret);
   /* Compute difference between authenticator share and client share */
   mpz_sub(difference, profile.share, otp);
   /* Since we are working with lines, the secret is the same distance
    * away from the client share */
   mpz_sub(secret, otp, difference);
   mpz_mod(secret, secret, profile.q);
   result = mpz_cmp(secret, profile.secret);
   if(result != 0)
   {
      return -1;
   }
   else
   {
      return 0;
   }
}

void get_otp(mpz_t rop, const profile_t profile)
{
   mpz_init_set(rop, profile.share);
}

void create_authenticator_profile(profile_t *authenticator_profile, const profile_t generator_profile)
{
   mpz_t difference;

   authenticator_profile->uid = generator_profile.uid;
   mpz_init_set(authenticator_profile->secret, generator_profile.secret);
   mpz_init_set(authenticator_profile->share, generator_profile.share);
   mpz_init_set(authenticator_profile->p, generator_profile.p);
   mpz_init_set(authenticator_profile->q, generator_profile.q);
   mpz_init_set(authenticator_profile->g, generator_profile.g);
   mpz_init_set(authenticator_profile->h, generator_profile.h);
   authenticator_profile->role = SHAMIR_AUTHENTICATOR;
   
   mpz_init(difference);
   mpz_sub(difference, generator_profile.share, generator_profile.secret);
   mpz_add(authenticator_profile->share, generator_profile.share, difference);
   mpz_mod(authenticator_profile->share, authenticator_profile->share,
      authenticator_profile->q);

   mpz_clear(difference);
}

void get_p_and_q(mpz_t p, mpz_t q, const mpz_t min)
{
   mpz_t min2; /* We need a number bigger than minimum, but different so
   we don't all have the same p and q */
   mpz_init(p);
   mpz_init(q);
   mpz_init(min2);

   mpz_mul_ui(min2, min, 2);
   while(1)
   {
      /* Bad way to generate a random number larger than min */
      get_random_number(q, min2);
      if(mpz_cmp(q, min) > 0)
      {
         break;
      }
   }

   /* Get a prime number larger than q */
   mpz_nextprime(q, q);
   mpz_t k;
   mpz_init_set_ui(k, 2);
   while(1)
   {
      /* 2 * q * k + 1 ?= prime */
      mpz_t result;
      mpz_init(result);

      mpz_mul_ui(result, k, 2);
      mpz_mul(result, result, q);
      mpz_add_ui(result, result, 1);
      if(mpz_probab_prime_p(result, 15) == 1)
      {

#ifdef SHAMIR_DEBUG
         gmp_printf("k = %Zd\n", k);
#endif
         mpz_set(p, result);
         mpz_clear(result);
         mpz_clear(k);
         return;
      }
      else
      {
         mpz_add_ui(k, k, 1);
      }
   }
}

void get_element_from_Gq(mpz_t rop, const mpz_t p, const mpz_t q)
{
   /* To generate g and h, pick random numbers until they turn out to
    * generate G_q */
   mpz_t exponentiated;
   mpz_init(exponentiated);
   get_random_number(rop, p);
   while(1)
   {
      mpz_powm(exponentiated, rop, q, p);
      if(mpz_cmp_ui(exponentiated, 1) == 0)
      {
         break;
      }
      mpz_add_ui(rop, rop, 1);
      mpz_mod(rop, rop, p);
   }
   mpz_clear(exponentiated);
}

void create_generator_profile(profile_t *generator_profile, const long security_bits)
{
   mpz_t uid;
   mpz_t a;
   /* Minimum number than can exist at security_bits in length */
   mpz_t security_min; 

   mpz_init(uid);
   mpz_init(a);

   mpz_init(generator_profile->secret);
   mpz_init(generator_profile->share);
   mpz_init(generator_profile->p);
   mpz_init(generator_profile->q);
   mpz_init(generator_profile->g);
   mpz_init(generator_profile->h);
   mpz_init(security_min);

   mpz_ui_pow_ui(security_min, 2, security_bits);
   get_p_and_q(generator_profile->p, generator_profile->q, security_min);

   get_element_from_Gq(generator_profile->g, generator_profile->p, generator_profile->q);
   get_element_from_Gq(generator_profile->h, generator_profile->p, generator_profile->q);
   get_random_number(generator_profile->secret, generator_profile->q);
   get_random_number(a, generator_profile->q);
   mpz_add(generator_profile->share, a, generator_profile->secret);
   mpz_mod(generator_profile->share, generator_profile->share, generator_profile->q);
   get_random_number(uid, generator_profile->p);
   generator_profile->uid = labs(mpz_get_ui(uid));
   generator_profile->role = SHAMIR_GENERATOR;

   mpz_clear(a);
   mpz_clear(uid);
   mpz_clear(security_min);
}

void generate_update_material(const profile_t profile, update_t *update)
{
   mpz_t difference;

   /* Generate a polynomial with f(0) = secret, f(1) = random, f(2) such
    * that f is a line */
   mpz_init(update->for_authenticator);
   mpz_init(update->for_generator);
   mpz_init(update->coefficient);

   /* fix coefficient */
   get_random_number(update->coefficient, profile.q);
   /* Calculate f(1) */
   mpz_add(update->for_generator, profile.secret, update->coefficient);
   mpz_mod(update->for_generator, update->for_generator, profile.q);

   /* Calculate f(2) */
   mpz_mul_ui(update->for_authenticator, update->coefficient, 2);
   mpz_add(update->for_authenticator, profile.secret, update->for_authenticator);
   mpz_mod(update->for_authenticator, update->for_authenticator, profile.q);
}

void update_profile(profile_t *profile, const mpz_t authenticator_material, const mpz_t generator_material)
{
   mpz_mul_ui(profile->secret, profile->secret, 3);
   mpz_mod(profile->secret, profile->secret, profile->q);

   mpz_add(profile->share, profile->share, generator_material);
   mpz_add(profile->share, profile->share, authenticator_material);
   mpz_mod(profile->share, profile->share, profile->q);
}

void shamir_E(mpz_t rop, const profile_t profile, const mpz_t arg1, const mpz_t arg2)
{
   mpz_t gPowArg1;
   mpz_t hPowArg2;

   mpz_init(gPowArg1);
   mpz_init(hPowArg2);
   mpz_init(rop);

   mpz_powm(gPowArg1, profile.g, arg1, profile.p);
   mpz_powm(hPowArg2, profile.h, arg2, profile.p); 
   mpz_mul(rop, gPowArg1, hPowArg2);
   mpz_mod(rop, rop, profile.p);

   mpz_clear(gPowArg1);
   mpz_clear(hPowArg2);
}

int verify_update_material(const profile_t profile, const verification_t verification, const mpz_t update_material)
{
   mpz_t E; /* E(update_material, r(2)) */
   mpz_t intermediate; /* E0 + E1 or E0 + E1^2 */
   int result;
   
   mpz_init(E);
   mpz_init(intermediate);

   shamir_E(E, profile, update_material, verification.rOfI);
#ifdef SHAMIR_DEBUG
   gmp_printf("E(update material, rOfI) = %Zx = %Zx ^ %Zx * %Zx ^ %Zx\n",
   E, profile.g, update_material, profile.h, verification.rOfI);
#endif

   /* If we are the authenticator, we need to exponentiate */
   if(profile.role == SHAMIR_AUTHENTICATOR)
   {
      mpz_pow_ui(intermediate, verification.E1, 2);
      mpz_mod(intermediate, intermediate, profile.p);
#ifdef SHAMIR_DEBUG
      gmp_printf("E1 = E1^2 = %Zx ^ 2 = %Zx\n", verification.E1,
      intermediate);
#endif
   }
   else
   {
#ifdef SHAMIR_DEBUG
      gmp_printf("E1 = E1^1 = %Zx ^ 1 = %Zx\n", verification.E1,
      intermediate);
#endif
      mpz_set(intermediate, verification.E1);
   }
   mpz_mul(intermediate, verification.E0, intermediate);
   mpz_mod(intermediate, intermediate, profile.p); 
#ifdef SHAMIR_DEBUG
      gmp_printf("E0*E1^i = %Zx\n", intermediate);
#endif

   result = mpz_cmp(intermediate, E);
   mpz_clear(intermediate);
   mpz_clear(E);

   if(result != 0)
   {
      return -1;
   }
   else
   {
      return 0;
   }
}

void generate_verification_material(const profile_t profile, verification_t *verification, const update_t update)
{
   mpz_t r, r1;
   mpz_init(r);
   mpz_init(r1);
   mpz_init(verification->E0);
   mpz_init(verification->E1);
   mpz_init(verification->rOfI);
  
   /* Construct a random polynomial r(x) = r + r1 * x */
   get_random_number(r, profile.q);
   get_random_number(r1, profile.q);
#ifdef SHAMIR_DEBUG
   gmp_printf("r(x) = %Zx + %Zx*x\n", r, r1);
#endif

   /* Calculate E0 = E(secret, r) */
   shamir_E(verification->E0, profile, profile.secret, r); 
#ifdef SHAMIR_DEBUG
   gmp_printf("E0 = %Zx = %Zx ^ %Zx * %Zx ^ %Zx\n", verification->E0,
   profile.g, profile.secret, profile.h, r);
#endif

   /* Calculate eithr r(1) or r(2) depending on role */
   if(profile.role == SHAMIR_AUTHENTICATOR)
   {
      /* r(1) = r + r1 */
      mpz_add(verification->rOfI, r, r1);
      mpz_mod(verification->rOfI, verification->rOfI, profile.q);
#ifdef SHAMIR_DEBUG
      gmp_printf("r(1) = %Zx = %Zx + %Zx * 1\n", verification->rOfI, r,
      r1);
#endif
   }
   else
   {
      /* profile.role == SHAMIR_GENERATOR */
      /* r(2) = r + r1 * 2 */
      mpz_mul_ui(verification->rOfI, r1, 2);
      mpz_add(verification->rOfI, r, verification->rOfI);
      mpz_mod(verification->rOfI, verification->rOfI, profile.q);
#ifdef SHAMIR_DEBUG
      gmp_printf("r(2) = %Zx = %Zx + %Zx * 2\n", verification->rOfI, r,
      r1);
#endif
   }

   /* Calculate E1 = E(coefficient in update function, r1) */
   shamir_E(verification->E1, profile, update.coefficient, r1);
#ifdef SHAMIR_DEBUG
   gmp_printf("E1 = %Zx = %Zx ^ %Zx * %Zx ^ %Zx\n", verification->E1,
   profile.g, update.coefficient, profile.h, r1);
#endif

   mpz_clear(r);
   mpz_clear(r1);
}
