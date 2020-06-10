#include "commonsExtensions.h"

int list_contains_int(t_list* self, int value){

	_Bool _compare_item_to_search(void* element) {
		return *((int*)element) == value;
	}

	t_list *satisfying = list_filter(self, &_compare_item_to_search);
	int result = satisfying->elements_count > 0 ? 1 : 0;
	list_destroy(satisfying);
	return result;
}
