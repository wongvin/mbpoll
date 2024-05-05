/* Copyright © 2015-2019 Pascal JEAN, All rights reserved.
 *
 * mbpoll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mbpoll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mbpoll.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef MBPOLL_GPIO_RTS

#ifdef USE_LIBGPIOD
#include <gpiod.h>
#include <unistd.h>
#else
#include <piduino/Arduino.h>
#endif /* USE_LIBGPIOD defined */

#include <stdio.h>
#include "custom-rts.h"

// -----------------------------------------------------------------------------
#ifdef USE_LIBGPIOD
const char *chipname = "gpiochip0";
static struct gpiod_chip *_chip;
static struct gpiod_line *_lineTXEN;    // TXEN
#else
static int _num = -1;
#endif /* USE_LIBGPIOD defined */

static int _rev = false;


// -----------------------------------------------------------------------------
int
init_custom_rts (int num, int act) {
  _rev = !act;

#ifdef USE_LIBGPIOD
  int ret;

  // Open GPIO chip
  _chip = gpiod_chip_open_by_name(chipname);
  if (!_chip) {
    perror("Open chip failed\n");
    return 1;
  }

  // Open GPIO line
  _lineTXEN = gpiod_chip_get_line(_chip, num);
  if (!_lineTXEN) {
    perror("Get line failed\n");
    return 1;
  }

  // Open GPIO line for output
  ret = gpiod_line_request_output(_lineTXEN, "mbpoll", 0);
  if (ret < 0) {
    perror("Request line as output failed\n");
    return 1;
  }
#else
  _num = num;

  pinMode (_num, OUTPUT);
#endif /* USE_LIBGPIOD defined */

  set_custom_rts (NULL, false);

  return 0;
}

// -----------------------------------------------------------------------------
void
set_custom_rts (modbus_t *ctx, int on) {
#ifdef USE_LIBGPIOD
  int ret;

  ret = gpiod_line_set_value(_lineTXEN, (on != 0) ^ _rev);

  if (ret < 0) {
    perror("Set line output failed\n");
  }
#else
  digitalWrite (_num, (on != 0) ^ _rev);
#endif /* USE_LIBGPIOD defined */
}

// -----------------------------------------------------------------------------
void
end_custom_rts (void) {
#ifdef USE_LIBGPIOD
  gpiod_line_release(_lineTXEN);
  gpiod_chip_close(_chip);
#endif /* USE_LIBGPIOD defined */
}
// -----------------------------------------------------------------------------
#endif /* MBPOLL_GPIO_RTS defined */
/* ========================================================================== */
