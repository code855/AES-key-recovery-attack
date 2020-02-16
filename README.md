The files are for the checking of the key-recocvery attacks.
We use a small-scale AES to do the experiments. 
Compared with the normal AES, the gap of probability between the right delta and wrong delta is small.
Therefore, we recall core function more times to exclude the wrong delta.
With 2^{15} chosen plaintexts, the program can extract the key byte very well.

usage: (for Unix/Linux platform )
make
./recoverykeybyte

