#!/bin/bash

# I couldn't put this into a cmake function :(
# this is the function from base_rules
# $1 is an object filename
# like zoom_in_white
# We don't care about extension thanks to stb

fn_w_us=$(echo $1 | sed -e 's/^\([0-9]\)/_\1/' -e 's/[^A-Za-z0-9_]/_/g')
h_fn=$(echo $1 | tr . _)

echo extern const u8  $fn_w_us"_end[];" > $h_fn.h
echo extern const u8  $fn_w_us"[];" >> $h_fn.h
echo extern const u32 $fn_w_us"_size;" >> $h_fn.h
