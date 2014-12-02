#ifndef NODE
#error "Define NODE macro prior to including " __FILE__
#endif


#ifndef PREFIX
#define PREFIX(x) x
#endif

#define P(x) PREFIX(x)


struct P(wrapper) {
	struct P(wrapper) *next;
	NODE n;
};


struct P(list) {
	struct P(wrapper) *head, *tail;
	uint count;
};


void
P(list_init)(struct P(list) *lst) {
	lst->head = lst->tail = NULL;
	lst->count = 0;
}


static struct P(wrapper) *
wrapper_new(NODE n) {
	struct P(wrapper) *w = malloc_safe(sizeof(struct P(wrapper)));
	// error if w == NULL

	w->next = NULL;
	w->n = n;

	return (w);
}


void
P(list_append)(struct P(list) *lst, NODE n) {
	if (lst->head == NULL) lst->head = lst->tail = wrapper_new(n);
	else lst->tail = lst->tail->next = wrapper_new(n);

	lst->count++;
}


NODE
P(list_shift)(struct P(list) *lst) {
	struct P(wrapper) *head = lst->head;
	if (head) {
		NODE n = head->n;
		lst->head = head->next;
		free(head);

		return (n);
	}
	
	return (NULL);
}


void
P(list_free)(struct P(list) *lst) {
	struct P(wrapper) *w = lst->head, *tmp;
	while (w != NULL) {
		tmp = w;
		w = w->next;

		free(tmp);
	}
}


#undef P

#undef NODE
#undef PREFIX
