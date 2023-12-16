# PMN image contrast
Given an image in PNM format(P5 of P6), program increases image contrast 
by stretching colour range. To deal with high contrast pixels, program can skip 
provided percent of 'low-contrast' and 'high-contrast' pixels.

To improve performance, OpenMP library was used.

# Usage
`contrast_fix threads input_file output_file delta`

Where:
- `threads` is the number of threads used for computations
- `input_file` is a path to PNM image for auto-contrasting
- `output_file` is a destination file name
- `delta` is a float value from 0 to 1, which shows, how many 
pixels should be removes from color range computation