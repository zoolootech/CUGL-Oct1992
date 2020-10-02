/* [BTREE.C of JUGPDS Vol.19]	*/

/* B-Tree search, insertion and deletion
   from ALGORITHMS+DATA STRUCTURES=PROGRAMS by N. Wirth
   implemented for BDS C by H. Katayose (JUG-CP/M No.179) */

#define	NN	8
#define	N	(NN/2)
#define	PAGE	struct _page
#define	ITEM	struct _item
#define	NULL	0
#define	TRUE	(-1)
#define	FALSE	0

/*
 * Storage allocation data, used by "alloc" and "free"
 */

struct _header  {
	struct _header *_ptr;
	unsigned _size;
 };

struct _header _base;		/* declare this external data to  */
struct _header *_allocp;	/* be used by alloc() and free()  */


struct	_item {
	int	key;
	PAGE	*right_ref;
	int	count;
};

struct	_page {
	int	item_cnt;
	PAGE	*left_ref;
	ITEM	e[NN];
};

PAGE	w;

main()
{
	int	i;
	int	x, h;				/* h : boolean */
	PAGE	*q, *root;
	ITEM	u;

	root = NULL;
	scanf("%d", &x);
	while (x) {
		printf("SEARCH KEY %d\n", x);
		if (search(x, root, &u)) {
			q = root;
			new(&root);
			root->item_cnt = 1;
			root->left_ref = q;
			itemcopy(root->e[0], u);
		}
		printtree(root, 0);
		scanf("%d", &x);
	}
	scanf("%d", &x);
	while (x) {
		printf("DELETE KEY %d\n", x);
		if (delete(x, root)) {
			if (root->item_cnt == 0) {
				q = root;
				root = q->left_ref;
			}
		}
		printtree(root, 0);
		scanf("%d", &x);
	}
}


search(x, a, v)
PAGE	*a;
ITEM	*v;
{
	int	i;
	ITEM	u;

	if (a == NULL) {
		v->key = x;
		v->count = 1;
		v->right_ref = NULL;
		return TRUE;
		}
	for (i = 0; i < a->item_cnt && x > a->e[i].key; i++)
		;
	if (x == a->e[i].key && i < a->item_cnt)
		a->e[i].count++;
	else
		if (search(x, i ? a->e[i-1].right_ref : a->left_ref, &u))
			return insert(a, i, &u, v);
	return FALSE;
}


insert(a, i, u, v)
PAGE	*a;
ITEM	*u, *v;
{
	PAGE	*b;
	int	j, h;

	if (a->item_cnt < NN) {
		for (j = a->item_cnt++; j >= i+1; j--)
			itemcopy(a->e[j], a->e[j-1]);
		itemcopy(a->e[i], u);
		return FALSE;
	} else {			/* page a is full; split it and
					   assign the emerging item to v */
		new(&b);
		if (i <= N) {
			if (i == N)
				itemcopy(v, u);
			else {
				itemcopy(v, a->e[N-1]);
				for (j = N-1; j >= i+1; j--)
					itemcopy(a->e[j], a->e[j-1]);
				itemcopy(a->e[i], u);
			}
			for (j = 0; j <= N-1; j++)
				itemcopy(b->e[j], a->e[j+N]);
		} else {
			i -= N;
			itemcopy(v, a->e[N]);
			for (j = 0; j <= i - 2; j++)
				itemcopy(b->e[j], a->e[j+N+1]);
			itemcopy(b->e[i-1], u);
			for (j = i; j <= N-1; j++)
				itemcopy(b->e[j], a->e[j+N]);
		}
		a->item_cnt = b->item_cnt = N;
		b->left_ref = v->right_ref;
		v->right_ref = b;
	}
	return TRUE;
}


itemcopy(d, s)
ITEM	*d, *s;
{
	d->key = s->key;
	d->right_ref = s->right_ref;
	d->count = s->count;
}


new(p)
PAGE	**p;
{
	PAGE	*alloc();

	if ((*p = alloc(sizeof **p)) == NULL)
		exit();
}


