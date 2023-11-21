package com.example.tetsstststs
import android.os.Bundle
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val webView: WebView = findViewById(R.id.webView)
        val webSettings = webView.settings

        // Enable JavaScript (optional)
        webSettings.javaScriptEnabled = true

        // Set up WebViewClient to handle URL loading within the WebView
        webView.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(
                view: WebView?,
                request: WebResourceRequest?
            ): Boolean {
                // Return false to indicate that the WebView should handle the URL loading
                return false
            }
        }

        // Load a website URL
        webView.loadUrl("https://26ec-2a09-bac5-3a08-18d2-00-279-1.ngrok-free.app ")
    }
}
