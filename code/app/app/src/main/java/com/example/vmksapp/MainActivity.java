package com.example.vmksapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initButtons();

        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        System.out.println(btAdapter.getBondedDevices());
    }
    //[E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 78:21:84:E0:B8:CA, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //[E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //78:21:84:E0:B8:CA
    public void initButtons() {
        createButtonListener((Button) findViewById(R.id.button1));
        createButtonListener((Button) findViewById(R.id.button2));
        createButtonListener((Button) findViewById(R.id.button3));
    }

    public void createButtonListener(Button btn) {
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.out.println("Clicked " + btn.getText());
            }
        });
    }
}
