addi a0 zero 3
jal 4
sw v0 zero 0
j 10000
addi t0 zero 2
slt t0 a0 t0
beq t0 zero 2
add v0 a0 zero
jr ra
addi sp sp -3
sw ra sp 0
sw a0 sp 1
sw s0 sp 2
addi a0 a0 -1
jal 4
add s0 v0 zero
lw a0 sp 1
addi a0 a0 -2
jal 4
add v0 v0 s0
lw ra sp 0
lw a0 sp 1
lw s0 sp 2
addi sp sp 3
jr ra