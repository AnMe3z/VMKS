package com.example.vmksapp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button buttonOne = (Button) findViewById(R.id.button1);
        Button buttonTwo = (Button) findViewById(R.id.button2);
        Button buttonThree = (Button) findViewById(R.id.button3);

        buttonIsPressed(buttonOne);
        buttonIsPressed(buttonTwo);
        buttonIsPressed(buttonThree);
    }

    public void buttonIsPressed(Button btn) {
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.out.println("Clicked " + btn.getText() + " nigga");
            }
        });
    }
}
