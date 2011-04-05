package edu.chris.miceli;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class Configuration extends Activity {
    public static final int FORMAT_ERROR_DIALOG = 1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.configuration_layout);
    }

    @Override
    public void onResume() {
        super.onResume();
        Button saveButton = (Button) findViewById(R.id.saveButton);
        saveButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                EditText profileNameEditText = (EditText) findViewById(R.id.profileNameEditText);
                EditText secretEditText = (EditText) findViewById(R.id.secretEditText);
                EditText shareEditText = (EditText) findViewById(R.id.shareEditText);
                EditText pEditText = (EditText) findViewById(R.id.pEditText);
                EditText qEditText = (EditText) findViewById(R.id.qEditText);
                EditText gEditText = (EditText) findViewById(R.id.gEditText);
                EditText hEditText = (EditText) findViewById(R.id.hEditText);

                try {
                    ShamirOTP shamirOTP = new ShamirOTP(profileNameEditText
                            .getText().toString(), secretEditText.getText()
                            .toString(), shareEditText.getText().toString(),
                            pEditText.getText().toString(), qEditText.getText()
                                    .toString(),
                            gEditText.getText().toString(), hEditText.getText()
                                    .toString());
                    shamirOTP.saveToPreferences(getSharedPreferences(
                            Profile.KEY_SHARED_PREFS, MODE_PRIVATE));
                    Intent intent = new Intent(Configuration.this,
                            Profile.class);
                    startActivity(intent);
                    finish();
                } catch (NumberFormatException e) {
                    showDialog(FORMAT_ERROR_DIALOG);
                }
            }
        });
    }

    @Override
    protected Dialog onCreateDialog(int id, Bundle args) {
        Dialog dialog;
        if (id == FORMAT_ERROR_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle(R.string.format_error_title);
            builder.setMessage(R.string.format_error_message);
            builder.setPositiveButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int id) {
                            dismissDialog(FORMAT_ERROR_DIALOG);
                        }
                    });
            dialog = builder.create();
        }
        else {
            dialog = super.onCreateDialog(id, args);
        }
        return dialog;
    }
}
