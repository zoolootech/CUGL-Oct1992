char  *strchr(const char *String, char Char)
{
  for (; *String; ++String)
    if (*String==Char)
      return  (char *) String;
  return  NULL;
}
