package com.timelapsify;


import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiNetworkSpecifier;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;



public class MainActivity extends AppCompatActivity {
    private WifiManager wifiManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ConstraintLayout loader = findViewById(R.id.loader);
        TextView loader_txt = findViewById(R.id.loader_txt);
        ImageView error = findViewById(R.id.error);
        Button reload_btn = findViewById(R.id.reload);

        ConstraintLayout webview_wrp = findViewById(R.id.webview_wrp);
        WebView webview = findViewById(R.id.webview);

        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);


        reload_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                error.setVisibility(View.INVISIBLE);
                reload_btn.setVisibility(View.INVISIBLE);

                go(loader, loader_txt, error, reload_btn, webview_wrp, webview);
            }
        });

        go(loader, loader_txt, error, reload_btn, webview_wrp, webview);
    }


    private void go(ConstraintLayout loader ,TextView loader_txt, ImageView error, Button reload_btn, ConstraintLayout webview_wrp, WebView webview) {
        try {
            loader_txt.setText("Establish wifi connection...");

            if (!wifiManager.isWifiEnabled()){
                error.setVisibility(View.VISIBLE);
                reload_btn.setVisibility(View.VISIBLE);
                loader_txt.setText("Wifi is disabled!");
                return;
            }

            WifiNetworkSpecifier.Builder builder = new WifiNetworkSpecifier.Builder();

            builder.setSsid("Timelapsify");
            builder.setWpa2Passphrase("g3t_th3_fuck_out");

            WifiNetworkSpecifier wifiNetworkSpecifier = builder.build();
            NetworkRequest.Builder networkRequestBuilder1 = new NetworkRequest.Builder();
            networkRequestBuilder1.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            networkRequestBuilder1.setNetworkSpecifier(wifiNetworkSpecifier);

            NetworkRequest nr = networkRequestBuilder1.build();
            ConnectivityManager cm = (ConnectivityManager)
                    getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
            ConnectivityManager.NetworkCallback networkCallback = new
                    ConnectivityManager.NetworkCallback() {
                        public void onAvailable(Network network) {
                            super.onAvailable(network);
                            Log.d("Timelapsify", "available: " + network);
                            cm.bindProcessToNetwork(network);

                            webview.post(new Runnable() {
                                @Override
                                public void run() {
                                    webview.setWebViewClient(new WebViewClient());
                                    webview.clearCache(true);
                                    webview.getSettings().setJavaScriptEnabled(true);
                                    webview.loadUrl("http://192.168.1.1");
                                }
                            });

                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    loader.setVisibility(View.GONE);
                                    webview_wrp.setVisibility(View.VISIBLE);
                                }
                            });
                        }
                    };
            cm.requestNetwork(nr, networkCallback);


        } catch (Exception e) {
            error.setVisibility(View.VISIBLE);
            reload_btn.setVisibility(View.VISIBLE);
            loader_txt.setText("Wifi connection can't establish!");

            Log.e("Timelapsify", e.toString());
        }
    }
}
