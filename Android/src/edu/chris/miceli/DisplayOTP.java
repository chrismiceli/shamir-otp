package edu.chris.miceli;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class DisplayOTP extends Activity {
    public static final int FORMAT_ERROR_DIALOG = 1;
    public static final int VERIFICATION_ERROR_DIALOG = 2;
    private Button finishButton;
    private TextView profileNameTextView;
    private TextView shareTextView;
    private TextView forAuthenticatorUpdateEditText;
    private TextView forAuthenticatorE0EditText;
    private TextView forAuthenticatorE1EditText;
    private TextView forAuthenticatorREditText;
    private EditText fromAuthenticatorUpdateEditText;
    private EditText fromAuthenticatorE0EditText;
    private EditText fromAuthenticatorE1EditText;
    private EditText fromAuthenticatorREditText;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.display_layout);
    }

    @Override
    public void onResume() {
        super.onResume();
        SharedPreferences sharedPreferences = getSharedPreferences(
                Profile.KEY_SHARED_PREFS, MODE_PRIVATE);

        finishButton = (Button) findViewById(R.id.FinishButton);
        profileNameTextView = (TextView) findViewById(R.id.profileNameTextView);
        shareTextView = (TextView) findViewById(R.id.shareTextView);
        forAuthenticatorUpdateEditText = (TextView) findViewById(R.id.forAuthenticatorUpdateMaterialTextView);
        forAuthenticatorE0EditText = (TextView) findViewById(R.id.forAuthenticatorE0TextView);
        forAuthenticatorE1EditText = (TextView) findViewById(R.id.forAuthenticatorE1TextView);
        forAuthenticatorREditText = (TextView) findViewById(R.id.forAuthenticatorRTextView);
        fromAuthenticatorUpdateEditText = (EditText) findViewById(R.id.fromAuthenticatorUpdateMaterialEditText);
        fromAuthenticatorE0EditText = (EditText) findViewById(R.id.fromAuthenticatorE0EditText);
        fromAuthenticatorE1EditText = (EditText) findViewById(R.id.fromAuthenticatorE1EditText);
        fromAuthenticatorREditText = (EditText) findViewById(R.id.fromAuthenticatorREditText);

        ShamirOTP shamirOTP = ShamirOTP.loadFromPreferences(sharedPreferences);

        profileNameTextView.setText(shamirOTP.getProfileName());
        shareTextView.setText(shamirOTP.getOTP());

        UpdateMaterial updateMaterial = shamirOTP.beginUpdate();
        forAuthenticatorUpdateEditText.setText(updateMaterial
                .getAuthenticatorShare().toString(16));

        VerificationMaterial verificationMaterial = shamirOTP
                .getVerification(updateMaterial);
        forAuthenticatorE0EditText.setText(verificationMaterial.getE0());
        forAuthenticatorE1EditText.setText(verificationMaterial.getE1());
        forAuthenticatorREditText.setText(verificationMaterial.getROfI());

        finishButton.setOnClickListener(new FinishButtonOnClickListener(
                shamirOTP, updateMaterial));
    }

    @Override
    protected Dialog onCreateDialog(int id, Bundle args) {
        Dialog dialog;
        if (id == FORMAT_ERROR_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.format_error_title);
            builder.setMessage(R.string.format_error_message);
            builder.setCancelable(true);
            builder.setPositiveButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dismissDialog(FORMAT_ERROR_DIALOG);
                        }
                    });
            dialog = builder.create();
        }
        else if (id == VERIFICATION_ERROR_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.verification_error_title);
            builder.setMessage(R.string.verification_error_message);
            builder.setCancelable(true);
            builder.setPositiveButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dismissDialog(VERIFICATION_ERROR_DIALOG);
                        }
                    });
            dialog = builder.create();
        }
        else {
            dialog = super.onCreateDialog(id, args);
        }
        return dialog;
    }

    private class FinishButtonOnClickListener implements OnClickListener {
        private ShamirOTP shamirOTP;
        private UpdateMaterial updateMaterial;

        public FinishButtonOnClickListener(ShamirOTP shamirOTP,
                UpdateMaterial updateMaterial) {
            this.shamirOTP = shamirOTP;
            this.updateMaterial = updateMaterial;
        }

        @Override
        public void onClick(View v) {
            try {
                String fromAuthenticatorUpdate = fromAuthenticatorUpdateEditText
                        .getText().toString();
                String fromAuthenticatorE0 = fromAuthenticatorE0EditText
                        .getText().toString();
                String fromAuthenticatorE1 = fromAuthenticatorE1EditText
                        .getText().toString();
                String fromAuthenticatorR = fromAuthenticatorREditText
                        .getText().toString();

                if (shamirOTP.verify(fromAuthenticatorUpdate,
                        fromAuthenticatorE0, fromAuthenticatorE1,
                        fromAuthenticatorR) == true) {
                    shamirOTP.finalizeUpdate(updateMaterial,
                            fromAuthenticatorUpdate);
                    shamirOTP.saveToPreferences(getSharedPreferences(
                            Profile.KEY_SHARED_PREFS, MODE_PRIVATE));
                    finish();
                }
                else {
                    showDialog(VERIFICATION_ERROR_DIALOG);
                }
            } catch (NumberFormatException e) {
                showDialog(Configuration.FORMAT_ERROR_DIALOG);
            }
        }
    }
}
