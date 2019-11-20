#ifndef ELMETA_PAIR_C
	#define ELMETA_PAIR_C

	namespace el {
		template<typename TFirst, typename TSecond>
		struct pair {
			using First = TFirst;
			using Second = TSecond;

			constexpr pair()
			{
			}

			pair(TFirst &f, TSecond &s):
			first(f), second(s)
			{
			}

			pair(TFirst const &f, TSecond const &s):
			first(f), second(s)
			{
			}

			pair(TFirst &&f, TSecond &&s):
			first(std::move(f)), second(std::move(s))
			{
			}

			TFirst first;
			TSecond second;
		};

		template<typename TFirst, typename TSecond>
		constexpr auto make_pair(TFirst f, TSecond s) noexcept
		{
			return el::pair<TFirst, TSecond>(f, s);
		}
	} // el
#endif // ELMETA_PAIR_C