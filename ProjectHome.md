CNB Data Parser

CNB Data Parser reads from a binary data file, usually generated on an embedded system, and creates a spreadsheet from that data. It optionally limits the size of that spreadsheet, and can automatically open it once ready.

The program supports limiting the number of rows output to fit within the limits of common spreadsheet programs. It does this by ignoring one in N rows. For example, if the input data file has 400,000 rows worth of data and the "Limit rows" function is set to limit to 65536 rows (the limit of MS Excel 2003), then only one in six rows will be recorded in the spreadsheet. In this case, only 393,216 values would be stored in the output spreadsheet because 6\*65,536=393,216.

The input values can be voltages or numbers (counters),  each between 1 and 8 bytes (8 and 64 bits).

VOLTAGE VALUES:
If voltages, the low and high can be set at the bottom of the window.
A raw value of "0" will be interpreted as the "low" value, and the maximum value which can be stored in an unsigned integer of the selected number of bytes is considered the "high" value. Intermediate values are interpreted linearly.

For example, if the "low" voltage is set to 0.0, the "high" is 5.0, and the value read is 2 bytes, then a raw value of "0" means 0.0 volts. A raw value of "65535" (max integer that fits in 2 bytes) means "5.0 volts", and a raw value of 32,767 corresponds to about 2.5 volts, which will be recorded in the spreadsheet as "2.50000".

COUNTER VALUES:
To interpret a value as a counter rather than a voltage, check the "count" checkbox for that value. This will make the spreadsheet's output show only the actual integer stored in the data file. For example, if a 1-byte count variable contains "123", then the spreadsheet will show "123".

This program was written in C++ and uses Nokia's Qt libraries. It was originally written to parse data from a rocket payload built at the RockOn NASA workshop. See:
http://spacegrant.colorado.edu/rockon
It is under evaluation for use at that workshop, but this implies no official connection with NASA, RockOn, or any related organization.