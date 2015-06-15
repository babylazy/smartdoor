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
 
#ifndef MBED_GP30RFID_H
#define MBED_GP30RFID_H
 
#include "mbed.h"

/** An interface for the GP-30 RFID reader device
 */
#define Taglength 10
#define CR 0x0d
#define LF 0x0a
#define STX 0x02
/** GP30 interface*/
class GP30RFID {

public:
    /** Create an GP-30 RFID interface, connected to the specified Serial rx port
     *
     * @param rx Recieve pin 
     */
    GP30RFID(PinName rx);

    /** A blocking function that will return a tag ID when available
     *
     * @return Non zero value when the device is readable
     */
    int readable();    
    
    /**
     last tag read
    */
    char TAG[Taglength+1];
private:
    /** A blocking function that will return a tag ID when available
     *
     * @Put the tag value in TAG
     */
    int read();

    Serial _rfid;
    
};

#endif
