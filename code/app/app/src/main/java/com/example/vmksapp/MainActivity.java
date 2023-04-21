package com.example.vmksapp;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private int PERMISSION_CODE = 1;
    static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        if (ContextCompat.checkSelfPermission(MainActivity.this,
                        Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(MainActivity.this, "You have already granted permission!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    requestBTPermission();
                }
     //   System.out.println("test");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
      //  System.out.println("test2");

        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        System.out.println(btAdapter.getBondedDevices());
//
        BluetoothDevice hc = btAdapter.getRemoteDevice("98:DA:50:01:83:F3");
        System.out.println(hc.getName());
//
        BluetoothSocket btSocket = null;
        int counter = 0;
        do {
            try {
                btSocket = hc.createRfcommSocketToServiceRecord(mUUID);
                System.out.println(btSocket);
                btSocket.connect();
                System.out.println(btSocket.isConnected());
            } catch (IOException e) {
                e.printStackTrace();
            }

            counter++;
        } while (!btSocket.isConnected() && counter < 3);
//
//
        try {
            OutputStream outputStream = btSocket.getOutputStream();
//            System.out.println("test3");
//            outputStream.write(49);
            //debugging
            initButtons(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
//
        InputStream inputStream;
        try {
            inputStream = btSocket.getInputStream();
            inputStream.skip(inputStream.available());
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    //All MACs (with module):[E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 78:21:84:E0:B8:CA, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //Without: [E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //Correct MAC: 78:21:84:E0:B8:CA/98:DA:50:01:83:F3

    public void initButtons(OutputStream outputStream) {
        createButtonListener(findViewById(R.id.button1), outputStream, 49);
        createButtonListener(findViewById(R.id.button2), outputStream, 50);
        createButtonListener(findViewById(R.id.button3), outputStream, 51);
        createButtonListener(findViewById(R.id.button4), outputStream, 52);
        createButtonListener(findViewById(R.id.button5), outputStream, 53);
        createButtonListener(findViewById(R.id.button6), outputStream, 54);
        createButtonListener(findViewById(R.id.button7), outputStream, 55);
        createButtonListener(findViewById(R.id.button8), outputStream, 56);
        createButtonListener(findViewById(R.id.button9), outputStream, 57);
        createButtonListener(findViewById(R.id.button10), outputStream, 58);
        createButtonListener(findViewById(R.id.button11), outputStream, 59);
        createButtonListener(findViewById(R.id.button12), outputStream, 60);
    }

    public void createButtonListener(Button btn, OutputStream outputStream, int message) {
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.out.println("Clicked " + btn.getText());
                System.out.println("Message " + message);
                try {
                    outputStream.write(message);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }

        });
    }

    private void requestBTPermission() {
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_ADVERTISE, Manifest.permission.BLUETOOTH_CONNECT}, PERMISSION_CODE);
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED && grantResults[1] == PackageManager.PERMISSION_GRANTED && grantResults[2] == PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(this, "Permission GRANTED", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, "Permission DENIED", Toast.LENGTH_SHORT).show();
            }
        }
    }
}