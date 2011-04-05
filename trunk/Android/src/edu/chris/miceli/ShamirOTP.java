package edu.chris.miceli;

import java.math.BigInteger;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

public class ShamirOTP {
    public final static String KEY_PROFILE_NAME = "edu.chris.miceli";
    public final static String KEY_SECRET = "edu.chris.miceli.secret";
    public final static String KEY_SHARE = "edu.chris.miceli.share";
    public final static String KEY_P = "edu.chris.miceli.p";
    public final static String KEY_Q = "edu.chris.miceli.q";
    public final static String KEY_G = "edu.chris.miceli.g";
    public final static String KEY_H = "edu.chris.miceli.h";

    private String profileName;
    private BigInteger secret;
    private BigInteger share;
    private BigInteger p;
    private BigInteger q;
    private BigInteger g;
    private BigInteger h;

    public ShamirOTP(String profileName, String secret, String share, String p,
            String q, String g, String h) {
        this.profileName = profileName;
        this.secret = new BigInteger(secret, 16);
        this.share = new BigInteger(share, 16);
        this.p = new BigInteger(p, 16);
        this.q = new BigInteger(q, 16);
        this.g = new BigInteger(g, 16);
        this.h = new BigInteger(h, 16);
    }

    public String getOTP() {
        return share.toString(16);
    }

    public UpdateMaterial beginUpdate() {
        UpdateMaterial updateMaterial = new UpdateMaterial();

        updateMaterial.setCoefficient(getRandomNumber(q));
        /* f(1) = secret + coefficient * 1 */
        BigInteger f1 = secret.add(updateMaterial.getCoefficient());
        f1 = f1.mod(q);
        updateMaterial.setGeneratorShare(f1);

        /* f(2) = secret + coefficient * 2 */
        BigInteger f2 = updateMaterial.getCoefficient().multiply(
                BigInteger.valueOf(2));
        f2 = f2.add(secret);
        f2 = f2.mod(q);
        updateMaterial.setAuthenticatorShare(f2);

        return updateMaterial;
    }

    public VerificationMaterial getVerification(UpdateMaterial updateMaterial) {
        VerificationMaterial verificationMaterial = new VerificationMaterial();
        BigInteger r1 = getRandomNumber(q);
        BigInteger r2 = getRandomNumber(q);

        /* Evaluate for authenticator r(2) */
        verificationMaterial.rOfI = r2.multiply(BigInteger.valueOf(2));
        verificationMaterial.rOfI = verificationMaterial.rOfI.add(r1);
        verificationMaterial.rOfI = verificationMaterial.rOfI.mod(q);

        verificationMaterial.E0 = VerificationMaterial.E(this, secret, r1);
        verificationMaterial.E1 = VerificationMaterial.E(this,
                updateMaterial.getCoefficient(), r2);
        return verificationMaterial;
    }

    public boolean verify(String updateMaterial, String E0, String E1,
            String rOfI) {
        BigInteger updateMaterialBigInteger = new BigInteger(updateMaterial, 16);
        BigInteger E0BigInteger = new BigInteger(E0, 16);
        BigInteger E1BigInteger = new BigInteger(E1, 16);
        BigInteger rOfIBigInteger = new BigInteger(rOfI, 16);

        /* Calculate E(update material, rOfI) */
        BigInteger E = VerificationMaterial.E(this, updateMaterialBigInteger,
                rOfIBigInteger);
        if (E.compareTo(E0BigInteger.multiply(E1BigInteger).mod(p)) == 0) {
            return true;
        }
        return false;
    }

    public void finalizeUpdate(UpdateMaterial updateMaterial,
            String fromAuthenticator) {
        BigInteger fromAuthenticatorBigInteger = new BigInteger(
                fromAuthenticator, 16);
        share = share.add(fromAuthenticatorBigInteger).add(
                updateMaterial.getGeneratorShare());
        share = share.mod(q);
        secret = secret.multiply(BigInteger.valueOf(3));
        secret = secret.mod(q);
    }

    public String getProfileName() {
        return profileName;
    }

    public BigInteger getG() {
        return g;
    }

    public BigInteger getH() {
        return h;
    }

    public BigInteger getP() {
        return p;
    }

    public BigInteger getQ() {
        return q;
    }

    public static ShamirOTP loadFromPreferences(
            SharedPreferences sharedPreferences) {
        String profileName;
        String secret;
        String share;
        String p;
        String q;
        String g;
        String h;

        profileName = sharedPreferences.getString(KEY_PROFILE_NAME, "");
        if (profileName == "") {
            return null;
        }
        secret = sharedPreferences.getString(KEY_SECRET, "");
        share = sharedPreferences.getString(KEY_SHARE, "");
        p = sharedPreferences.getString(KEY_P, "");
        q = sharedPreferences.getString(KEY_Q, "");
        g = sharedPreferences.getString(KEY_G, "");
        h = sharedPreferences.getString(KEY_H, "");
        return new ShamirOTP(profileName, secret, share, p, q, g, h);
    }

    public void saveToPreferences(SharedPreferences sharedPreferences) {
        Editor editor = sharedPreferences.edit();
        editor.putString(KEY_PROFILE_NAME, profileName);
        editor.putString(KEY_SECRET, secret.toString(16));
        editor.putString(KEY_SHARE, share.toString(16));
        editor.putString(KEY_P, p.toString(16));
        editor.putString(KEY_Q, q.toString(16));
        editor.putString(KEY_G, g.toString(16));
        editor.putString(KEY_H, h.toString(16));
        editor.commit();
    }

    private BigInteger getRandomNumber(BigInteger modulation) {
        /* Use default in case we can't get a better one later */
        SecureRandom secureRandom = new SecureRandom();
        byte[] bytes = new byte[512];
        try {
            secureRandom = SecureRandom.getInstance("SHA1PRNG");
        } catch (NoSuchAlgorithmException e) {
            Log.v("ShamirOTP", "Falling back to default SecureRandom!");
            e.printStackTrace();
        }
        /* Force internal seeding */
        secureRandom.nextBytes(bytes);
        BigInteger retval = new BigInteger(bytes);
        retval = retval.abs();
        retval = retval.mod(modulation);
        return retval;
    }
}
