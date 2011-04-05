package edu.chris.miceli;

import java.math.BigInteger;

public class VerificationMaterial {
    public BigInteger E0;
    public BigInteger E1;
    public BigInteger rOfI;

    public static BigInteger E(ShamirOTP shamirOTP, BigInteger arg1,
            BigInteger arg2) {
        BigInteger p = shamirOTP.getP();
        BigInteger part1 = shamirOTP.getG().modPow(arg1, p);
        BigInteger part2 = shamirOTP.getH().modPow(arg2, p);
        BigInteger retval = part1.multiply(part2).mod(p);
        return retval;
    }

    public String getE0() {
        return E0.toString(16);
    }

    public String getE1() {
        return E1.toString(16);
    }

    public String getROfI() {
        return rOfI.toString(16);
    }

}
