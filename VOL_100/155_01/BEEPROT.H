/* btree function prototype		*/

FILE *open_keyfile(
	char *filename, 
	struct keyinfo *fileinfo
	);   /* used to open keyfile */

void close_keyfile(
	struct keyinfo *fileinfo
	);   /* write out header information and close file */

int write_node(
	long nbr, 
	struct node *nodeinfo, 
	struct keyinfo *fileinfo
	);   /* write a node's info to file */

void print_node(
	struct node *node1
	);  /* display contents of a node on screen (debug) */

void push_left_stack(
	long nbr
	);  /* moved left in tree - record this in stack */

void push_right_stack(
	long nbr
	);  /* moved right in tree - record this in stack */

long pop_right_stack();

long pop_left_stack();

void get_node(
	long nbr, 
	struct node *nodeinfo, 
	struct keyinfo *fileinfo
	);  /* read the info stored in node NBR */

int insert(
	char *argkey, 
	long recnbr, 
	struct keyinfo *fileinfo
	);  /* insert key (argkey) into tree */

void link(
	int alpha1, 
	struct node *node1, 
	int alpha2, 
	struct node *node2
	);

void nbr_link(
	long *nbr, 
	int alpha, 
	struct node *node1
	);  /* set a record number according to alpha */

void link_nbr(
	int alpha, 
	struct node *node1, 
	long nbr
	); /* set a link according to alpha */

void node_bal(
	int alpha, 
	struct node *node1, 
	struct node *node2, 
	struct node *node3
	);  /* node balancing in Step A9 */

void delete_key(
	long node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	);

int get_next(
	long *node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	);  /* retrieve next higher node */

int find_key(
	char *key1,
	long *node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	); /* locate a key */

int get_last(
	long *node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	); /* get a last record */

int get_first(
	long *node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	); /* get a first record */

int get_previous(
	long *node_nbr, 
	struct node *current_node, 
	struct keyinfo *fileinfo
	);
/*-------- end of beeprot.h -------------*/
