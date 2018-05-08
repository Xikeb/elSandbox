#ifndef EL_LAST_H
#define EL_LAST_H

namespace el {
	template<el::type_list L>
	struct last: el::conditional<
		el::is_same<L::next, el::nothing>(),
		L,
		last<L::next>
	> {

	};

	typedef struct last last;
} // el

#endif // EL_LAST_H
