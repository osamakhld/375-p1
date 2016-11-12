# Compute first twelve Fibonacci numbers and put in array
.set noreorder 
main: 
      lw   $t1,  16($zero)
      lw   $t2, 20($zero)
      addi  $t3, $t1, 0   
      .word 0xfeedfeed  
      .word 0x80000000
      .word -0x00000001    
	
	
