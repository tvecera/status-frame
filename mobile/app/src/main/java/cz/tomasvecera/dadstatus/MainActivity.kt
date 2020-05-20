/*
 * Dad's status V1.0
 *
 * Copyright (c) 2020 Tomas Vecera tomas@vecera.dev
 *
 * This code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this code.  If not, see <http://www.gnu.org/licenses/>.
 */
package cz.tomasvecera.dadstatus

import android.os.Bundle
import android.util.Log
import android.widget.ImageButton
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import com.google.firebase.Timestamp
import com.google.firebase.database.*
import kotlinx.android.synthetic.main.activity_main.*
import java.time.LocalDateTime
import java.time.ZoneId
import java.time.format.DateTimeFormatter

const val FB_PATH_STATUS = "status"
const val FB_PATH_LOCATION = "location"
const val FB_PATH_ACTIVITY = "activity"
const val FB_PATH_LASTCHECK = "frame/lastcheck"

const val TAG_FIREBASE = "firebase"
const val TAG_ACTION = "action"

class MainActivity : AppCompatActivity() {

    private val data = FirebaseDatabase.getInstance()
    private val dataRef = data.reference
    private val statusRef = dataRef.child(FB_PATH_STATUS)
    private val lastcheckRef = dataRef.child(FB_PATH_LASTCHECK)
    private val formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initFirebaseDatabase()
        initLayout()
    }

    private fun clearButtons() {
        Log.d(TAG_ACTION, "Clear all buttons.")
        dndButton.clearColorFilter()
        workButton.clearColorFilter()
        transportButton.clearColorFilter()
        homeButton.clearColorFilter()
    }

    private fun activateButton(btn: ImageButton) {
        Log.d(TAG_ACTION, "Change button state.")
        clearButtons()
        btn.setColorFilter(
            ContextCompat.getColor(
                applicationContext,
                R.color.activeButton
            )
        )
    }

    private fun addStatusDataListener(ref: DatabaseReference) {
        ref.addValueEventListener(object : ValueEventListener {
            override fun onCancelled(e: DatabaseError) {
                Log.w(TAG_FIREBASE, "Failed to read value.", e.toException())
            }

            override fun onDataChange(d: DataSnapshot) {
                val activity = d.child(FB_PATH_ACTIVITY).value
                val location = d.child(FB_PATH_LOCATION).value
                Log.d(TAG_FIREBASE, "Activity value: $activity, Location value: $location")

                if (activity == ActivityEnum.DND.name) {
                    activateButton(dndButton)
                } else {
                    when (location) {
                        LocationEnum.WORK.name -> activateButton(workButton)
                        LocationEnum.TRANSPORT.name -> activateButton(transportButton)
                        LocationEnum.HOME.name -> activateButton(homeButton)
                    }
                }
            }
        })
    }

    private fun convertFirebaseTimestamp(timestamp: Any?): String {
        var result = getString(R.string.not_available)
        if (timestamp != null && timestamp is Long) {
            val tt = timestamp / 1000
            result = LocalDateTime.ofInstant(
                Timestamp(tt, 0).toDate().toInstant(),
                ZoneId.systemDefault()
            ).format(formatter)
        }
        return result
    }

    private fun addLastFrameCheckDataListener(ref: DatabaseReference, tx: TextView) {
        ref.addValueEventListener(object : ValueEventListener {
            override fun onCancelled(e: DatabaseError) {
                Log.w(TAG_FIREBASE, "Failed to read value.", e.toException())
            }

            override fun onDataChange(d: DataSnapshot) {
                val timestamp = d.value
                Log.d(TAG_FIREBASE, "Last connection of frame: $timestamp")
                tx.text = convertFirebaseTimestamp(timestamp)
            }
        })
    }

    private fun setDataValue(activity: ActivityEnum, location: LocationEnum?) {
        if (location != null) {
            statusRef.child(FB_PATH_LOCATION).setValue(location)
        }
        statusRef.child(FB_PATH_ACTIVITY).setValue(activity)
    }

    private fun initFirebaseDatabase() {
        Log.d("firebase", "Init firebase database listeners.")
        addStatusDataListener(statusRef)
        addLastFrameCheckDataListener(lastcheckRef, lastFrameCheckView)
    }

    private fun initLayout() {
        Log.d(TAG_ACTION, "Init layout.")
        val dndButton = findViewById<ImageButton>(R.id.dndButton)
        val workButton = findViewById<ImageButton>(R.id.workButton)
        val transportButton = findViewById<ImageButton>(R.id.transportButton)
        val homeButton = findViewById<ImageButton>(R.id.homeButton)
        val lastFrameCheckText = findViewById<TextView>(R.id.lastFrameCheckView)
        lastFrameCheckText.text = getString(R.string.not_available)

        dndButton.setOnClickListener {
            activateButton(dndButton)
            setDataValue(ActivityEnum.DND, null)
        }
        workButton.setOnClickListener {
            activateButton(workButton)
            setDataValue(ActivityEnum.WORK, LocationEnum.WORK)
        }
        transportButton.setOnClickListener {
            activateButton(transportButton)
            setDataValue(ActivityEnum.TRANSPORT, LocationEnum.TRANSPORT)
        }
        homeButton.setOnClickListener {
            activateButton(transportButton)
            setDataValue(ActivityEnum.FREE, LocationEnum.HOME)
        }
    }
}
