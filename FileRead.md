The FileRead Program simulates ultrahigh speed acquisition by file reading.

# Introduction #

Contains the main GPU engine code in CUDA for processing and display. This applies for both Spectral Domain OCT (SD) and Swept Source OCT (SS), as contained in the zipped folder.

# Details #

The thread library used is a very light-weight thread library we found online. Otherwise, only CUDA installation is required for this program to operate, which can be foudn on the NVIDIA website for CUDA developpers.
  * This code is for CUDA 4.2, and may not be compatible with older or newer versions of CUDA