printtree(p, l)
PAGE	*p;
{
	int	i;

	if (p != NULL) {
		for (i = 0; i <= l; i++)
			printf("    ");
		for (i = 0; i < p->item_cnt; i++)
			printf("%4d", p->e[i].key);
		putchar('\n');
		printtree(p->left_ref, l+1);
		for (i = 0; i < p->item_cnt; i++)
			printtree(p->e[i].right_ref, l+1);
	}
}


delete(x, a)
PAGE *a;
{
	int	i;
	int	k, l, r;
	PAGE	*q;

	if (a == NULL) {
		printf("Key is not in tree!\n");
		return FALSE;
	} else {				/* binary array search */
		for (l = 0, r = a->item_cnt - 1; l <= r; ) {
			k = (l + r)/2;
			if (x <= a->e[k].key) r = k-1;
			if (x >= a->e[k].key) l = k+1;
		}
		q = (r == -1) ? a->left_ref : a->e[r].right_ref;
		if (l - r > 1)			/* found, now delete e[k] */
			if (q == NULL) {	/* a is a terminal page   */
				a->item_cnt--;
				for (i = k; i < a->item_cnt; i++)
					itemcopy(a->e[i], a->e[i+1]);
				return (a->item_cnt < N);
				}
			else {
				if (del(q, a, k))
					return underflow(a, q, r);
				}
		else {
			if (delete(x, q))
				return underflow(a, q, r);
			}
		}
}


underflow(c, a, s)
PAGE	*c, *a;
int	s;
{
	PAGE *b;
	int i, k, mb, mc;

	mc = c->item_cnt;		/* h : true, a->item_cnt = n - 1 */
	if (s < mc-1) {
		s++;
		b = c->e[s].right_ref;
		mb = b->item_cnt;
		k = (mb - N + 1)/2;
		itemcopy(a->e[N-1], c->e[s]);
		a->e[N-1].right_ref = b->left_ref;
		if (k > 0) {
			for(i = 0; i < k-1; i++)
				itemcopy(a->e[i+N], b->e[i]);
			itemcopy(c->e[s], b->e[k-1]);
			c->e[s].right_ref = b;
			b->left_ref = b->e[k-1].right_ref;
			mb -= k;
			for (i = 0; i < mb; i++)
				itemcopy(b->e[i],b->e[i+k]);
			b->item_cnt = mb;
			a->item_cnt = N-1+k;
			return FALSE;
			}
		else {
			for (i = 0; i <  N; i++) itemcopy(a->e[i+N], b->e[i]);
			for (i = s; i < mc; i++) itemcopy(c->e[i], c->e[i+1]);
			a->item_cnt = NN;
			c->item_cnt = mc-1;
			}
		}
	else {
		b = (s == 0) ? c->left_ref : c->e[s-1].right_ref;
		mb = b->item_cnt + 1;
		k = (mb - N) / 2;
		if (k > 0) {
			for(i = N-2; i >= 0; i--)
				itemcopy(a->e[i+k], a->e[i]);
			itemcopy(a->e[k-1], c->e[s]);
			a->e[k-1].right_ref = a->left_ref;
			mb -= k;
			for (i = k-2; i>=0; i--) itemcopy(a->e[i], b->e[i+mb]);
			a->left_ref = b->e[mb].right_ref;
			itemcopy(c->e[s], b->e[mb-1]);
			c->e[s].right_ref = a;
			b->item_cnt = mb - 1;
			a->item_cnt = N-1+k;
			return FALSE;
			}
		else {
			itemcopy(b->e[mb], c->e[s]);
			b->e[mb].right_ref = a->left_ref;
			for (i = 0; i < N-1; i++)
				itemcopy(b->e[i+mb], a->e[i]);
			b->item_cnt = NN;
			c->item_cnt = mc-1;
			}
	}
	return TRUE;
}


del(p, a, k)
PAGE	*p, *a;
{
	PAGE	*q;

	if ((q = p->e[p->item_cnt-1].right_ref) != NULL) {
		if (del(q, a, k))
			return underflow(p, q, p->item_cnt-1);
		}
	else {
		p->e[p->item_cnt-1].right_ref = a->e[k].right_ref;
		itemcopy(a->e[k], p->e[p->item_cnt-1]);
		p->item_cnt--;
		return (p->item_cnt < N);
	}
}
