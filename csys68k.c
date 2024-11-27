extern void outbyte(unsigned char c);
extern char inbyte();

int read(int fd, char *buf, int nbytes)
{
  char c;
  int  i;

  for (i = 0; i < nbytes; i++) {
    c = inbyte();

    if (c == '\r' || c == '\n'){ /* CR -> CRLF */
      outbyte('\r');
      outbyte('\n');
      *(buf + i) = '\n';

    /* } else if (c == '\x8'){ */     /* backspace \x8 */
    } else if (c == '\x7f'){      /* backspace \x8 -> \x7f (by terminal config.) */
      if (i > 0){
	outbyte('\x8'); /* bs  */
	outbyte(' ');   /* spc */
	outbyte('\x8'); /* bs  */
	i--;
      }
      i--;
      continue;

    } else {
      outbyte(c);
      *(buf + i) = c;
    }

    if (*(buf + i) == '\n'){
      return (i + 1);
    }
  }
  return (i);
}

int write (int fd, char *buf, int nbytes)
{
  int i, j;
  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      outbyte ('\r');          /* LF -> CRLF */
    }
    outbyte (*(buf + i));
    for (j = 0; j < 300; j++);
  }
  return (nbytes);
}
