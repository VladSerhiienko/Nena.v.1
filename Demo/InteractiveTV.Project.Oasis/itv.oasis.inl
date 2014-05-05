
__forceinline InteractiveTV::Oasis *InteractiveTV::Oasis::GetForCurrentThread()
{
	static Oasis s_oasis; return &s_oasis;
}