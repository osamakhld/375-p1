# Compute first twelve Fibonacci numbers and put in array
main:
      lw   $t0, 16($zero)
      lw   $t1, 16($zero)
      add  $t2, $t1, $t0
      .word 0xfeedfeed
      .word 0xff000000
      .word 0x00000001
