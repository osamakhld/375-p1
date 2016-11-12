# Compute first twelve Fibonacci numbers and put in array
.set noreorder 
main:
      lw   $t1,  16($zero)
      sh   $t1, 40
      addi  $t3, $t1, 0   
      .word 0xfeedfeed  
      .word 0x12341111
      .word -0x00000001    
	
	
