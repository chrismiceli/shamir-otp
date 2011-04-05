#ifndef SHAMIR_OTP_H_INCLUDED
#define SHAMIR_OTP_H_INCLUDED

#include <gmp.h>

/**
 * The type of a participant in the Shamir one-time password scheme.
 */
typedef enum
{
/**
 * The generator of the password.
 */
   SHAMIR_GENERATOR,
/**
 * The authenticator of the password.
 */
   SHAMIR_AUTHENTICATOR
} role_t;

/**
 * A profile that stores settings for a Shamir one-time password setup.
 */
typedef struct {
/**
 * Unique identifier for this setup.
*/
   long uid;

/**
 * Secret value for this setup.
*/
   mpz_t secret;

/**
 * Current share (a one-time password) for this profile.
*/
   mpz_t share;

/**
 * Maximum value allowed for g, h, and E commitment 
 * operations.
 * p is prime
*/
   mpz_t p;

/**
 * Maximum value allowed for secret, share, and update values
 * q is prime
 * q | p - 1
*/
   mpz_t q;

/**
 * Used in computation of verification material
 * Belongs to the cyclic subgroup of Zp* of order q.
*/
   mpz_t g;

/**
 * Used in computation of verification material
 * Belongs to the cyclic subgroup of Zp* of order q.
*/
   mpz_t h;

/**
 * Role this profile plays in this instance of the password system.
*/
   role_t role;
} profile_t;

/**
 * Structure describes the information necessary to verify update
 * material.
 */
typedef struct {
/**
 * The commitment on the secret bound to a coefficient of the r
 * polynomial.
 */
   mpz_t E0;
/**
 * The commitment on the update material bound to a coefficient of the
 * r polynomial.
 */
   mpz_t E1;
/**
 * The r polynomial evaluated at either 1 or 2 depending on the role.
 */
   mpz_t rOfI;
} verification_t;

/**
 * Structure describing the information necessary to perform an update.
 */
typedef struct {
/**
 * The update material intended for the generator.
 */
   mpz_t for_generator;
/**
 * The update material intended for the authenticator.
 */
   mpz_t for_authenticator;
/**
 * The coefficient used in generating the update material.
 * This is used in the verification process.
 */
   mpz_t coefficient;
} update_t;

/**
 * Prints a profile to standard out.
 *
 * @param profile Profile to be printed
*/
void print_profile(const profile_t profile);

/**
 * Load a profile from disk.
 *
 * Loads the Shamir one-time password profile from the file
 * specified at filename into a profile.
 *
 * @param profile Used to store the loaded profile.
 * @param filename Name of the file to load the profile from.
 * @return 0 if successful, -1 otherwise. errno is set appropriately
*/
int load_profile(profile_t *profile, const char *filename);

/**
 * Save a profile to disk.
 *
 * Saves the Shamir one-time password profile specified 
 * at filename to the file on disk specified at filename.
 *
 * @param profile Profile written to disk.
 * @param filename Name of the file to write the profile to.
 * @return 0 if successful, -1 otherwise. errno is set appropriately
*/
int save_profile(const profile_t profile, const char *filename);

/**
 * Check a one-time password to authenticate the user who provided this
 * password.
 * 
 * @param profile Current profile to check the password against. This is
 * the authenticator's profile that is associated with the generator's
 * profile.
 * @param otp Shamir one-time password to check.
 * @return 0 if the password is valid, -1 otherwise.
*/
int check_otp(const profile_t profile, const mpz_t otp);

/**
 * Gets the current Shamir one-time password for the generator's
 * profile.
 * 
 * @param rop Used to store the Shamir one-time password extracted from
 * profile.
 * @param profile Profile used to extract the Shamir one-time password
 * form.
*/
void get_otp(mpz_t rop, const profile_t profile);

/**
 * Generates a profile for an authenticator.
 * 
 * Create a profile for an authenticator from a generator_profile and
 * store it in authenticator_profile.
 *
 * @param authenticator_profile Used to store the profile created from
 * generator_profile.
 * @param generator_profile Used to create a profile for the
 * authenticator.
*/
void create_authenticator_profile(profile_t *authenticator_profile, const profile_t generator_profile);

/**
 * Generates a profile for the generator (client).
 * 
 * Create a profile for a generator (client desiring future
 * authentication) that has security_bits of security.
 *
 * @param generator_profile Used to store the created generator profile.
 * @param security_bits The number of bits of security provided.
*/
void create_generator_profile(profile_t *generator_profile, const long security_bits);

/**
 * Create update material.
 * 
 * Create material that is used by update_profile to update a profile
 * to have the next valid Shamir one-time password.
 *
 * @param profile Profile of the party creating the update material.
 * @param update Used to store the resulting update material that can be
 * used to update the password via update_profile.
*/
void generate_update_material(const profile_t profile, update_t *update);

/**
 * Updates a profile to be ready for the next Shamir one-time password
 * 
 * Use material created via generate_update_material to update the
 * provided profile.
 *
 * @param profile The profile to be updated in preparation for the next
 * Shamir one-time password.
 * @param authenticator_material Material created by the authenticator
 * from generate_update_material.
 * @param generator_material Material created by the generator from
 * generate_update_material.
*/
void update_profile(profile_t *profile, const mpz_t authenticator_material, const mpz_t generator_material);

/**
 * Verifies whether update material is corrupt or malicious.
 * 
 * Verifies whether the update material update_material is valid
 * in the given profile context with information provided by
 * verification.
 *
 * @param profile The profile of the One-Time password that is desiring
 * to be updated with the update material.
 * @param verification Information provided by the other party used to
 * determine the validity of the update_material.
 * @param update_material The update material to be verified for
 * authenticity.
 * @return 0 for successful verification, -1 otherwise.
*/
int verify_update_material(const profile_t profile, const verification_t verification, const mpz_t update_material);

/**
 * Generates material to be utilized to verify update material.
 * 
 * Populate verification with information for the other
 * participating party associated with this profile.
 *
 * @param profile The profile of the one-time password currently in
 * question.
 * @param verification Structure to be populated with information to be
 * utilized by the other participating member.
 * @param update The update material that is going to be
 * verified by the other party.
*/
void generate_verification_material(const profile_t profile, verification_t *verification, const update_t update);

#endif /* SHAMIR_OTP_H_INCLUDED */
