package com.example.vmksapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        System.out.println(btAdapter.getBondedDevices());

        BluetoothDevice esp32 = btAdapter.getRemoteDevice("78:21:84:E0:B8:CA");
        System.out.println(esp32.getName());

        BluetoothSocket btSocket = null;
        int counter = 0;
        do {
            try {
                btSocket = esp32.createRfcommSocketToServiceRecord(mUUID);
                System.out.println(btSocket);
                btSocket.connect();
                System.out.println(btSocket.isConnected());
            } catch (IOException e) {
                e.printStackTrace();
            }

            counter++;
        } while (!btSocket.isConnected() && counter < 3);


        try {
            OutputStream outputStream = btSocket.getOutputStream();
            System.out.println("bruh");
            outputStream.write(49);


            initButtons(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }

        InputStream inputStream = null;
        try {
            inputStream = btSocket.getInputStream();
            inputStream.skip(inputStream.available());

            for (int i = 0; i < 26; i++) {

                byte b = (byte) inputStream.read();
                System.out.println((char) b);

            }

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    //All MACs (with ESP):[E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 78:21:84:E0:B8:CA, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //Without ESP: [E0:62:67:47:19:AD, 55:0F:63:31:54:3D, FC:58:FA:2F:4C:2C, AA:F0:36:2F:B6:63, 80:57:19:E8:43:8F, 41:42:4C:F8:B4:A0, 58:FB:84:86:29:04, FF:FF:38:59:B5:B3, FC:58:FA:4B:E8:7F]
    //ESP MAC: 78:21:84:E0:B8:CA

    public void initButtons(OutputStream outputStream ) {
        createButtonListener((Button) findViewById(R.id.button1), outputStream, 49);
        createButtonListener((Button) findViewById(R.id.button2), outputStream, 50);
        createButtonListener((Button) findViewById(R.id.button3), outputStream, 51);
        createButtonListener((Button) findViewById(R.id.button4), outputStream, 52);
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
}
