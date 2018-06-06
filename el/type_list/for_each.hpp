#ifndef ELMETA_FOR_EACH_HPP
	#define ELMETA_FOR_EACH_HPP

	namespace el {
		namespace impl {
			template<typename TList, typename TF, typename ...Args>
			void for_each(el::size_c<(TList::size - 1)> i, TF&& f, Args&&... args)
			{
				f(
					el::type_c<typename TList::template At<(TList::size - 1)>>,
					i, std::forward<Args>(args)...
				);
			}

			template<typename TList, std::size_t Idx, typename TF, typename ...Args>
			void for_each(el::size_c<Idx> i, TF&& f, Args&&... args)
			{
				f(
					el::type_c<typename TList::template At<Idx>>,
					i, std::forward<Args>(args)...
				);
				for_each<TList>(
					el::size_c<Idx + 1>(),
					std::forward<TF>(f),
					std::forward<Args>(args)...
				);
			}
		} // impl
	} // el
#endif // ELMETA_FOR_EACH_HPP