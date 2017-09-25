.text
	addu	$s0,$zero,$zero		# s0 = 0
	addu	$s1,$zero,$zero		# s1 = 0
   	addiu	$v0,$zero,5		# v0 = read value 
	syscall
goof:    or  $t0,$s1,$s2
    mult    $a0,$a1
    slt $ra,$t8,$t9
    addiu   $t8,$k0,10
    addu $t0,$t1,$t2
    and $k0,$k1,$gp
Xi: beq $s0,$s4,L1
    bne $s0,$s4,Xi
    div $fp,$v1
tan:    j goof
    lw  $v0,n($gp)
    lw  $v1,66($gp)
    lw  $v1,66($gp)
    lw  $s5,c($gp)
    lw  $a0,k($gp)
    j tan
    mfhi    $s0
    mfhi    $zero
    mfhi    $a3
    mflo    $a3
    mflo    $t8
    mult    $t8,$t9
    or  $a3,$a1,$v0
    slt $a3,$a3,$s6
    subu $a1,$t2,$s3
    sw  $a0,c($gp)
    sw  $a0,c($gp)
    sw  $t0,9($gp)
    sw  $v1,p($gp)
    sw  $at,3($gp)
    syscall
	sw	$v0,n($gp)		# M[n] = v0
L1:	lw	$s2,n($gp)		# s2 = M[n]
	slt	$t0,$s1,$s2		# if s1 >= s2 then
	beq	$t0,$zero,L2		#    goto L2
   	addiu	$v0,$zero,22		# v0 = read value
	syscall
	addu	$s0,$s0,$v0		# s0 += v0
	addiu	$s1,$s1,1		# s1 += 1
	j	L1			# goto L2
L2:	addu	$a0,$s0,$zero		# a0 = s0
	addiu	$v0,$zero,1		# print a0
	syscall
	addiu	$v0,$zero,10		# exit
	syscall
.data
n:	.word	0
    .word 5,6,7
    .space 3
k:  .word -5
c:  .space 2
p:  .word 99
    .space 1
    .word 1
