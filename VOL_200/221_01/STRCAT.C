/*
**  concat str2 to str1
*/
strcat(str1, str2) char *str1, *str2; {
  while(*str1++);
  --str1;
  while(*str1++ = *str2++);
}
