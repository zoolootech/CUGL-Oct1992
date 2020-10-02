flush_key()
/* This will flush the keyboard buffer */
{
	while(bioskey(1) != 0) bioskey(0);
}
