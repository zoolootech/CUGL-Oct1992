/*
 * df - display bytes free on disk
 * Usage: df [d]
 *
 * David MacKenzie
 * Latest revision: 04/22/88
 */

main(argc, argv)
    int     argc;
    char  **argv;
{
    long    disk_bytes_free();
    long    dbf;

    switch (argc) {
    case 1:
	dbf = disk_bytes_free(0);
	break;
    case 2:
	dbf = disk_bytes_free(argv[1][0]);
	break;
    default:
	printf("Usage: df [d]\n");
	exit(1);
    }

    if (dbf == -1) {
	printf("Bad drive\n");
	exit(1);
    } else {
	printf("%ld bytes free\n", dbf);
	exit(0);
    }
}
