global io_hlt, io_out8, io_in8
global intr1

extern intr1_handler

io_hlt:
  hlt
  ret

io_out8:
  mov   dx, [esp + 4]
  mov   ax, [esp + 6]
  out   dx, ax
  ret

io_in8:
  mov   dx, [esp + 4]
  in    ax, dx
  ret

intr1:
  jmp   intr1
  push  rax
  push  rcx
  push  rdx
  push  rbx
  push  rbp
  push  rsi
  push  rdi
  push  rsp
  call  intr1_handler
  pop  rsp
  pop  rdi
  pop  rsi
  pop  rbp
  pop  rbx
  pop  rdx
  pop  rcx
  pop  rax
  iretq
