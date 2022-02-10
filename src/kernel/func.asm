global io_hlt, io_out8, io_in8
global loadIDT

io_hlt:
  hlt
  ret

io_out8: ; 8bit port, 8bit data
  mov   dx, di
  mov   al, sil
  out   dx, al
  ret

io_in8:
  mov   dx, di
  in    al, dx
  ret

loadIDT:
  push  rbp
  mov   rbp, rsp
  sub   rsp, 10
  mov   [rsp], di
  mov   [rsp+2], rsi
  lidt  [rsp]
  add   rsp, 10
  pop   rbp
  ret
