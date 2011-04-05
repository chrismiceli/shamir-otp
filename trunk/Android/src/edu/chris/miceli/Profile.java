package edu.chris.miceli;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class Profile extends Activity {
    public final static String LOG_TAG = "edu.chris.miceli.Profile";
    public final static String KEY_SHARED_PREFS = "edu.chris.miceli";
    public final static int ABOUT_DIALOG = 1;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.profile_layout);
    }

    @Override
    public void onResume() {
        super.onResume();
        SharedPreferences sharedPreferences = getSharedPreferences(
                KEY_SHARED_PREFS, MODE_PRIVATE);
        ShamirOTP shamirOTP = ShamirOTP.loadFromPreferences(sharedPreferences);
        if (shamirOTP == null) {
            // If we don't a profile, we need to direct the user to enter
            // one.
            Intent intent = new Intent(Profile.this, Configuration.class);
            startActivity(intent);
            finish();
        }
        else {
            TextView profileNameTextView = (TextView) findViewById(R.id.profileNameTextView);
            profileNameTextView.setText(shamirOTP.getProfileName());
            Button displayOTPButton = (Button) findViewById(R.id.displayOTPButton);
            displayOTPButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(Profile.this, DisplayOTP.class);
                    startActivity(intent);
                }
            });
        }
    }

    @Override
    protected Dialog onCreateDialog(int id, Bundle args) {
        Dialog dialog;
        if (id == ABOUT_DIALOG) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.about_title);
            builder.setMessage(R.string.about_message);
            builder.setCancelable(true);
            builder.setPositiveButton(R.string.ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dismissDialog(ABOUT_DIALOG);
                        }
                    });
            dialog = builder.create();
        }
        else {
            dialog = super.onCreateDialog(id, args);
        }
        return dialog;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.profile_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
            case R.id.new_profile:
                Intent intent = new Intent(Profile.this, Configuration.class);
                startActivity(intent);
                finish();
                return true;
            case R.id.about:
                showDialog(ABOUT_DIALOG);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}