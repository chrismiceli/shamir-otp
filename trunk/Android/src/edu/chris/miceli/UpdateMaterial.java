package edu.chris.miceli;

import java.math.BigInteger;

public class UpdateMaterial {
    public BigInteger getGeneratorShare() {
        return generatorShare;
    }

    public void setGeneratorShare(BigInteger share1) {
        this.generatorShare = share1;
    }

    public BigInteger getAuthenticatorShare() {
        return authenticatorShare;
    }

    public void setAuthenticatorShare(BigInteger share2) {
        this.authenticatorShare = share2;
    }

    public BigInteger getCoefficient() {
        return coefficient;
    }

    public void setCoefficient(BigInteger coefficient) {
        this.coefficient = coefficient;
    }

    private BigInteger generatorShare;
    private BigInteger authenticatorShare;
    private BigInteger coefficient;
    /*
     * private BigInteger E0; private BigInteger E1; private BigInteger r;
     * private BigInteger rOfi;
     */
}
