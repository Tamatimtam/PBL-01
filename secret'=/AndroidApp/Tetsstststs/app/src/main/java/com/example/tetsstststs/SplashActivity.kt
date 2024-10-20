package com.example.tetsstststs

import android.content.Intent
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.VideoView
import android.os.Handler


class SplashActivity : AppCompatActivity() {

    private val videoPath = "android.resource://com.example.tetsstststs/${R.raw.vid}"
    private lateinit var videoView: VideoView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)

        videoView = findViewById(R.id.videoView)
        playVideo()

        // Use a Handler to delay the transition to the main activity
        Handler().postDelayed({
            // Start the main activity
            val intent = Intent(this@SplashActivity, MainActivity::class.java)
            startActivity(intent)

            // Close the splash activity to prevent going back to it
            finish()
        }, getVideoDuration())
    }

    private fun playVideo() {
        val videoUri = Uri.parse(videoPath)
        videoView.setVideoURI(videoUri)
        videoView.setOnPreparedListener { mp ->
            mp.isLooping = true
            videoView.start()
        }
    }

    private fun getVideoDuration(): Long {
        // Return the duration of your video in milliseconds
        return 2500 // 5 seconds as an example
    }
}
