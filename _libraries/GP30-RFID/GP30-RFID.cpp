/* mbed ID12 RFID Library
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "GP30-RFID.h"

#include "mbed.h"

GP30RFID::GP30RFID(PinName rx)
    : _rfid(NC, rx)
{
}

int GP30RFID::readable()
{
 if (_rfid.readable()) {
   read(); 
   return 1;
  }
  else
  return 0; 
}

int GP30RFID::read()
{
    char c=0;
    int v = 0;
    while (c!=CR) {
        c=_rfid.getc();
        if (c>='0'){
         TAG[v]=c;
         if (v<Taglength) v++;
        } 
    }
    //TAG[Taglength]=0; //null termination

    return v;
}
