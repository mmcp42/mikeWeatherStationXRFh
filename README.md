mikeWeatherStationXRFh
======================
first release of weather station using XRF for communication

include files used
- #include \<DHT22.h\>            humidity sensor library
- #include \<EEPROM.h\>           standard Arduino EEPROM access (for storing and retrieving parameters)
- #include "DataRecord.h"       defines shape of MY data record
- #include \<Sodaq_dataflash.h\>  access to on-board flash data
- #include \<Sodaq_DS3231.h\>     access to on-board RTC
- #include \<Barometer.h\>        barometer sensor library
- #include \<Wire.h\>             standard Arduino Wire library
- #include "diag.h"             diagnostics support

it also needs
- wxXRF.py                  python file to receive the data and send it on to the database server
- wx.html                   html file to display current weather station readings
- wxgraphs.html             html file to show historical graphical data
- ts.php                    php script that supplies current UNIX epoch value for initialising the RTC
- wxPutRecord.php           php script to receive one record from python script and write it to the database
- wxGetRecord.php           php script to retrieve records from the database for display
- steelseries-min.js        javascript file to support drawing of graphical dials and guages
- tween-min.js              javascript file to support drawing of graphical dials and guages
- RGraph.common.core.js     javascript file to support drawing of charts
- RGraph.common.dynamic.js  javascript file to support drawing of charts
- RGraph.common.key.js      javascript file to support drawing of charts
- RGraph.line.js            javascript file to support drawing of charts
