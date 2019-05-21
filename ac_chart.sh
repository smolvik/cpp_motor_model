#!/bin/bash

rm freq_resp.dat

AMPL=1

for f in {1..100}
do
	echo $f
	./ac $AMPL $f >> freq_resp.dat
done

./plot_freq_resp_logscale
