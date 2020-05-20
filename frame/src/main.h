/**
 * Dad's status V1.0
 *
 * Copyright (C) 2020 Tomas Vecera tomas@vecera.dev
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
#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#define FIREBASE_HOST "dads-status.firebaseio.com"
#define FIREBASE_AUTH "OAMqLLl2Ow0zCh9armnIVnW3raPVUHQcnpps6kR8"

#define WIFI_SSID "Kraken"
#define WIFI_PASSWORD "Kraken147Kraken147"

// GPI14, D5
#define LED_1_PIN 14
// GPI12, D6
#define LED_2_PIN 12
// GPI13, D7
#define LED_3_PIN 13

#define DATA_CHECK_INTERVAL 60 * 1000

#define STATUS_PATH "/status"
#define ACTIVITY_PATH "activity"
#define LOCATION_PATH "location"
#define LASTCHECK_PATH "/frame/lastcheck"

#endif  // SRC_MAIN_H_
